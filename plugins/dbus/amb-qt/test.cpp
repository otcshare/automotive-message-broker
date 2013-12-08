#include "ambqt.h"
#include "timestamp.h"

#include <QCoreApplication>

#include <debugout.h>
#include <iostream>

int main(int argc, char** argv)
{
	QCoreApplication app(argc,argv);

	AmbProperty speed;

	double totalLatency=0;
	int numSamples=0;

	QObject::connect(&speed, &AmbProperty::signalChanged,[&](QVariant val)
	{
		double t1 = speed.time();
		double t2 = amb::currentTime();

		double latency = (t2-t1)*1000.0;

		DebugOut(0)<<"latency: "<<latency<<std::endl;
		totalLatency+=latency;
		numSamples++;
		DebugOut(0)<<"Average: "<<totalLatency / numSamples<<std::endl;
	});

	speed.setPropertyName("VehicleSpeed");
	speed.connect();

	return app.exec();
}
