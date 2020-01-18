#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <cstdio>
#include <cctype>
#include <cassert>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// A very basic atof function (no exponentials, no sign).
static float atof_(char s[])
{
	float val, power;
	int8_t i = 0;

	while (!isdigit(s[i]))
		i++;

	for (val = 0.0; isdigit(s[i]); i++)
		val = 10.0 * val + (s[i] - '0');

	if (s[i] == '.')
		i++;

	for (power = 1.0; isdigit(s[i]); i++)
	{
		val = 10.0 * val + (s[i] - '0');
		power *= 10.0;
	}

	return val / power;
}

// strtok implementation which recognizes consecutive delimiters.
static char* strtok_(char* str, const char* delim)
{
	static char* staticStr = 0;          // Stores last address.
	int i = 0, strLen = 0, delimLen = 0; // Indexes.

	// If delimiter is NULL or no more chars remaining.
	if (delim == 0 || (str == 0 && staticStr == 0))
		return 0;

	if (str == 0)
		str = staticStr;

	// Get length of string and delimiter.
	while (str[strLen])
		strLen++;
	while (delim[delimLen])
		delimLen++;

	// Find a delimiter.
	char* p = strpbrk(str, delim);
	if (p)
		i = p - str;
	else
	{
		// If no delimiters, return str.
		staticStr = 0;
		return str;
	}

	// Terminate the string.
	str[i] = '\0';

	// Save remaining string.
	if ((str + i + 1) != 0)
		staticStr = (str + i + 1);
	else
		staticStr = 0;

	return str;
}

#ifdef __cplusplus
}
#endif

// Convert hex string to decimal.
static char hex(const char ch)
{
	if (ch >= '0' && ch <= '9')
		return ch - '0';
	if (ch >= 'a' && ch <= 'f')
		return ch - 'a' + 10;

	return 0;
}

template<typename T>
static T htoi(const char* hexStr)
{
	T value = T{ 0 };

	if (hexStr != nullptr)
		for (size_t i = 0; i < sizeof(T) * 2; ++i)
			value |= hex(tolower(hexStr[i])) << (8 * sizeof(T) - 4 * (i + 1));

	return value;
};

static float ConvertToSeconds(char* time)
{
	if (time == nullptr)
		return 0.0f;

	float ft = atof_(time);
	
	uint16_t hm = (uint16_t)(ft / 100);
	uint16_t hours = (uint16_t)(ft / 10000);
	uint16_t minutes = (hm - (hours * 100) + (hours * 60));
	float seconds = ft - (hm * 100) + (minutes * 60);

	return seconds;
}

// Determine if floats are relatively equal.
static bool Equal(float a, float b) { return fabs(a - b) <= FLT_EPSILON; }

// Check heading and angle within 30 degrees.
static bool Within30(const uint16_t a, const uint16_t h) { return ((360 - abs(a - h) % 360 < 30) || (abs(a - h) % 360 < 30)); }

static float Distance(const point_t t1, const point_t t2)
{
	float Lat1, Long1, Lat2, Long2;		// Coordinates in degrees.
	float dlat, dlon;			// Change in location.
	float a, d;
	float c;				// Great Circle distance (radians).

	Lat1 = (float)((uint32_t)(t1.y / 100.0f));
	Lat1 = (float)(Lat1 + (t1.y - Lat1 * 100.0f) / 60.0f);
	Lat1 = (float)(Lat1 * D_TO_RADIANS);

	Long1 = (float)((uint32_t)(t1.x / 100.0f));
	Long1 = (float)(Long1 + (t1.x - Long1 * 100.0f) / 60.0f);
	Long1 = (float)(Long1 * D_TO_RADIANS);

	Lat2 = (float)((uint32_t)(t2.y / 100.0f));
	Lat2 = (float)(Lat2 + (t2.y - Lat2 * 100.0f) / 60.0f);
	Lat2 = (float)(Lat2 * D_TO_RADIANS);

Long2 = (float)((uint32_t)(t2.x / 100.0f));
	Long2 = (float)(Long2 + (t2.x - Long2 * 100.0f) / 60.0f);
	Long2 = (float)(Long2 * D_TO_RADIANS);

	dlat = Lat2 - Lat1;
	dlon = Long2 - Long1;

	a = (float)((sin(dlat / 2.0f) * sin(dlat / 2.0f)) + cos(Lat1) * cos(Lat2) * (sin(dlon / 2.0f) * sin(dlon / 2.0f)));
	c = (float)(2.0f * atan2(sqrt(a), sqrt(1.0f - a)));

	d = (float)(EARTH_RADIUS * c);

	return d;
}

