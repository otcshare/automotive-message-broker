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
#include <ambplugin.h>

#include <iostream>
#include <thread>
#include <opencv2/opencv.hpp>
#include <opencv2/core/ocl.hpp>

#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>


#include <opencv2/core/ocl.hpp>

using namespace std;

const std::string VideoLogging = "VideoLogging";
const std::string DriverDrowsiness = "DriverDrowsiness";
const std::string OpenCL = "OpenCL";

#include "debugout.h"

extern "C" void create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	auto plugin = new AmbPlugin<OpenCvLuxPlugin>(routingengine, config);
	plugin->init();
}

OpenCvLuxPlugin::OpenCvLuxPlugin(AbstractRoutingEngine* re, map<string, string> config, AbstractSource &parent)
	:AmbPluginImpl(re, config, parent), lastLux(0), speed(0), latitude(0), longitude(0)
{
	videoLogging = addPropertySupport(Zone::None, [](){
		return new BasicPropertyType<bool>(VideoLogging, false);
	});

	driverDrowsiness = addPropertySupport(Zone::None, [](){
		return new OpenCvLuxPlugin::DriverDrowsinessType(DriverDrowsiness, false);
	});

	openCl = addPropertySupport(Zone::None, [](){
		return new BasicPropertyType<bool>(OpenCL, false);
	});

	extBrightness = addPropertySupport<VehicleProperty::ExteriorBrightnessType>(Zone::None);

	shared = amb::make_unique(new Shared);
	shared->parent = this;

	shared->m_capture = NULL;
	shared->mWriter = nullptr;

	shared->threaded = false;
	shared->kinect = false;
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
		if(cv::ocl::haveOpenCL())
		{
			bool useOcl = config["opencl"] == "true";
			cv::ocl::setUseOpenCL(useOcl);
			openCl->setValue(cv::ocl::useOpenCL());
		}
		else
		{
			DebugOut(DebugOut::Warning) << "You really don't have openCL support." << endl;
		}
	}

	if(config.find("ddd") != config.end())
	{
		shared->ddd = config["ddd"] == "true";
	}

	if(config.find("logging") != config.end())
	{
		shared->loggingOn = config["logging"] == "true";
	}

	if(shared->ddd)
	{
		faceCascade = amb::make_unique(new cv::CascadeClassifier());
		eyeCascade = amb::make_unique(new cv::CascadeClassifier());

		std::string faceFile = config["faceCascade"];

		if(!faceCascade->load(faceFile))
		{
			DebugOut(DebugOut::Warning) << "Could not load face cascade: " << faceFile <<". Disabling ddd" << endl;
			shared->ddd = false;
		}

		std::string eyeFile = config["eyeCascade"];

		if(!eyeCascade->load(eyeFile))
		{
			DebugOut(DebugOut::Warning) << "Could not load eye cascade: " << eyeFile <<". Disabling ddd" << endl;
			shared->ddd = false;
		}

	}

	routingEngine->subscribeToProperty(VehicleProperty::VehicleSpeed, &parent);
	routingEngine->subscribeToProperty(VehicleProperty::Latitude, &parent);
	routingEngine->subscribeToProperty(VehicleProperty::Longitude, &parent);
}

OpenCvLuxPlugin::~OpenCvLuxPlugin()
{

}

const string OpenCvLuxPlugin::uuid() const
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

		init();
		grabImage(shared.get());
	}

	AmbPluginImpl::getPropertyAsync(reply);
}

void OpenCvLuxPlugin::getRangePropertyAsync(AsyncRangePropertyReply *reply)
{
	throw std::runtime_error("OpenCVLuxPlugin does not support this operation.  We should never hit this method.");
}

AsyncPropertyReply *OpenCvLuxPlugin::setProperty(AsyncSetPropertyRequest request )
{
	if(request.property == VideoLogging)
	{
		shared->loggingOn = request.value->value<bool>();
	}
	if(request.property == OpenCL)
	{
		QMutexLocker lock(&mutex);
		cv::ocl::setUseOpenCL(request.value->value<bool>());
	}

	return AmbPluginImpl::setProperty(request);;
}

void OpenCvLuxPlugin::subscribeToPropertyChanges(VehicleProperty::Property property)
{
	if(!shared->mRequests.size())
	{
		init();
		g_timeout_add(1000 / shared->fps, grabImage, shared.get());
	}

	shared->mRequests.push_back(property);
}

