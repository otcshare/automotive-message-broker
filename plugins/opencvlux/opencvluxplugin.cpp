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
#include <listplusplus.h>
#include <superptr.hpp>

#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

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

const std::string VideoLogging = "VideoLogging";
const std::string DriverDrowsiness = "DriverDrowsiness";
const std::string OpenCL = "OpenCL";

#include "debugout.h"

extern "C" AbstractSource * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	VehicleProperty::registerProperty(VideoLogging, [](){
		return new BasicPropertyType<bool>(VideoLogging, false);
	});

	VehicleProperty::registerProperty(DriverDrowsiness, [](){
		return new OpenCvLuxPlugin::DriverDrowsinessType(DriverDrowsiness, false);
	});

	VehicleProperty::registerProperty(OpenCL, [](){
		return new BasicPropertyType<bool>(OpenCL, false);
	});

	return new OpenCvLuxPlugin(routingengine, config);
}

OpenCvLuxPlugin::OpenCvLuxPlugin(AbstractRoutingEngine* re, map<string, string> config)
	:AbstractSource(re, config), lastLux(0), speed(0), latitude(0), longitude(0), extBrightness(new VehicleProperty::ExteriorBrightnessType(0))
{
	driverDrowsiness = amb::make_unique(new DriverDrowsinessType(DriverDrowsiness, false));
	openCl = amb::make_unique(new BasicPropertyType<bool>(OpenCL, false));

	shared = amb::make_unique(new Shared);
	shared->parent = this;

	shared->m_capture = NULL;
	shared->mWriter = nullptr;

	shared->threaded = false;
	shared->kinect = false;
	shared->useOpenCl = false;
	shared->useCuda = false;
	shared->loggingOn = false;
	shared->ddd = false;

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
#ifdef OPENCL
		shared->useOpenCl = config["opencl"] == "true";
		openCl->setValue(shared->useOpenCl);
#else
		DebugOut(DebugOut::Warning) << "You really don't have openCL support.  Disabling." << endl;
		shared->useOpenCl = false;
#endif

	}

	if(config.find("cuda") != config.end())
	{
#ifdef CUDA
		shared->useCuda = config["cuda"] == "true";
#endif
	}

	if(config.find("ddd") != config.end())
	{
		shared->ddd = config["ddd"] == "true";
	}

	if(config.find("logging") != config.end())
	{
		shared->loggingOn = config["logging"] == "true";
	}


#ifdef OPENCL

	cv::ocl::PlatformsInfo platforms;
	cv::ocl::getOpenCLPlatforms(platforms);

	for(auto p : platforms)
	{
		DebugOut(0)<<"platform: "<<p->platformName<<" vendor: "<<p->platformVendor<<endl;
	}

	cv::ocl::DevicesInfo info;
	cv::ocl::getOpenCLDevices(info, cv::ocl::CVCL_DEVICE_TYPE_ALL);

	DebugOut(0)<<"There are "<<info.size()<<" OpenCL devices on this system"<<endl;

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

	if(shared->ddd)
	{
#ifdef OPENCL
		faceCascade = amb::make_unique(new cv::ocl::OclCascadeClassifier());
		eyeCascade = amb::make_unique(new cv::ocl::OclCascadeClassifier());
#else
		faceCascade = amb::make_unique(new cv::CascadeClassifier());
		eyeCascade = amb::make_unique(new cv::CascadeClassifier());
#endif

		std::string faceFile = CV_DATA "/haarcascades/haarcascade_frontalface_alt.xml";
		if(!faceCascade->load(faceFile))
		{
			DebugOut(DebugOut::Warning) << "Could not load face cascade: " << faceFile <<". Disabling ddd" << endl;
			shared->ddd = false;
		}

		std::string eyeFile = CV_DATA "/haarcascades/haarcascade_eye_tree_eyeglasses.xml";

		if(!eyeCascade->load(eyeFile))
		{
			DebugOut(DebugOut::Warning) << "Could not load eye cascade: " << eyeFile <<". Disabling ddd" << endl;
			shared->ddd = false;
		}

	}

	routingEngine->subscribeToProperty(VehicleProperty::VehicleSpeed, this);
	routingEngine->subscribeToProperty(VehicleProperty::Latitude, this);
	routingEngine->subscribeToProperty(VehicleProperty::Longitude, this);
}

