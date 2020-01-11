// predictive lap timer header file.
#ifndef _PREDICTIVE_LAP_TIMER_H_
#define _PREDICTIVE_LAP_TIMER_H_

// msvc
#define float double

#define MIN(x, y)                     (((x) < (y)) ? (x) : (y))
#define MAX(x, y)                     (((x) > (y)) ? (x) : (y))
#define DEGTORAD(deg)                 (deg*57.29577995f)
#define D_TO_RADIANS                  (PI/180.0)

// gps rmc sentence fields.
#define RMC_PREFIX                    0
#define RMC_TIME                      1
#define RMC_STATUS                    2
#define RMC_LATITUDE                  3
#define RMC_LATITUDE_PREFIX           4
#define RMC_LONGITUDE                 5
#define RMC_LONGITUDE_PREFIX          6
#define RMC_SPEED                     7
#define RMC_TRACK                     8
#define RMC_DATE                      9
#define RMC_MAGNETIC_VARIATION        10
#define RMC_MAGNETIC_VARIATION_PREFIX 11
#define RMC_FAA_MODE_INDICATOR        12
#define RMC_CHECKSUM                  13

#define LATITUDE                      0x01
#define LONGITUDE                     0x02

// Maximum possible characters in a GPS string (+ fudge).
const std::size_t GPS_STRING_LENGTH = 96;

// Gps update frequency and period.
const std::size_t GPS_UPDATE_FREQUENCY = 5; // Hz.
static constexpr float GPS_UPDATE_PERIOD{ 1./GPS_UPDATE_FREQUENCY };

// Distance calculations.
static constexpr float PI{ 3.141592654 };
static constexpr float EARTH_RADIUS{ 3956.0 }; // In miles. 

// Definitions for the startline.
static constexpr float LINE_WIDTH{ 50.0f };
static constexpr float LINE_WIDTH_2{ 25.0f };
static constexpr float PROJECTION_DISTANCE{ 100.0f };

// Serial port with gps device connected (see device manager).
constexpr int COM_PORT{ 5 };
// Serial/file input buffer.
char buffer[GPS_STRING_LENGTH];

struct lap
{
	void setStart(float ts) 
	{
		assert(ts >= 0.);
		start = ts;
	}

	void setStop(float ts) 
	{ 
		assert(ts >= 0.);
		stop = ts; 
	}
	
	float getStart() { return start; }
	float getStop() { return stop; }
	
	float getTime()
	{
		assert(stop >= start);
		return (stop - start);
	}

private:
	float start;
	float stop;
};

struct point_t { float x, y; };
struct line_t { point_t p0, p1; };

// Coordinate of start/finish location.
point_t startPoint;
// Startline endpoints.
line_t startingLine;
// Heading crossing start/finish.
uint16_t startHeading;
// Coordinates of current & previous GPS location.
line_t track;

static const char* description[] = { "NO ERROR", "INVALID SENTENCE", "CHECKSUM FAILURE", "NO FIX" "NON-SEQUENTIAL SENTENCE", "END OF FILE" };

struct err
{
	enum ID : size_t { NONE = 0, CHECKSUM, BAD_SENTENCE, NO_FIX, TIME_STAMP, FILE_EOF };

	void Clear() { error_ = ID::NONE; }
	void SetError(ID id) { error_ = id; }
	ID GetError() { return error_; }
	const char* GetDescription() { return description[error_]; }
	//const char* GetDescription() { return static_cast<const char*>(description[error_]); }

private:
	ID error_;
	//static constexpr const char* description[] = { "NO ERROR", "INVALID SENTENCE", "CRC FAILURE", "NO FIX" "NON SEQUENTIAL TIME STAMP", "END OF FILE" };
};

// Gps errors.
err error;

#define FILE_INPUT

#ifdef FILE_INPUT
const char* filePath = "pir2.txt";
FILE* file = NULL;
bool FILE_INPUT_EOF = false;
PORT port = nullptr;
#endif

#endif
