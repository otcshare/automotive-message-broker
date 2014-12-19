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
#include <ambpluginimpl.h>
#include <string>
#include <memory>

#include <opencv2/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <QObject>
#include <QMutex>

using namespace std;

class OpenCvLuxPlugin: public QObject, public AmbPluginImpl
{
Q_OBJECT
public:

	typedef BasicPropertyType<bool> DriverDrowsinessType;

	struct Shared
	{
		Shared(): frameCount(0) { }
		std::unique_ptr<cv::VideoCapture> m_capture;
		std::unique_ptr<cv::VideoWriter> mWriter;
		PropertyList mRequests;
		OpenCvLuxPlugin* parent;

		int fps;
		bool threaded;
		bool kinect;
		bool useCuda;
		int pixelLowerBound;
		int pixelUpperBound;
		bool loggingOn;
		bool ddd;
		int frameCount;
	};

	OpenCvLuxPlugin(AbstractRoutingEngine* re, map<string, string> config,  AbstractSource& parent);

	~OpenCvLuxPlugin();

	const string uuid() const;
	void getPropertyAsync(AsyncPropertyReply *reply);
	void getRangePropertyAsync(AsyncRangePropertyReply *reply);
	AsyncPropertyReply * setProperty(AsyncSetPropertyRequest request);
	void subscribeToPropertyChanges(VehicleProperty::Property property);
	void unsubscribeToPropertyChanges(VehicleProperty::Property property);

	void propertyChanged(AbstractPropertyType* value);

	void supportedChanged(const PropertyList &);

	void updateProperty(uint16_t lux);

	void writeVideoFrame(cv::UMat frame);

	void detectEyes(cv::UMat frame);

	void init();

	std::shared_ptr<AbstractPropertyType> videoLogging;

public Q_SLOTS:
	void imgProcResult();

private:
	uint speed;
	uint latitude;
	uint longitude;

	uint lastLux;
	std::string device;
	std::list<AsyncPropertyReply*> replyQueue;

	std::shared_ptr<AbstractPropertyType> extBrightness;

	std::unique_ptr<Shared> shared;
	QMutex mutex;

	std::unique_ptr<cv::CascadeClassifier> faceCascade;
	std::unique_ptr<cv::CascadeClassifier> eyeCascade;

	std::shared_ptr<AbstractPropertyType> driverDrowsiness;
	std::shared_ptr<AbstractPropertyType> openCl;
};

static int grabImage(void *data);
static uint evalImage(cv::UMat qImg, OpenCvLuxPlugin::Shared *shared);

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

TrafficLight::Color detectLight(cv::UMat img, OpenCvLuxPlugin::Shared* shared);


#endif // EXAMPLEPLUGIN_H
