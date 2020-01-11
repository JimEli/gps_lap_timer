#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <cassert>

// Determine if floats are relatively equal.
bool Equal(float a, float b) { return fabs(a - b) <= FLT_EPSILON; }

// Check heading and angle within 30 degrees.
bool Within30(uint16_t a, uint16_t h) { return ((360 - abs(a - h) % 360 < 30) || (abs(a - h) % 360 < 30)); }

float Distance(point_t t1, point_t t2)
{
	float Lat1, Long1, Lat2, Long2;		// Coordinates in degrees.
	float dlat, dlon;					// Change in location.
	float a, d;
	float c;							// Great Circle distance (radians).

	Lat1 = (float)((uint32_t)(t1.y / 100.0));
	Lat1 = (float)(Lat1 + (t1.y - Lat1 * 100) / 60.0);
	Lat1 = (float)(Lat1 * D_TO_RADIANS);

	Long1 = (float)((uint32_t)(t1.x / 100.0));
	Long1 = (float)(Long1 + (t1.x - Long1 * 100) / 60.0);
	Long1 = (float)(Long1 * D_TO_RADIANS);

	Lat2 = (float)((uint32_t)(t2.y / 100.0));
	Lat2 = (float)(Lat2 + (t2.y - Lat2 * 100) / 60.0);
	Lat2 = (float)(Lat2 * D_TO_RADIANS);

	Long2 = (float)((uint32_t)(t2.x / 100.0));
	Long2 = (float)(Long2 + (t2.x - Long2 * 100) / 60.0);
	Long2 = (float)(Long2 * D_TO_RADIANS);

	dlat = Lat2 - Lat1;
	dlon = Long2 - Long1;

	a = (float)((sin(dlat / 2) * sin(dlat / 2)) + cos(Lat1) * cos(Lat2) * (sin(dlon / 2) * sin(dlon / 2)));
	c = (float)(2 * atan2(sqrt(a), sqrt(1 - a)));

	d = (float)(EARTH_RADIUS * c);

	return d;
}

// Construct a startline.
void StartLine(float sx, float sy, float shdg)
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
	m = -1.0 / m;
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
bool LineIntersection(line_t track)
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
	if ((z = ((track.p0.x - startingLine.p0.x) * (startingLine.p1.y - startingLine.p0.y)) - ((track.p0.y - startingLine.p0.y) * (startingLine.p1.x - startingLine.p0.x))) < 0)
		s1 = -1; // Counterclockwise. 
	else if (z > 0)
		s1 = 1;  // Clockwise.
	else
		s1 = 0;  // Collinear.

	if ((z = ((track.p1.x - startingLine.p0.x) * (startingLine.p1.y - startingLine.p0.y)) - ((track.p1.y - startingLine.p0.y) * (startingLine.p1.x - startingLine.p0.x))) < 0)
		s2 = -1;
	else if (z > 0)
		s2 = 1;
	else
		s2 = 0;

	if ((z = ((startingLine.p0.x - track.p0.x) * (track.p1.y - track.p0.y)) - ((startingLine.p0.y - track.p0.y) * (track.p1.x - track.p0.x))) < 0)
		s3 = -1;
	else if (z > 0)
		s3 = 1;
	else
		s3 = 0;

	if ((z = ((startingLine.p1.x - track.p0.x) * (track.p1.y - track.p0.y)) - ((startingLine.p1.y - track.p0.y) * (track.p1.x - track.p0.x))) < 0)
		s4 = -1;
	else if (z > 0)
		s4 = 1;
	else
		s4 = 0;

	if ((s1 * s2 <= 0) && (s3 * s4 <= 0))
		return true;

	// Line segments do not intersect.
	return false;
}

// 2d lines point of intersection.
void IntersectPoint(point_t p1, point_t p2, point_t* i)
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
void GeoCopy(char* s, char* d, unsigned char value)
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
