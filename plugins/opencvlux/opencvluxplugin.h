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

#ifndef EXAMPLEPLUGIN_H
#define EXAMPLEPLUGIN_H

#include <abstractsource.h>
#include <string>

#include <QTimer>
#include <QList>
#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace std;

class OpenCvLuxPlugin: public QObject, public AbstractSource
{
	Q_OBJECT

public:

	OpenCvLuxPlugin(AbstractRoutingEngine* re, map<string, string> config);
	
	string uuid();
	void getPropertyAsync(AsyncPropertyReply *reply);
	void getRangePropertyAsync(AsyncRangePropertyReply *reply);
	AsyncPropertyReply * setProperty(AsyncSetPropertyRequest request);
	void subscribeToPropertyChanges(VehicleProperty::Property property);
	void unsubscribeToPropertyChanges(VehicleProperty::Property property);
	PropertyList supported();

	int supportedOperations();
	
	void propertyChanged(VehicleProperty::Property property, AbstractPropertyType* value, string uuid) {}
	void supportedChanged(PropertyList) {}
	
private Q_SLOTS:
	void grabImage();
	uint evalImage(cv::Mat qImg);
	void imageGrabCompleted();
	
private: /// method s:
	void init();
	void updateProperty(uint lux);

private:
	bool started;
	double fps;
	bool threaded;
	bool kinect;
	bool useOpenCl;
	int pixelLowerBound;
	int pixelUpperBound;
	uint lastLux;
	QString device;
	PropertyList mRequests;
	QTimer *timer;
	QList<AsyncPropertyReply*> replyQueue;

	cv::VideoCapture *m_capture;
	cv::Mat m_image;
};

#endif // EXAMPLEPLUGIN_H
