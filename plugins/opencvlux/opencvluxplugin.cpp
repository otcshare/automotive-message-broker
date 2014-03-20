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
#include <opencv2/imgproc/imgproc.hpp>

#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>

#ifdef OPENCL
#include <opencv2/ocl/ocl.hpp>
#endif

#ifdef CUDA
#include <opencv2/gpu/gpu.hpp>
#endif

using namespace std;

#include "debugout.h"

OpenCvLuxPlugin::OpenCvLuxPlugin(AbstractRoutingEngine* re, map<string, string> config)
	:AbstractSource(re, config), lastLux(0), speed(0), latitude(0), longitude(0)
{
	re->setSupported(supported(), this);
	re->subscribeToProperty(VehicleProperty::VehicleSpeed, this);
	re->subscribeToProperty(VehicleProperty::Latitude,this);
	re->subscribeToProperty(VehicleProperty::Longitude, this);

	shared = new Shared;
	shared->parent = this;

	shared->m_capture = NULL;
	shared->mWriter = nullptr;

	shared->threaded = false;
	shared->kinect = false;
	shared->useOpenCl = false;
	shared->useCuda = false;

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

	if(config.find("cuda") != config.end())
	{
		shared->useCuda = config["cuda"] == "true";
	}


#ifdef OPENCL
	if(shared->useOpenCl)
	{
		cv::ocl::PlatformsInfo platforms;
		cv::ocl::getOpenCLPlatforms(platforms);

		for(auto p : platforms)
		{
			DebugOut(1)<<"platform: "<<p->platformName<<" vendor: "<<p->platformVendor<<endl;
		}

		cv::ocl::DevicesInfo info;
		cv::ocl::getOpenCLDevices(info, cv::ocl::CVCL_DEVICE_TYPE_ALL);

		DebugOut(1)<<"There are "<<info.size()<<" OpenCL devices on this system"<<endl;

		if(!info.size())
		{
			DebugOut(1)<<"No CL devices.  Disabling OpenCL"<<endl;
			shared->useOpenCl = false;
		}
		else
		{
			cv::ocl::setDevice(info[0]);
		}

		for(auto i : info)
		{
			for(auto name : i->deviceName)
			{
				DebugOut(1)<<"OpenCLDeviceName: "<<name<<endl;
			}
		}
	}
#endif

#ifdef CUDA
	if(shared->useCuda)
	{
		int devices = cv::gpu::getCudaEnabledDeviceCount();
		DebugOut()<<"There are "<<devices<<" CUDA devices on this system"<<endl;
		if(devices)
		{
			DebugOut()<<"We will use 0 as the default device"<<endl;
			cv::gpu::DeviceInfo info(0);
			DebugOut()<<"Cuda Device Name: "<<info.name()<<endl;
			DebugOut()<<"Version: "<<info.majorVersion()<<"major"<<info.minorVersion()<<"minor"<<endl;
			DebugOut()<<"Streaming processor count: "<<info.multiProcessorCount()<<endl;
			cv::gpu::setDevice(0);
		}
		else
		{
			DebugOut(DebugOut::Warning)<<"No CUDA device found.  Disabling CUDA."<<endl;
			shared->useCuda = false;
		}
	}
#endif

}



extern "C" AbstractSource * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new OpenCvLuxPlugin(routingengine, config);
	
}

const string OpenCvLuxPlugin::uuid()
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

