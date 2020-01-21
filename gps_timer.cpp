/*************************************************************************
* Title: GPS Lap Timer.
* File: gps_timer.cpp
* Author: James Eli
* Date: 1/11/2019
*
* Program parses an RMC sentence from a gps connected to a usb port to time 
* consequetive laps. The extracts time, location and fix information from the 
* NMEA RMC sentence. When initiated the program waits in a loop until the GPS 
* acquires a valid fix. Then simply establish a start/finish line by pressing
* a key. Subsequent passing of the start/finish line will display a time for 
* each lap. A basic data file of several laps is available for testing 
* purposes (FILE_INPUT). The final version of this program is intended to run 
* on an Arduino based uC device.
*
* The following is an example NMEA RMC sentence:
*   $GPRMC,220131.60,A,3203.55750,N,11041.03710,W,0.078,,030120,,,A*68
*
* Notes:
*  (1) Compiled with MS Visual Studio 2017 Community (v141).
*************************************************************************
* Change Log:
*   01/11/2019: Initial release. JME
*************************************************************************/

#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <cstdint>
#include <cassert>
#include <math.h>
#include <array>
#include "serial_port.h" // com port routines.
#include "gps_timer.h"   // our header.
#include "utility.h"     // utility functions.
#include "gps.h"         // gps specific functions.
#include <conio.h>       // _kbhit, _getchar.

// Prepends s onto d. Assumes d has enough space allocated for the combined string.
static void Prepend(char* d, const char* s)
{
	assert(s != nullptr && d != nullptr);
	
	size_t len = strlen(s);

	memmove(d + len, d, strlen(d) + 1);
	memcpy(d, s, len);
}

// Convert float seconds to MM::SS.SS format.
static void DisplayTime(const uint8_t n, const float ft) 
{
	assert(ft > 0.);

	char s1[16];

	memset(&s1[10], 0, 6);
	uint16_t m = (uint16_t)ft / 60;
	float fs = ft - (m * 60);
	sprintf(s1, "%.02d:%05.2f ", m, fs);

	// Prepend lap number.
	if (n)
	{
		char s2[6];

		sprintf(s2, "%d: ", n);
		Prepend(s1, s2);
	}

	std::cout << s1;
}

static float EstablishStartLine(char *tokens[])
{
	float ts;

#ifdef FILE_INPUT
	
	// Safeway parking lot: $GPRMC,194924.80,A,3203.02116,N,11042.41425,W,1.304,30.95,120120,,,A*48
	startPoint.x = 32.0302116;
	startPoint.y = 110.4241425;
	// Heading while crossing start/finish.
	startHeading = 31;
	// Position timestamp.
	char t[] = "194924.80";
	ts = ConvertToSeconds(t);

#else

	if (tokens[RMC_TIME] == nullptr || tokens[RMC_TRACK] == nullptr ||
	    tokens[RMC_LATITUDE] == nullptr || tokens[RMC_LONGITUDE] == nullptr)
		return 0.0f;

	// Position timestamp.
	ts = ConvertToSeconds(tokens[RMC_TIME]);

	// Get current track position (lat, long).
	char temp[12];
	GeoCopy(tokens[RMC_LATITUDE], temp, LATITUDE);
	startPoint.x = atof_(temp);
	GeoCopy(tokens[RMC_LONGITUDE], temp, LONGITUDE);
	startPoint.y = atof_(temp);

	// Heading while crossing start/finish.
	startHeading = atoi(tokens[RMC_TRACK]);

#endif

	// Define startline.
	StartLine((float)startPoint.x, (float)startPoint.y, (float)startHeading);
	track.p0.x = startPoint.x;
	track.p0.y = startPoint.y;

	return ts;
}

