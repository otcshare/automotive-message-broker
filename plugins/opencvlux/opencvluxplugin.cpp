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

#ifdef OPENCL
#include <opencv/ocl/ocl.hpp>
#endif

using namespace std;

#include "debugout.h"

OpenCvLuxPlugin::OpenCvLuxPlugin(AbstractRoutingEngine* re, map<string, string> config)
	:AbstractSource(re, config), lastLux(0)
{
	re->setSupported(supported(), this);

	shared = new Shared;
	shared->parent = this;

	shared->m_capture = NULL;

	shared->threaded = false;
	shared->kinect = false;
	shared->useOpenCl = false;

	shared->fps=30;
	device="0";
	shared->pixelLowerBound = 0;
	shared->pixelUpperBound = 255;


	if(config.find("threaded") != config.end())
	{
		shared->threaded = config["threaded"] == "true";
	}

	if(config.find("kinect") != config.end())
	{
		shared->kinect = config["kinect"] == "true";
	}

	if(config.find("fps") != config.end())
	{
		shared->fps = boost::lexical_cast<int>(config["fps"]);
	}

	if(config.find("device") != config.end())
	{
		device = config["device"].c_str();
	}

	if(config.find("pixelLowerBound") != config.end())
	{
		shared->pixelLowerBound = boost::lexical_cast<int>(config["pixelLowerBound"]);

		if(shared->pixelLowerBound < 0)
		{
			shared->pixelLowerBound = 0;
		}
	}


	if(config.find("pixelUpperBound") != config.end())
	{
		shared->pixelUpperBound = boost::lexical_cast<int>(config["pixelUpperBound"]);

		if(shared->pixelUpperBound > 255)
			shared->pixelUpperBound = 255;
	}

	if(config.find("opencl") != config.end())
	{
		shared->useOpenCl = config["opencl"] == "true";
	}


#ifdef OPENCL
	if(useOpenCl)
	{
		std::vector<cv::ocl::Info> info;
		cv::ocl::getDevice(info);
	}
#endif

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
	if(!shared->m_capture || !shared->m_capture->isOpened())
	{
		/// we want to turn on the camera for one shot to get an image and determine the intensity

		if(init())
			grabImage(shared);
	}

	if(reply->property == VehicleProperty::ExteriorBrightness)
	{
		replyQueue.push_back(reply);
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
	throw std::runtime_error("OpenCVLuxPlugin does not support this operation.  We should never hit this method.");
}

AsyncPropertyReply *OpenCvLuxPlugin::setProperty(AsyncSetPropertyRequest request )
{
	throw std::runtime_error("OpenCVLuxPlugin does not support this operation.  We should never hit this method.");
}

void OpenCvLuxPlugin::subscribeToPropertyChanges(VehicleProperty::Property property)
{
	if(!shared->mRequests.size())
	{
		if(init())
			g_timeout_add(1000 / shared->fps, grabImage, shared);
	}

	shared->mRequests.push_back(property);
}

void OpenCvLuxPlugin::unsubscribeToPropertyChanges(VehicleProperty::Property property)
{
	shared->mRequests.remove(property);
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

static int grabImage(void *data)
{
	OpenCvLuxPlugin::Shared* shared = static_cast<OpenCvLuxPlugin::Shared*>(data);

	cv::Mat m_image;

	if(shared->kinect)
	{
		shared->m_capture->grab();
		shared->m_capture->retrieve( m_image, CV_CAP_OPENNI_GRAY_IMAGE );
	}
	else
	{
		*(shared->m_capture) >> m_image;
	}

	uint lux = evalImage(m_image,shared);

	shared->parent->updateProperty(lux);

	if(shared->mRequests.size())
	{
		return true;
	}

	delete shared->m_capture;
	shared->m_capture = NULL;
	return false;

}

static uint evalImage(cv::Mat qImg, OpenCvLuxPlugin::Shared *shared)
{
	cv::Scalar avgPixelIntensity;


	if(shared->useOpenCl)
	{
#ifdef OPENCL
		cv::Scalar stdDev;
		cv::ocl::meanStdDev(qImg, avgPixelIntensity, stdDev);
#endif
	}
	else
	{
		avgPixelIntensity = cv::mean(qImg);
	}


	double val = avgPixelIntensity.val[0];

	double qualifiedPixel = (val - shared->pixelLowerBound);

	if(qualifiedPixel < 0) qualifiedPixel = 0;

	uint lux = qualifiedPixel * (130000.0 / shared->pixelUpperBound);

	DebugOut(7)<<"average pixel value: "<<qualifiedPixel<<" lux: "<<lux<<endl;

	return lux;
}


bool OpenCvLuxPlugin::init()
{
	if(shared->m_capture) delete shared->m_capture;

	if(shared->kinect)
	{
		shared->m_capture = new cv::VideoCapture(CV_CAP_OPENNI);
	}
	else if(device == "")
		shared->m_capture = new cv::VideoCapture(0);
	else shared->m_capture = new cv::VideoCapture(atoi(device.c_str()));

	if(!shared->m_capture->isOpened())
	{
		DebugOut()<<"we failed to open camera device ("<<device<<") or no camera found"<<endl;
		return false;
	}



	DebugOut()<<"camera frame width: "<<shared->m_capture->get(CV_CAP_PROP_FRAME_WIDTH)<<endl;
	DebugOut()<<"camera frame height: "<<shared->m_capture->get(CV_CAP_PROP_FRAME_HEIGHT)<<endl;
	DebugOut()<<"camera frame fps: "<<shared->m_capture->get(CV_CAP_PROP_FPS)<<endl;

	return true;
}

void OpenCvLuxPlugin::updateProperty(uint lux)
{
	VehicleProperty::ExteriorBrightnessType l(lux);

	for(auto itr = replyQueue.begin(); itr != replyQueue.end(); itr++)
	{
		AsyncPropertyReply* reply = *itr;
		reply->value = &l;
		reply->success = true;
		reply->completed(reply);
	}

	replyQueue.clear();

	if(lux != lastLux && shared->mRequests.size())
	{
		lastLux = lux;
		routingEngine->updateProperty(VehicleProperty::ExteriorBrightness,&l, uuid());
	}


}