void OpenCvLuxPlugin::propertyChanged(AbstractPropertyType *value)
{
	if(value->name == VehicleProperty::VehicleSpeed)
	{
		speed = value->value<uint16_t>();
	}
	else if(value->name == VehicleProperty::Latitude)
	{
		latitude = value->value<double>();
	}
	else if(value->name == VehicleProperty::Longitude)
	{
		longitude = value->value<double>();
	}
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

	shared->parent->writeVideoFrame( m_image );

	if(shared->threaded)
	{
		QFutureWatcher<uint> *watcher = new QFutureWatcher<uint>();
		QObject::connect(watcher, &QFutureWatcher<uint>::finished, shared->parent, &OpenCvLuxPlugin::imgProcResult);

		QFuture<uint> future = QtConcurrent::run( evalImage, m_image, shared);
		watcher->setFuture(future);
	}
	else
	{
		int lux = evalImage(m_image, shared);
		//detectLight(m_image,shared);
		shared->parent->updateProperty(lux);
	}

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
		cv::ocl::oclMat src(qImg), gray;
		cv::ocl::cvtColor(src, gray, CV_BGR2GRAY);
		cv::ocl::meanStdDev(src, avgPixelIntensity, stdDev);
#endif
	}
	else if(shared->useCuda)
	{
#ifdef CUDA
		cv::gpu::GpuMat src(qImg), dest;
		cv::gpu::cvtColor(src, dest, CV_BGR2GRAY);
		cv::Scalar stdDev;
		try
		{

			cv::gpu::meanStdDev(dest, avgPixelIntensity, stdDev);
		}
		catch(...)
		{
			DebugOut(DebugOut::Error)<<"CUDA pixel intensity calculation failed."<<endl;
		}
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
	cv::Size s = cv::Size((int) shared->m_capture->get(CV_CAP_PROP_FRAME_WIDTH),
					  (int) shared->m_capture->get(CV_CAP_PROP_FRAME_HEIGHT));

	shared->mWriter = new cv::VideoWriter("/tmp/video.avi",CV_FOURCC('H','2','6','4'),30,s);

	DebugOut()<<"camera frame width: "<<shared->m_capture->get(CV_CAP_PROP_FRAME_WIDTH)<<endl;
	DebugOut()<<"camera frame height: "<<shared->m_capture->get(CV_CAP_PROP_FRAME_HEIGHT)<<endl;
	DebugOut()<<"camera frame fps: "<<shared->m_capture->get(CV_CAP_PROP_FPS)<<endl;

	return true;
}

void OpenCvLuxPlugin::writeVideoFrame(cv::Mat frame)
{
	//if(speed > 0)
	{
		(*shared->mWriter)<<frame;
	}
}

void OpenCvLuxPlugin::updateProperty(uint lux)
{
	VehicleProperty::ExteriorBrightnessType l(lux);

	for(auto reply : replyQueue)
	{
		reply->value = &l;
		reply->success = true;
		try{
			reply->completed(reply);
		}
		catch(...)
		{
			DebugOut(DebugOut::Warning)<<"reply failed"<<endl;
		}
	}

	replyQueue.clear();

	if(lux != lastLux && shared->mRequests.size())
	{
		lastLux = lux;
		routingEngine->updateProperty(&l, uuid());
	}


}

void OpenCvLuxPlugin::imgProcResult()
{
	QFutureWatcher<uint> *watcher = dynamic_cast<QFutureWatcher<uint>* >(sender());

	uint lux = watcher->result();
	shared->parent->updateProperty(lux);
}

TrafficLight::Color detectLight(cv::Mat img, OpenCvLuxPlugin::Shared *shared)
{

	cv::Mat gray;

	if(shared->useOpenCl)
	{
#ifdef OPENCL
		cv::ocl::oclMat src(img), gray2;
		cv::ocl::cvtColor(src, gray2, CV_BGR2GRAY);
		cv::ocl::GaussianBlur(gray2, gray2, cv::Size(9,9), 2, 2);

		gray = gray2;
#endif
	}
	else
	{
		cv::cvtColor(img, gray, CV_BGR2GRAY);
		cv::GaussianBlur(gray, gray, cv::Size(9,9), 2, 2);
	}

	std::vector<cv::Vec3f> circles;

	//cv::HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, gray.rows/8, 200, 100, 0, 0);

	cv::HoughCircles( gray, circles, CV_HOUGH_GRADIENT, 2, 30, 231, 50, 0, 0 );

	for(auto i = 0; i < circles.size(); i++)
	{
		cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);

		cv::Rect rect(center.x - radius / 2, center.y - radius / 2, radius, radius);

		try {

			cv::Mat light(img, rect);

			cv::circle( img, center, radius, cv::Scalar(0,0,255), 3, 8, 0 );

			cv::rectangle(img, rect,cv::Scalar(255,0,0));

			cv::Scalar avgPixel = cv::mean(light);

			if(avgPixel[2] > 128 && avgPixel[0] < 128 && avgPixel[1] < 128)
			{
				DebugOut(1)<<"Red Light!!!"<<endl;
				return TrafficLight::Red;
			}
			else if(avgPixel[1] > 128 && avgPixel[0] < 128 && avgPixel[2] < 128)
			{
				DebugOut(1)<<"Green Light!!!"<<endl;
				return TrafficLight::Green;
			}
			else if(avgPixel[0] > 128 && avgPixel[1] < 128 && avgPixel[2] < 128)
			{
				DebugOut(1)<<"Bluel Light!!!"<<endl;
			}
		}
		catch(...)
		{

		}

	}

	cv::namedWindow( "Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE );
	cv::imshow( "Hough Circle Transform Demo", img );


}
