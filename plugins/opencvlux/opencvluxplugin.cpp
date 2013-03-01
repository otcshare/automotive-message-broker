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

#include "opencvluxplugin.h"
#include "timestamp.h"

#include <iostream>
#include <boost/assert.hpp>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentRun>

using namespace std;

#include "debugout.h"

OpenCvLuxPlugin::OpenCvLuxPlugin(AbstractRoutingEngine* re, map<string, string> config)
	:QObject(),AbstractSource(re, config), m_capture(0),lastLux(0)
{
	re->setSupported(supported(), this);

	threaded = false;
	kinect = false;
	timer = new QTimer(this);
	fps=30;
	device="0";
	pixelLowerBound = 0;
	pixelUpperBound = 255;

	connect(timer,SIGNAL(timeout()),this,SLOT(grabImage()));

	if(config.find("threaded") != config.end())
	{
		threaded = config["threaded"] == "true";
	}

	if(config.find("kinect") != config.end())
	{
		kinect = config["kinect"] == "true";
	}

	if(config.find("fps") != config.end())
	{
		fps = boost::lexical_cast<int>(config["fps"]);
	}

	if(config.find("device") != config.end())
	{
		device = config["device"].c_str();
	}

	if(config.find("pixelLowerBound") != config.end())
	{
		pixelLowerBound = boost::lexical_cast<int>(config["pixelLowerBound"]);

		if(pixelLowerBound < 0)
			pixelLowerBound = 0;
	}


	if(config.find("pixelUpperBound") != config.end())
	{
		pixelUpperBound = boost::lexical_cast<int>(config["pixelUpperBound"]);

		if(pixelUpperBound > 255)
			pixelUpperBound = 255;
	}
}



extern "C" AbstractSource * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new OpenCvLuxPlugin(routingengine, config);
	
}

string OpenCvLuxPlugin::uuid()
{
	return "3c7a1ea0-7d2e-11e2-9e96-0800200c9a66";
}


void OpenCvLuxPlugin::getPropertyAsync(AsyncPropertyReply *reply)
{
	if(!timer->isActive())
	{
		/// we want to turn on the camera for one shot to get an image and determine the intensity

		init();
		timer->singleShot(1,this,SLOT(grabImage()));
	}

	if(reply->property == VehicleProperty::ExteriorBrightness)
	{
		replyQueue.append(reply);
	}

	else  ///We don't support what you are asking for.  Reply false
	{
		reply->value = NULL;
		reply->success = false;
		reply->completed(reply);
	}
}

void OpenCvLuxPlugin::getRangePropertyAsync(AsyncRangePropertyReply *reply)
{

}

AsyncPropertyReply *OpenCvLuxPlugin::setProperty(AsyncSetPropertyRequest request )
{

}

void OpenCvLuxPlugin::subscribeToPropertyChanges(VehicleProperty::Property property)
{
	if(!timer->isActive())
	{
		init();
		timer->start(1000 / fps);
	}

	mRequests.push_back(property);
}

void OpenCvLuxPlugin::unsubscribeToPropertyChanges(VehicleProperty::Property property)
{
	mRequests.remove(property);

	if(!mRequests.size())
	{
		timer->stop();
	}
}

PropertyList OpenCvLuxPlugin::supported()
{
	PropertyList props;
	props.push_back(VehicleProperty::ExteriorBrightness);

	
	return props;
}

int OpenCvLuxPlugin::supportedOperations()
{
	return Get;
}

void OpenCvLuxPlugin::grabImage()
{

	if(kinect)
	{
		m_capture->grab();
		m_capture->retrieve( m_image, CV_CAP_OPENNI_GRAY_IMAGE );
	}
	else
	{
		(*m_capture) >> m_image;
	}

	if(threaded)
	{
		QFutureWatcher<uint> *watcher = new QFutureWatcher<uint>();
		connect(watcher,SIGNAL(finished()),this,SLOT(imageGrabCompleted()));

		QFuture<uint> future = QtConcurrent::run(this, &OpenCvLuxPlugin::evalImage, m_image);
		watcher->setFuture(future);
	}
	else
	{
		uint lux = evalImage(m_image);

		updateProperty(lux);
	}

}

uint OpenCvLuxPlugin::evalImage(cv::Mat qImg)
{
	cv::Scalar avgPixelIntensity = cv::mean(qImg);

	double val = avgPixelIntensity.val[0];

	double qualifiedPixel = (val - pixelLowerBound);

	if(qualifiedPixel < 0) qualifiedPixel = 0;

	uint lux = qualifiedPixel * (130000.0 / pixelUpperBound);

	DebugOut(7)<<"average pixel value: "<<qualifiedPixel<<" lux: "<<lux<<endl;

	return lux;
}

void OpenCvLuxPlugin::imageGrabCompleted()
{
	QFutureWatcher<uint> * watcher = dynamic_cast<QFutureWatcher<uint>* >(sender());

	uint lux = watcher->result();

	updateProperty(lux);
}

void OpenCvLuxPlugin::init()
{
	if(m_capture) delete m_capture;

	if(kinect)
	{
		m_capture = new cv::VideoCapture(CV_CAP_OPENNI);
	}
	else if(device == "")
		m_capture = new cv::VideoCapture(0);
	else m_capture = new cv::VideoCapture(device.toInt());

	if(!m_capture->isOpened())
	{
		qDebug()<<"we failed to open camera device ("<<device<<") or no camera found";
		return;
	}

	DebugOut()<<"camera frame width: "<<m_capture->get(CV_CAP_PROP_FRAME_WIDTH)<<endl;
	DebugOut()<<"camera frame height: "<<m_capture->get(CV_CAP_PROP_FRAME_HEIGHT)<<endl;
	DebugOut()<<"camera frame fps: "<<m_capture->get(CV_CAP_PROP_FPS)<<endl;
}

void OpenCvLuxPlugin::updateProperty(uint lux)
{
	VehicleProperty::ExteriorBrightnessType l(lux);

	Q_FOREACH(AsyncPropertyReply* reply, replyQueue)
	{
		reply->value = &l;
		reply->success = true;
		reply->completed(reply);
	}

	replyQueue.clear();

	if(!timer->isActive())
	{
		delete m_capture;
		m_capture = NULL;
	}

	if(lux != lastLux && mRequests.size())
	{
		lastLux = lux;
		routingEngine->updateProperty(VehicleProperty::ExteriorBrightness,&l, uuid());
	}


}

