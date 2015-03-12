#ifndef _TIMESTAMP_H___
#define _TIMESTAMP_H___

#include <time.h>

namespace amb {

double currentTime();

class Timestamp {
protected:
	Timestamp();

public:

	double currentTime();
	double currentTime(double time);

	double epochTime(double time);

	double epochTime();

    static double fromTimeval(const struct ::timeval& tv);
    static struct ::timeval toTimeval(const double time);

public:
	static Timestamp *instance();

private:
	double startTimeEpoch;
	static Timestamp* mInstance;
};

}

#endif
