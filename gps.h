#ifndef _GPS_H_
#define _GPS_H_

#include <cstdint>
#include <stdio.h>
#include "gps_timer.h"
#include "serial_port.h"

static bool Checksum(char* sentence)
{
	assert(sentence != nullptr);

	uint8_t crc = 0;
	uint8_t n = htoi<uint8_t>(&sentence[strlen(sentence) - 2]);

	// Skip initial '$' and last 2 bytes (crc) + '*'.
	for (size_t i = 1; i < strlen(sentence) - 3; i++)
		crc ^= sentence[i];

	return (crc == n);
}

// Parse gps string.
static size_t ParseRMC(char* sentence, char* tokens[])
{
	assert(sentence != nullptr);

	size_t n = 0;

	tokens[n] = strtok_(sentence, ",");
	while (tokens[n] && n < RMC_CHECKSUM)
		tokens[++n] = strtok_(NULL, ",*");

	return n;
}

static bool GetRMCSentence(PORT port, char* tokens[])
{
	error.Clear();

#ifdef FILE_INPUT

	if (fgets(buffer, GPS_STRING_LENGTH, file) == NULL)
	{
		error.SetError(err::ID::FILE_EOF);
		return false;
	}

#else
	// Read data from serial port.
	ReceiveData(port, buffer, GPS_STRING_LENGTH - 1);
	
#endif

	// RMC sentence?
	if (strncmp("$GPRMC", buffer, 6) == 0)
	{
		// Terminate sentence at eol.
		char* eol = strchr(buffer, 0x0a);
		if (eol != NULL)
#ifdef FILE_INPUT
			buffer[eol - buffer] = 0;
#else
			buffer[eol - buffer - 1] = 0;
#endif

		// Confirm crc.
		if (!Checksum(buffer))
		{
			error.SetError(err::ID::CHECKSUM);
			return false;
		}

		// Parse the gps rmc string and check for valid fix.
		if (ParseRMC(buffer, tokens) == RMC_CHECKSUM)
		{
			if (tokens[RMC_STATUS] == nullptr || *tokens[RMC_STATUS] != 'A')
			{
				error.SetError(err::ID::NO_FIX);
				return false;
			}
			else
				return true;
		}
	}

	error.SetError(err::ID::BAD_SENTENCE);
	return false;
}

// UBX Protocol.
//const unsigned char UBLOX_INIT[] = {
// Rate (pick one below)
//UBX   SYNC  CLSS  ID    LENGTH     MSG                            CHECKSUM
//0xB5, 0x62, 0x06, 0x08, 0x06,0x00, 0x64,0x00,0x01,0x00,0x01,0x00, 0x7A,0x12 // 10Hz
//0xB5, 0x62, 0x06, 0x08, 0x06,0x00, 0xC8,0x00,0x01,0x00,0x01,0x00, 0xDE,0x6A // 5Hz
//0xB5, 0x62, 0x06, 0x08, 0x06,0x00, 0xE8,0x03,0x01,0x00,0x01,0x00, 0x01,0x39 // 1Hz
// Disable NMEA
//0xB5,0x62,0x06,0x01,0x08,0x00, 0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x01, 0x00,0x24, // GxGGA off
//0xB5,0x62,0x06,0x01,0x08,0x00, 0xF0,0x01,0x00,0x00,0x00,0x00,0x00,0x01, 0x01,0x2B, // GxGLL off
//0xB5,0x62,0x06,0x01,0x08,0x00, 0xF0,0x02,0x00,0x00,0x00,0x00,0x00,0x01, 0x02,0x32, // GxGSA off
//0xB5,0x62,0x06,0x01,0x08,0x00, 0xF0,0x03,0x00,0x00,0x00,0x00,0x00,0x01, 0x03,0x39, // GxGSV off
//0xB5,0x62,0x06,0x01,0x08,0x00, 0xF0,0x04,0x00,0x00,0x00,0x00,0x00,0x01, 0x04,0x40, // GxRMC off
//0xB5,0x62,0x06,0x01,0x08,0x00, 0xF0,0x05,0x00,0x00,0x00,0x00,0x00,0x01, 0x05,0x47, // GxVTG off
//
// Can also disable/enable NMEA sentences with a text command: 
// $PUBX,40,GLL,1,0,0,0,0,0*5D enables the GLL sentence on the serial port.
// $PUBX,41,1,0007,0003,19200,0*25 sets the GPS modules baud rate to 19200.
//const char baud19200[] = "$PUBX,41,1,0007,0003,19200,0*25";
//const char baud9600[] = "PUBX,41,1,3,3,9600,0";
//const char baud38400[] = "PUBX,41,1,3,3,38400,0";

std::array<unsigned char, 14> gpsUpdateRate = { 0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xC8, 0x00, 0x01, 0x00, 0x01, 0x00, 0xDE, 0x6A }; // 5Hz update rate.
std::array<unsigned char, 16> gpsDisableGGA = { 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x24 }; // GxGGA off
std::array<unsigned char, 16> gpsDisableGGL = { 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x2B }; // GxGLL off
std::array<unsigned char, 16> gpsDisableGSA = { 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x32 }; // GxGSA off
std::array<unsigned char, 16> gpsDisableGSV = { 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x39 }; // GxGSV off
std::array<unsigned char, 16> gpsDisableVTG = { 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x05, 0x47 }; // GxVTG off
std::array<unsigned char, 32> gpsBaud19200 = { "$PUBX,41,1,0007,0003,19200,0*25" }; // 19200 baud rate

static void gpsSetup(const PORT p)
{
	assert(p != NULL);

	SendData(p, &gpsDisableGGA[0], gpsDisableGGA.size()); // Disable GGA.
	SendData(p, &gpsDisableGGL[0], gpsDisableGGL.size()); // Disable GGL.
	SendData(p, &gpsDisableGSA[0], gpsDisableGSA.size()); // DIsable GSA.
	SendData(p, &gpsDisableGSV[0], gpsDisableGSV.size()); // Disable GSV.
	SendData(p, &gpsDisableVTG[0], gpsDisableVTG.size()); // Disable VTG.
	SendData(p, &gpsUpdateRate[0], gpsUpdateRate.size()); // Set update rate at 5Hz.
	//SendData(p, &gpsBaud19200[0], gpsBaud19200.size()); // Set 19200 baud rate.
}

#endif
