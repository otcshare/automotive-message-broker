#include "timestamp.h"

#include <math.h>
#include <time.h>
#include <iostream>
#include <chrono>

double amb::currentTime()
{
	return Timestamp::instance()->currentTime();
}

amb::Timestamp* amb::Timestamp::mInstance = nullptr;

amb::Timestamp::Timestamp()
{
	auto tm = std::chrono::system_clock::now();
	auto tm2 = std::chrono::steady_clock::now();

	double eTime = (std::chrono::duration_cast<std::chrono::milliseconds>(tm.time_since_epoch()).count() / 1000.00);
	double sTime = (std::chrono::duration_cast<std::chrono::milliseconds>(tm2.time_since_epoch()).count() / 1000.00);

	startTimeEpoch =  eTime - sTime;
}

double amb::Timestamp::currentTime()
{
	auto tm = std::chrono::steady_clock::now();

	double time = std::chrono::duration_cast<std::chrono::milliseconds>(tm.time_since_epoch()).count() / 1000.00;

	return time;
}

double amb::Timestamp::epochTime(double time)
{
	return startTimeEpoch + time;
}

double amb::Timestamp::currentTime(double time)
{
    return time - startTimeEpoch;
}

double amb::Timestamp::epochTime()
{
	auto tm = std::chrono::system_clock::now();

	double time = std::chrono::duration_cast<std::chrono::milliseconds>(tm.time_since_epoch()).count() / 1000.00;

	return time;
}

amb::Timestamp* amb::Timestamp::instance()
{
	if(!mInstance)
		mInstance = new Timestamp();

	return mInstance;
}

double amb::Timestamp::fromTimeval(const struct ::timeval &tv)
{
    return tv.tv_sec*1.0 + tv.tv_usec*1e-6;
}

struct ::timeval amb::Timestamp::toTimeval(const double time)
{
    return { (__time_t) time, (__suseconds_t)fmod(time*1e6, 1e6) };
}

struct ::bcm_timeval amb::Timestamp::toBcmTimeval(const double time)
{
    return { (long) time, (long)fmod(time*1e6, 1e6) };
}