OpenCvLuxPlugin::~OpenCvLuxPlugin()
{

}

const string OpenCvLuxPlugin::uuid()
{
	return "3c7a1ea0-7d2e-11e2-9e96-0800200c9a66";
}

void OpenCvLuxPlugin::getPropertyAsync(AsyncPropertyReply *reply)
{
	reply->timedout = [this](AsyncPropertyReply* reply) {
		removeOne(&replyQueue, reply);
	};

	if(!shared->m_capture || !shared->m_capture->isOpened())
	{
		/// we want to turn on the camera for one shot to get an image and determine the intensity

		if(init())
			grabImage(shared.get());
	}

	if(reply->property == VehicleProperty::ExteriorBrightness)
	{
		replyQueue.push_back(reply);
	}
	else if(reply->property == VideoLogging)
	{
		BasicPropertyType<bool> tmp(VideoLogging, shared->loggingOn);
		reply->value = &tmp;
		reply->success = true;
		reply->completed(reply);
	}
	else  ///We don't support what you are asking for.  Reply false
	{
		reply->value = nullptr;
		reply->success = false;
		reply->error = AsyncPropertyReply::InvalidOperation;
		reply->completed(reply);
	}
}

void OpenCvLuxPlugin::getRangePropertyAsync(AsyncRangePropertyReply *reply)
{
	throw std::runtime_error("OpenCVLuxPlugin does not support this operation.  We should never hit this method.");
}

AsyncPropertyReply *OpenCvLuxPlugin::setProperty(AsyncSetPropertyRequest request )
{
	AsyncPropertyReply *reply = new AsyncPropertyReply(request);

	reply->success = false;
	reply->error = AsyncPropertyReply::InvalidOperation;

	if(request.property == VideoLogging)
	{
		shared->loggingOn = request.value->value<bool>();
		reply->success = true;
		reply->error = AsyncPropertyReply::NoError;
	}

	reply->completed(reply);

	return reply;
}

void OpenCvLuxPlugin::subscribeToPropertyChanges(VehicleProperty::Property property)
{
	if(!shared->mRequests.size())
	{
		if(init())
			g_timeout_add(1000 / shared->fps, grabImage, shared.get());
	}

	shared->mRequests.push_back(property);
}

void OpenCvLuxPlugin::unsubscribeToPropertyChanges(VehicleProperty::Property property)
{
	removeOne(&shared->mRequests,property);
}

PropertyList OpenCvLuxPlugin::supported()
{
	PropertyList props;
	props.push_back(VehicleProperty::ExteriorBrightness);
	props.push_back(DriverDrowsiness);
	props.push_back(VideoLogging);

	return props;
}

int OpenCvLuxPlugin::supportedOperations()
{
	return Get | Set;
}

void OpenCvLuxPlugin::propertyChanged(AbstractPropertyType *value)
{
	QMutexLocker lock(&mutex);
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

void OpenCvLuxPlugin::supportedChanged(const PropertyList &)
{
	DebugOut()<<"OpenCvLuxPlugin::supported changed."<<endl;
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
		*(shared->m_capture.get()) >> m_image;
	}

	if(shared->ddd)
	{
		try
		{
			shared->parent->detectEyes(m_image);
		}
		catch(...)
		{
			DebugOut(DebugOut::Warning) << "DDD detection exception: "<< endl;
		}
	}

	if(shared->threaded)
	{
		QFutureWatcher<uint> *watcher = new QFutureWatcher<uint>();
		QObject::connect(watcher, &QFutureWatcher<uint>::finished, shared->parent, &OpenCvLuxPlugin::imgProcResult);

		QFuture<uint> future = QtConcurrent::run(evalImage, m_image, shared);
		watcher->setFuture(future);

		QtConcurrent::run(shared->parent, &OpenCvLuxPlugin::writeVideoFrame, m_image);
	}
	else
	{
		shared->parent->writeVideoFrame(m_image);
		try
		{
			int lux = evalImage(m_image, shared);
			shared->parent->updateProperty(lux);
		}
		catch(...)
		{
			DebugOut(DebugOut::Warning) << "Exception caught "<< __FUNCTION__ << endl;
		}

		//detectLight(m_image,shared);

	}

	if(shared->mRequests.size())
	{
		return true;
	}

	return false;
}