static void Run(const PORT port, float timeStamp, char *tokens[])
{
	// Lap counters.
	uint8_t numLaps = 0;
	uint16_t hzCounter = 1;
	// Lap data.
	std::array<lap, 256> lapData;
	// Best lap time (lap #, time).
	std::pair<uint8_t, float> bestTime(0, 0.0f);

	uint8_t ticToc = 0;
	unsigned char clock[2] = { 47, 92 };

	// Note timestamp of startline point.
	lapData[numLaps].setStart(timeStamp);

	// Main gps string processing loop.
	while (1)
	{
		if (!GetRMCSentence(port, tokens))
		{

#ifdef FILE_INPUT
			
			if (error.GetError() == err::ID::FILE_EOF)
				return;

#endif

			std::cout << error.GetDescription() << std::endl;
			continue;
		}
		else
			std::cout << clock[++ticToc & 0x01] << '\r';

		// Previous position gps time stamp.
		float prevTimeStamp = timeStamp;

		// Confirm sentence is sequential.
		timeStamp = ConvertToSeconds(tokens[RMC_TIME]);
		if (!Equal(timeStamp, prevTimeStamp + GPS_UPDATE_PERIOD))
		{
			error.SetError(err::ID::TIME_STAMP);
			std::cout << error.GetDescription() << std::endl;
			continue;
		}

		// Get current track position (lat, long).
		if (tokens[RMC_LATITUDE] != nullptr || tokens[RMC_LONGITUDE] != nullptr)
		{
			char temp[12];

			GeoCopy(tokens[RMC_LATITUDE], temp, LATITUDE);
			track.p1.x = atof_(temp);
			GeoCopy(tokens[RMC_LONGITUDE], temp, LONGITUDE);
			track.p1.y = atof_(temp);
		}
		else
			continue;

		// Ignore gps sentences for 1 second after crossing start/finish.
		if (hzCounter < GPS_UPDATE_FREQUENCY)
		{
			hzCounter++;


			// Prepare for next iteration.
			track.p0.x = track.p1.x;
			track.p0.y = track.p1.y;
			continue;
		}
		
		// Heading sanity check & check if crossed start/finish line?
		if (Within30(startHeading, (uint16_t)atol(tokens[RMC_TRACK])) && LineIntersection(track))
		{
			point_t intersectPoint;

			// Calculate track/start line intersection point.
			IntersectPoint(track.p0, track.p1, &intersectPoint);

			// Overall length of this track segment.
			float totDist = Distance(track.p0, track.p1);
			// Length from start line intersection point to track segment end point.
			float segDist = Distance(intersectPoint, track.p1);

			// Calculate startline crossing time for this and next lap.
			float xTime = timeStamp - (GPS_UPDATE_PERIOD * (segDist / totDist));
			lapData[numLaps].setStop(xTime);
			lapData[numLaps + 1].setStart(xTime);

			// Determine current lap stats.
			DisplayTime(numLaps + 1, lapData[numLaps].getTime());
			if (numLaps > 0)
				DisplayTime(bestTime.first + 1, bestTime.second);

			// Is this lap a new best?
			if (numLaps == 0 || lapData[numLaps].getTime() < bestTime.second)
			{
				// Announce new fast lap.
				std::cout << " << Fast Lap";
				bestTime = std::make_pair(numLaps, lapData[numLaps].getTime());
			}
			std::cout << "\n";

			// Increment counters.
			numLaps++;
			hzCounter = 1;
		}

		// Prepare for next iteration.
		track.p0.x = track.p1.x;
		track.p0.y = track.p1.y;
	}
}

int main(void)
{
	// Pointers to gps rmc (string) fields.
	char* gpsTokens[RMC_CHECKSUM + 1];
	PORT port = NULL;

#ifdef FILE_INPUT
	
	// Attempt to open gps data file.
	if (fopen_s(&file, filePath, "r"))
	{
		printf("ERROR OPENING FILE.\n");
		exit(-1);
	}

#else

	uint8_t led = 0;

	// Open serial port.
	port = OpenPort(comPort);
	if (port == NULL)
	{
		std::cout << "FAILURE OPENING COM PORT " << comPort << std::endl;
		exit(-1);
	}

	//FlushInput(port);

	//SetPortBaudRate(port, CBR_19200);

	// Setup gps for proper sentence and refresh rate.
	gpsSetup(port);

	std::cout << "Waiting for GPS active status";

	// Wait for valid gps fix.
	do {
		if (led++ & 0x04)
			std::cout << ".";
	} while (!GetRMCSentence(port, gpsTokens));

#endif

	// Establish startline.
	std::cout << "\nGPS status active!\nAwaiting keypress to establish startline.";
	while (1)
	{
		if (GetRMCSentence(port, gpsTokens))
		{

#ifdef FILE_INPUT
			if (getchar())
#else
			if (_kbhit()) // Non-blocking.
#endif

			{
				std::cout << std::endl;

				float ts = EstablishStartLine(gpsTokens);

				if (ts != 0.0f)
				{
					Run(port, ts, gpsTokens);
					break;
				}
			}
		}
	}

#ifdef FILE_INPUT
	
	// Close file.
	if (file)
		fclose(file);

#else

	// Close serial port.
	ClosePort(port);

#endif

	return 0;
}

