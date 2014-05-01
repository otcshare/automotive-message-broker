/*
Copyright (C) 2012 Intel Corporation

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef OPENCVLUXPLUGIN_H
#define OPENCVLUXPLUGIN_H

#include <abstractsource.h>
#include <string>

#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>

#include <QObject>
#include <QMutex>

using namespace std;

class OpenCvLuxPlugin: public QObject, public AbstractSource
{
Q_OBJECT
public:

	struct Shared
	{
		cv::VideoCapture *m_capture;
		cv::VideoWriter *mWriter;
		PropertyList mRequests;
		OpenCvLuxPlugin* parent;

		double fps;
		bool threaded;
		bool kinect;
		bool useOpenCl;
        bool useCuda;
		int pixelLowerBound;
		int pixelUpperBound;
		bool loggingOn;
	};

	OpenCvLuxPlugin(AbstractRoutingEngine* re, map<string, string> config);

	~OpenCvLuxPlugin();
	
	const string uuid();
	void getPropertyAsync(AsyncPropertyReply *reply);
	void getRangePropertyAsync(AsyncRangePropertyReply *reply);
	AsyncPropertyReply * setProperty(AsyncSetPropertyRequest request);
	void subscribeToPropertyChanges(VehicleProperty::Property property);
	void unsubscribeToPropertyChanges(VehicleProperty::Property property);
	PropertyList supported();

	int supportedOperations();

	void propertyChanged(AbstractPropertyType* value);
	
	void supportedChanged(const PropertyList &);
	
	void updateProperty(uint lux);

	void writeVideoFrame(cv::Mat frame);

public Q_SLOTS:
	void imgProcResult();

	
private: /// methods:
	bool init();

private:	
	uint speed;
	uint latitude;
	uint longitude;

	uint lastLux;
	std::string device;
	std::list<AsyncPropertyReply*> replyQueue;

	Shared* shared;
	QMutex mutex;
};

static int grabImage(void *data);
static uint evalImage(cv::Mat qImg, OpenCvLuxPlugin::Shared *shared);

namespace TrafficLight
{
enum Color
{
	Unknown = -1,
	Red = 0,
	Yellow,
	Green
};
}

TrafficLight::Color detectLight(cv::Mat img, OpenCvLuxPlugin::Shared* shared);

#endif // EXAMPLEPLUGIN_H
