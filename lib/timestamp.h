#ifndef _TIMESTAMP_H__
#define _TIMESTAMP_H__

#include <time.h>

namespace amb {

double currentTime()
{
	struct timespec tm;

	clock_gettime(CLOCK_REALTIME, &tm);

	double time = tm.tv_sec + (tm.tv_nsec / 1000000);
}

}

#endif
