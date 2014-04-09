#include "timestamp.h"

#include <time.h>
#include <iostream>
#include <chrono>

double amb::currentTime()
{
	auto tm = std::chrono::high_resolution_clock::now();

	double time = std::chrono::duration_cast<std::chrono::milliseconds>(tm.time_since_epoch()).count() / 1000.00;

	return time;
}
