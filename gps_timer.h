// Lap timer header file.
#ifndef _LAP_TIMER_H_
#define _LAP_TIMER_H_

// msvc
#define float double

#define MIN(x, y)                     (((x) < (y)) ? (x) : (y))
#define MAX(x, y)                     (((x) > (y)) ? (x) : (y))
#define DEGTORAD(deg)                 (deg*57.29577995f)
#define D_TO_RADIANS                  (PI/180.0f)

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
static constexpr float GPS_UPDATE_PERIOD{ 1.0f/GPS_UPDATE_FREQUENCY };

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

// Lap time class.
struct lap
{
	void setStart(const float ts) 
	{
		assert(ts >= 0.0f);
		start = toSeconds(ts);
	}

	void setStop(const float ts) 
	{ 
		assert(ts >= 0.0f);
		stop = toSeconds(ts);
	}
	
	float getStart() const { return toTimeStamp(start); }
	float getStop() const { return toTimeStamp(stop); }
	
	float getTime() const 
	{
		if (stop < start)
			return 0.0f;
		
		return (stop - start);
	}

private:
	static float toSeconds(float t)
	{
		int hm = t / 100;
		int h = t / 10000;
		int m = hm - (h * 100) + (h * 60);
		float s = t - (hm * 100) + (m * 60);

		return s;
	}

	static float toTimeStamp(float s)
	{
		int h = s / 3600;
		int m = s / 60 - (h * 60);
		float fs = s - (int)(s / 100) * 100;
		float ts = (((h * 100) + m) * 100) + fs;
		
		return ts;
	}

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

static const char* description[] = { "NO ERROR", "CHECKSUM FAILURE", "INVALID SENTENCE", "NO FIX", "NON-SEQUENTIAL SENTENCE", "END OF FILE" };

struct err
{
	enum ID : size_t { NONE = 0, CHECKSUM, BAD_SENTENCE, NO_FIX, TIME_STAMP, FILE_EOF };

	void Clear() { error_ = ID::NONE; }
	void SetError(const ID id) { error_ = id; }
	ID GetError() const { return error_; }
	const char* GetDescription() const { return description[error_]; }
	//const char* GetDescription() { return static_cast<const char*>(description[error_]); }

private:
	ID error_;
	//static constexpr const char* description[] = { "NO ERROR", "INVALID SENTENCE", "CHECKSUM FAILURE", "NO FIX" "NON-SEQUENTIAL SENTENCE, "END OF FILE" };
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
