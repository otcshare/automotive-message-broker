#ifndef _TIMESTAMP_H__
#define _TIMESTAMP_H__

#include <time.h>
#include <iostream>

namespace amb {

double currentTime()
{
	struct timespec tm;

	clock_gettime(CLOCK_REALTIME, &tm);

	double ns = double(tm.tv_nsec) / 1000000000;


	double time = double(tm.tv_sec) + ns;

	return time;
}

}

#endif