// Construct a startline.
static void StartLine(const float sx, const float sy, const float shdg)
{
	float tx, ty; // Projected track coordinates.
	float m, b;   // Slope & y intercept.

	// Project racetrack along current heading.
	tx = sx + PROJECTION_DISTANCE * cos(DEGTORAD(shdg));
	ty = sy + PROJECTION_DISTANCE * sin(DEGTORAD(shdg));
	// Projected racetrack slope & y-intercept. 
	m = (sy - ty) / (sx - tx);
	b = sy - (m * sx);

	// Construct perpendicular (startline) slope & y-intercept.
	m = -1.0f / m;
	b = sy - (m * sx);

	// Define endpoints of the perpendicular.
	tx = sx + LINE_WIDTH_2; // Note: tx re-used as a temporary value here.
	startingLine.p0.y = (m * tx + b);
	startingLine.p0.x = tx;
	tx -= LINE_WIDTH;
	startingLine.p1.y = (m * tx + b);
	startingLine.p1.x = tx;
}

// 2d line intersection.
static bool LineIntersection(const line_t track)
{
	float z;
	int16_t s1, s2, s3, s4;

	// Quick rejection test.
	if (!(MAX(startingLine.p0.x, startingLine.p1.x) >= MIN(track.p0.x, track.p1.x) &&
		MAX(track.p0.x, track.p1.x) >= MIN(startingLine.p0.x, startingLine.p1.x) &&
		MAX(startingLine.p0.y, startingLine.p1.y) >= MIN(track.p0.y, track.p1.y) &&
		MAX(track.p0.y, track.p1.y) >= MIN(startingLine.p0.y, startingLine.p1.y)))
		return false;

	// Straddle tests.
	if ((z = ((track.p0.x - startingLine.p0.x) * (startingLine.p1.y - startingLine.p0.y)) - ((track.p0.y - startingLine.p0.y) * (startingLine.p1.x - startingLine.p0.x))) < 0.0f)
		s1 = -1; // Counterclockwise. 
	else if (z > 0.0f)
		s1 = 1;  // Clockwise.
	else
		s1 = 0;  // Collinear.

	if ((z = ((track.p1.x - startingLine.p0.x) * (startingLine.p1.y - startingLine.p0.y)) - ((track.p1.y - startingLine.p0.y) * (startingLine.p1.x - startingLine.p0.x))) < 0.0f)
		s2 = -1;
	else if (z > 0.0f)
		s2 = 1;
	else
		s2 = 0;

	if ((z = ((startingLine.p0.x - track.p0.x) * (track.p1.y - track.p0.y)) - ((startingLine.p0.y - track.p0.y) * (track.p1.x - track.p0.x))) < 0.0f)
		s3 = -1;
	else if (z > 0.0f)
		s3 = 1;
	else
		s3 = 0;

	if ((z = ((startingLine.p1.x - track.p0.x) * (track.p1.y - track.p0.y)) - ((startingLine.p1.y - track.p0.y) * (track.p1.x - track.p0.x))) < 0.0f)
		s4 = -1;
	else if (z > 0.0f)
		s4 = 1;
	else
		s4 = 0;

	if ((s1 * s2 <= 0) && (s3 * s4 <= 0))
		return true;

	// Line segments do not intersect.
	return false;
}

// 2d lines point of intersection.
static void IntersectPoint(const point_t p1, const point_t p2, point_t* i)
{
	float denom, numera, mua; //numerb, mub;

	// No checks because it is assumed the lines intersect.
	denom = (startingLine.p1.y - startingLine.p0.y) * (p2.x - p1.x) - (startingLine.p1.x - startingLine.p0.x) * (p2.y - p1.y);
	numera = (startingLine.p1.x - startingLine.p0.x) * (p1.y - startingLine.p0.y) - (startingLine.p1.y - startingLine.p0.y) * (p1.x - startingLine.p0.x);
	//numerb = (p2.x - p1.x)*(p1.y - startingLine1.y) - (p2.y - p1.y)*(p1.x - startingLine1.x);

	mua = numera / denom;
	//mub = numerb/denom;
	i->x = p1.x + mua * (p2.x - p1.x);
	i->y = p1.y + mua * (p2.y - p1.y);
}

// Copy lat/long strings and format as ddd.dddd
static void GeoCopy(const char* s, char* d, const unsigned char value)
{
	assert(s != nullptr);
	assert(d != nullptr);

	int i = 0;

	// Copy all numerals, insert/skip decimal point.
	do {
		if (value == LONGITUDE && i == 3)
		{
			*d++ = '.';
			i++;
		}

		if (value == LATITUDE && i == 2)
		{
			*d++ = '.';
			i++;
		}

		if ((*s >= '0') && (*s <= '9'))
		{
			*d++ = *s;
			i++;
		}
	} while (*s++ != '\0');

	// Null terminate.
	*d = '\0';
}

#endif