static uint evalImage(cv::Mat qImg, OpenCvLuxPlugin::Shared *shared)
{
	if(qImg.empty())
	{
		DebugOut(DebugOut::Warning)<<"Empty image frame."<<endl;
		return 0;
	}

	cv::Scalar avgPixelIntensity;


	if(shared->useOpenCl)
	{
#ifdef OPENCL
		cv::Scalar stdDev;
		cv::ocl::oclMat src(qImg), gray;
		cv::ocl::cvtColor(src, gray, CV_BGR2GRAY);
		cv::ocl::meanStdDev(gray, avgPixelIntensity, stdDev);
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
	if(!shared->m_capture && shared->kinect)
	{
		shared->m_capture = amb::make_unique(new cv::VideoCapture(CV_CAP_OPENNI));
	}
	else if(!shared->m_capture)
	{
		if(device == "")
			shared->m_capture = amb::make_unique(new cv::VideoCapture(0));
		else
			shared->m_capture = amb::make_unique(new cv::VideoCapture(atoi(device.c_str())));
	}

	if(!shared->m_capture->isOpened())
	{
		DebugOut()<<"we failed to open camera device ("<<device<<") or no camera found"<<endl;
		return false;
	}

	if(configuration.find("logging") != configuration.end() &&
			configuration["logging"] == "true" &&
			(!shared->mWriter || !shared->mWriter->isOpened()))
	{
		cv::Size s = cv::Size((int) shared->m_capture->get(CV_CAP_PROP_FRAME_WIDTH),
							  (int) shared->m_capture->get(CV_CAP_PROP_FRAME_HEIGHT));

		std::string codec = configuration["codec"];

		if(codec.empty() || codec.size() != 4)
		{
			DebugOut(DebugOut::Warning)<<"Invalid codec.  Using default: MJPG"<<endl;
			codec = "MJPG";
		}

		std::string filename = configuration["logfile"];

		if(filename.empty()) filename = "/tmp/video.avi";

		boost::algorithm::to_upper(codec);

		shared->mWriter = amb::make_unique(new cv::VideoWriter(filename, CV_FOURCC(codec.at(0), codec.at(1), codec.at(2), codec.at(3)),30,s));
	}

	DebugOut()<<"camera frame width: "<<shared->m_capture->get(CV_CAP_PROP_FRAME_WIDTH)<<endl;
	DebugOut()<<"camera frame height: "<<shared->m_capture->get(CV_CAP_PROP_FRAME_HEIGHT)<<endl;
	DebugOut()<<"camera frame fps: "<<shared->m_capture->get(CV_CAP_PROP_FPS)<<endl;

	return true;
}



void OpenCvLuxPlugin::writeVideoFrame(cv::Mat f)
{
	QMutexLocker locker(&mutex);

	if(shared->loggingOn && speed > 0)
	{
		cv::Mat frame;
		f.copyTo(frame);

		std::stringstream str;
		str<<"Speed: "<<speed<<" kph, Lat/Lon: "<<latitude<<"/"<<longitude;

		std::stringstream datestr;
		datestr<<QDateTime::currentDateTime().toString().toStdString();

		locker.unlock();

		std::string text = str.str();

		int fontFace = cv::FONT_HERSHEY_SIMPLEX;
		double fontScale = 1;
		int thickness = 3;

		int baseline=0;
		cv::Size textSize = cv::getTextSize(text, fontFace,
									fontScale, thickness, &baseline);

		cv::Size dateTextSize = cv::getTextSize(datestr.str(), fontFace,
									fontScale, thickness, &baseline);

		baseline += thickness;

		// center the text
		cv::Point textOrg((frame.cols - textSize.width)/2,
					  (frame.rows - textSize.height));

		cv::Point dateOrg((frame.cols - dateTextSize.width)/2, dateTextSize.height);

		// then put the text itself
		cv::putText(frame, text, textOrg, fontFace, fontScale,
				cv::Scalar::all(255), thickness, 8);

		cv::putText(frame, datestr.str(), dateOrg, fontFace, fontScale,
				cv::Scalar::all(255), thickness, 8);

		(*shared->mWriter) << frame;
		(*shared->mWriter) << frame;
	}
}

void OpenCvLuxPlugin::updateProperty(uint lux)
{
	 extBrightness->setValue(lux);

	for(auto reply : replyQueue)
	{
		reply->value = extBrightness.get();
		reply->success = true;
		try{
			if(reply->completed)
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
		routingEngine->updateProperty(extBrightness.get(), uuid());
	}


}

void OpenCvLuxPlugin::imgProcResult()
{
	try
	{
		QFutureWatcher<uint> *watcher = dynamic_cast<QFutureWatcher<uint>*>(sender());

		uint lux = watcher->result();
		shared->parent->updateProperty(lux);

		watcher->deleteLater();
	}
	catch(...)
	{
		DebugOut(DebugOut::Warning) << "exception caught getting img processing result" << endl;
	}
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
				DebugOut(1)<<"Yellow Light!!!"<<endl;
				return TrafficLight::Yellow;
			}
		}
		catch(...)
		{

		}

	}

	cv::namedWindow( "Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE );
	cv::imshow( "Hough Circle Transform Demo", img );
}