void OpenCvLuxPlugin::unsubscribeToPropertyChanges(VehicleProperty::Property property)
{
	removeOne(&shared->mRequests,property);
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

	shared->frameCount++;

	if(shared->frameCount >= shared->fps)
		shared->frameCount = 0;

	if(shared->frameCount % 4 != 0)
	{
		return true;
	}

	if(!shared->m_capture->isOpened())
		return false;

	cv::UMat m_image;

	*(shared->m_capture.get()) >> m_image;

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
		if(shared->parent->videoLogging->value<bool>())
			shared->parent->writeVideoFrame(m_image);
		try
		{
			uint16_t lux = evalImage(m_image, shared);
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

static uint evalImage(cv::UMat qImg, OpenCvLuxPlugin::Shared *shared)
{
	if(qImg.empty())
	{
		DebugOut(DebugOut::Warning)<<"Empty image frame."<<endl;
		return 0;
	}

	cv::Scalar avgPixelIntensity;

	avgPixelIntensity = cv::mean(qImg);

	double val = avgPixelIntensity.val[0];

	double qualifiedPixel = (val - shared->pixelLowerBound);

	if(qualifiedPixel < 0) qualifiedPixel = 0;

	uint lux = qualifiedPixel * (130000.0 / shared->pixelUpperBound);

	DebugOut(7)<<"average pixel value: "<<qualifiedPixel<<" lux: "<<lux<<endl;

	return lux;
}


void OpenCvLuxPlugin::init()
{
	if(!shared->m_capture)
	{
		if(device == "")
			shared->m_capture = amb::make_unique(new cv::VideoCapture(0));
		else
			shared->m_capture = amb::make_unique(new cv::VideoCapture(atoi(device.c_str())));
	}

	if(!shared->m_capture->isOpened())
	{
		DebugOut() << "we failed to open camera device (" << device << ") or no camera found" << endl;
		return;
	}

	if(!shared->mWriter || !shared->mWriter->isOpened())
	{
		cv::Size s = cv::Size((int) shared->m_capture->get(cv::CAP_PROP_FRAME_WIDTH),
							  (int) shared->m_capture->get(cv::CAP_PROP_FRAME_HEIGHT));

		std::string codec = configuration["codec"];

		if(codec.empty() || codec.size() != 4)
		{
			DebugOut(DebugOut::Warning)<<"Invalid codec: "<<codec <<". Using default: MJPG"<<endl;
			codec = "MJPG";
		}

		std::string filename = configuration["logfile"];

		if(filename.empty()) filename = "/tmp/video.avi";

		boost::algorithm::to_upper(codec);

		shared->mWriter = amb::make_unique(new cv::VideoWriter(filename, cv::VideoWriter::fourcc(codec.at(0), codec.at(1), codec.at(2), codec.at(3)),30,s));
	}

	DebugOut()<<"camera frame width: "<<shared->m_capture->get(cv::CAP_PROP_FRAME_WIDTH)<<endl;
	DebugOut()<<"camera frame height: "<<shared->m_capture->get(cv::CAP_PROP_FRAME_HEIGHT)<<endl;
	DebugOut()<<"camera frame fps: "<<shared->m_capture->get(cv::CAP_PROP_FPS)<<endl;

	return;
}



void OpenCvLuxPlugin::writeVideoFrame(cv::UMat f)
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

void OpenCvLuxPlugin::updateProperty(uint16_t lux)
{
	extBrightness->setValue(lux);

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

TrafficLight::Color detectLight(cv::UMat img, OpenCvLuxPlugin::Shared *shared)
{

	cv::UMat gray;

	cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
	cv::GaussianBlur(gray, gray, cv::Size(9,9), 2, 2);

	std::vector<cv::Vec3f> circles;

	//cv::HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, gray.rows/8, 200, 100, 0, 0);

	cv::HoughCircles( gray, circles, cv::HOUGH_GRADIENT, 2, 30, 231, 50, 0, 0 );

	for(auto i = 0; i < circles.size(); i++)
	{
		cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);

		cv::Rect rect(center.x - radius / 2, center.y - radius / 2, radius, radius);

		try {

			cv::UMat light(img, rect);

			cv::circle(img, center, radius, cv::Scalar(0,0,255), 3, 8, 0 );

			cv::rectangle(img, rect.tl(), rect.br(), cv::Scalar(255,0,0));

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
}


void OpenCvLuxPlugin::detectEyes(cv::UMat frame)
{
	if(frame.empty())
		return;

	bool hasEyes = false;
	std::vector<cv::Rect> faces;
	cv::UMat frameGray;


	cv::cvtColor(frame, frameGray, cv::COLOR_BGR2GRAY);
	cv::equalizeHist(frameGray, frameGray);
	faceCascade->detectMultiScale(frameGray, faces, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

	for( size_t i = 0; i < faces.size(); i++ )
	{
		cv::Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
		cv::ellipse( frame, center, cv::Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, cv::Scalar( 255, 0, 255 ), 4, 8, 0 );

		std::vector<cv::Rect> eyes;


		cv::UMat faceROI(frameGray(faces[i]));
		eyeCascade->detectMultiScale( faceROI, eyes, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30) );

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

			if(driverDrowsiness->value<bool>())
			{
				driverDrowsiness->setValue(false);
				routingEngine->updateProperty(driverDrowsiness.get(), this->uuid());
			}
		}
		else
		{
			if(!driverDrowsiness->value<bool>())
			{
				driverDrowsiness->setValue(true);
				routingEngine->updateProperty(driverDrowsiness.get(), this->uuid());
			}
		}
	}
}
