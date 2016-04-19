#ifndef _TIMESTAMP_H___
#define _TIMESTAMP_H___


namespace amb {

double currentTime();

class Timestamp {
protected:
	Timestamp();

public:

	double currentTime();

	double epochTime(double time);

	double epochTime();

public:
	static Timestamp *instance();

private:
	double startTimeEpoch;
	static Timestamp* mInstance;
};

}

#endif