void OpenCvLuxPlugin::detectEyes(cv::Mat frame)
{
	if(frame.empty())
		return;

	bool hasEyes = false;
	std::vector<cv::Rect> faces;
	cv::Mat frameGray;
#ifdef OPENCL
	cv::ocl::oclMat gray2;
#endif

	if(shared->useOpenCl)
	{
#ifdef OPENCL
		cv::ocl::oclMat src(frame);
		cv::ocl::cvtColor(src, gray2, CV_BGR2GRAY);
		cv::ocl::equalizeHist(gray2, gray2);
		cv::ocl::OclCascadeClassifier* fc = static_cast<cv::ocl::OclCascadeClassifier*>(faceCascade.get());
		fc->detectMultiScale(gray2, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
#endif
	}
	else
	{
		cv::cvtColor(frame, frameGray, CV_BGR2GRAY);
		cv::equalizeHist(frameGray, frameGray);
		faceCascade->detectMultiScale(frameGray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
	}

	for( size_t i = 0; i < faces.size(); i++ )
	{
		cv::Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
		cv::ellipse( frame, center, cv::Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, cv::Scalar( 255, 0, 255 ), 4, 8, 0 );

		std::vector<cv::Rect> eyes;

		if(shared->useOpenCl)
		{
#ifdef OPENCL
			cv::ocl::oclMat faceROI(gray2(faces[i]));
			DebugOut() << "oclMat.rows: " << faceROI.rows << endl;
			cv::ocl::OclCascadeClassifier* ec = static_cast<cv::ocl::OclCascadeClassifier*>(eyeCascade.get());
			ec->detectMultiScale( faceROI, eyes, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30) );
#endif
		}
		else
		{
			cv::Mat faceROI(frameGray(faces[i]));
			eyeCascade->detectMultiScale( faceROI, eyes, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30) );
		}

		for( size_t j = 0; j < eyes.size(); j++ )
		{
			cv::Point center( faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5 );
			int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
			cv::circle( frame, center, radius, cv::Scalar( 255, 0, 0 ), 4, 8, 0 );
		}

		if(eyes.size())
		{
			hasEyes = true;
			DebugOut() << "Number of eyes: " << eyes.size() << endl;
			if(!driverDrowsiness->basicValue() && shared->loggingOn)
			{
				cv::namedWindow("Eyes", CV_WINDOW_AUTOSIZE);
				cv::imshow("Eyes", frame);
			}

			driverDrowsiness->setValue(true);
			routingEngine->updateProperty(driverDrowsiness.get(), this->uuid());
		}
		else
		{
			DebugOut() << "No eyes!!!" << endl;
			driverDrowsiness->setValue(false);
			routingEngine->updateProperty(driverDrowsiness.get(), this->uuid());
		}
	}


}
