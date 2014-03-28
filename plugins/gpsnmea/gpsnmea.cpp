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

#include "gpsnmea.h"
#include "timestamp.h"
#include "serialport.hpp"
#include "bluetooth.hpp"

#include <iostream>
#include <boost/assert.hpp>
#include <boost/algorithm/string.hpp>
#include <time.h>


using namespace std;

#include "debugout.h"
#include "abstractpropertytype.h"

#define GPSTIME "GpsTime"

template<typename T2>
inline T2 lexical_cast(const std::string &in) {
	T2 out;
	std::stringstream ss;
	ss << std::hex << in;
	ss >> out;
	return out;
}

class Location
{
public:
	Location(AbstractRoutingEngine* re, std::string uuid);

	void parse(std::string gprmc);

	VehicleProperty::LatitudeType latitude()
	{
		return mLatitude;
	}

	VehicleProperty::LongitudeType longitude()
	{
		return mLongitude;
	}

	VehicleProperty::AltitudeType altitude()
	{
		return mAltitude;
	}

	VehicleProperty::DirectionType direction()
	{
		return mDirection;
	}

	VehicleProperty::VehicleSpeedType speed()
	{
		return mSpeed;
	}

	BasicPropertyType<double> gpsTime()
	{
		return mGpsTime;
	}

	std::list<AbstractPropertyType*> fix()
	{
		std::list<AbstractPropertyType*> l;

		l.push_back(&mLatitude);
		l.push_back(&mLongitude);
		l.push_back(&mAltitude);
		l.push_back(&mDirection);
		l.push_back(&mSpeed);
		l.push_back(&mGpsTime);

		return l;
	}

private: ///methods:

	void parseGprmc(string gprmc);
	void parseGpgga(string gpgga);

	void parseTime(std::string h, std::string m, std::string s, string dd, string mm, string yy);
	void parseLatitude(std::string d, std::string m, std::string ns);
	void parseLongitude(std::string d, string m, string ew);
	void parseSpeed(std::string spd);
	void parseDirection(std::string dir);
	void parseAltitude(std::string alt);

	double degsToDecimal(double degs);

private:

	VehicleProperty::LatitudeType mLatitude;
	VehicleProperty::LongitudeType mLongitude;
	VehicleProperty::AltitudeType mAltitude;
	VehicleProperty::DirectionType  mDirection;
	VehicleProperty::VehicleSpeedType mSpeed;
	BasicPropertyType<double> mGpsTime;

	bool isActive;

	std::string mUuid;

	AbstractRoutingEngine* routingEngine;

};

Location::Location(AbstractRoutingEngine* re, std::string uuid)
	:mLatitude(0), mLongitude(0), mAltitude(0), mDirection(0), mSpeed(0), mGpsTime(GPSTIME,0), isActive(false), routingEngine(re), mUuid(uuid)
{

}

void Location::parse(string nmea)
{
	if(boost::algorithm::starts_with(nmea,"GPRMC"))
	{
		parseGprmc(nmea);
	}
	else if(boost::algorithm::starts_with(nmea,"GPGGA"))
	{
		parseGpgga(nmea);
	}
	else
	{
		DebugOut(DebugOut::Warning)<<"unknown/unhandled message: "<<nmea<<endl;
	}
}

void Location::parseGprmc(string gprmc)
{
	DebugOut(7)<<"parsing gprmc message"<<endl;

	std::vector<std::string> tokens;
	boost::split(tokens, gprmc, boost::is_any_of(","));

	if(tokens[2] == "A")
	{
		isActive = true;
	}

	parseTime(tokens[1].substr(0,2),tokens[1].substr(2,2),tokens[1].substr(4,2),tokens[9].substr(0,2),tokens[9].substr(2,2),tokens[9].substr(4,2));

	parseLatitude(tokens[3], "", tokens[4]);
	parseLongitude(tokens[5], "", tokens[6]);
	parseSpeed(tokens[7]);
	parseDirection(tokens[8]);

}

void Location::parseGpgga(string gpgga)
{

	std::vector<std::string> tokens;
	boost::split(tokens, gpgga, boost::is_any_of(","));

	if(tokens.size() != 15)
	{
		DebugOut()<<"Invalid GPGGA message: "<<gpgga<<endl;
		return;
	}

	parseLatitude(tokens[2],"",tokens[3]);
	parseLongitude(tokens[4],"",tokens[5]);
	if(tokens[6] != "0")
	{
		isActive = true;
	}
	else isActive = false;

	parseAltitude(tokens[9]);
}

void Location::parseTime(string h, string m, string s, string dd, string mm, string yy)
{
	try
	{
		tm t;
		t.tm_hour = boost::lexical_cast<int>(h);
		t.tm_min = boost::lexical_cast<int>(m);
		t.tm_sec = boost::lexical_cast<int>(s);
		t.tm_mday = boost::lexical_cast<int>(dd);
		t.tm_mon = boost::lexical_cast<int>(mm);
		t.tm_year = boost::lexical_cast<int>(yy) + 100;

		time_t time = mktime(&t);

		BasicPropertyType<double> temp(GPSTIME,(double)time);

		if(mGpsTime != temp)
		{
			mGpsTime = temp;
			routingEngine->updateProperty(&mGpsTime, mUuid);
		}
	}
	catch(...)
	{
		DebugOut(DebugOut::Warning)<<"Failed to parse time "<<endl;
	}
}

void Location::parseLatitude(string d, string m, string ns)
{
	try
	{
		if(d.empty() )
			return;

		double degs = boost::lexical_cast<double>(d + m);
		double dec = degsToDecimal(degs);

		if(ns == "S")
			dec *= -1;

		VehicleProperty::LatitudeType temp(dec);

		if(mLatitude != temp)
		{
			mLatitude = temp;\
			routingEngine->updateProperty(&mLatitude, mUuid);
		}
	}
	catch(...)
	{
		DebugOut(DebugOut::Warning)<<"Failed to parse latitude"<<endl;
	}
}

void Location::parseLongitude(string d, string m, string ew)
{
	try
	{
		if(d.empty()) return;

		double degs = boost::lexical_cast<double>(d + m);
		double dec = degsToDecimal(degs);

		if(ew == "W")
			dec *= -1;

		VehicleProperty::LongitudeType temp(dec);

		if(mLongitude != temp)
		{
			mLongitude = temp;\
			routingEngine->updateProperty(&mLongitude, mUuid);
		}
	}
	catch(...)
	{
		DebugOut(DebugOut::Warning)<<"failed to parse longitude"<<endl;
	}
}

void Location::parseSpeed(string spd)
{
	try
	{
		double s = boost::lexical_cast<double>(spd);

		///to kph:
		s *= 1.852;
		VehicleProperty::VehicleSpeedType temp(s);
		if(mSpeed != temp)
		{
			mSpeed = temp;
			routingEngine->updateProperty(&mSpeed, mUuid);
		}
	}
	catch(...)
	{
		DebugOut(DebugOut::Warning)<<"failed to parse speed"<<endl;
	}
}

void Location::parseDirection(string dir)
{
	try {
		uint16_t d = boost::lexical_cast<double>(dir);

		VehicleProperty::DirectionType temp(d);
		if(mDirection != temp)
		{
			mDirection = temp;
			routingEngine->updateProperty(&mDirection, mUuid);
		}
	}
	catch(...)
	{
		DebugOut(DebugOut::Warning)<<"Failed to parse direction: "<<dir<<endl;
	}
}

void Location::parseAltitude(string alt)
{
	try{

		if(alt.empty()) return;

		double a = boost::lexical_cast<double>(alt);

		VehicleProperty::AltitudeType temp(a);
		if(mAltitude != temp)
		{
			mAltitude = temp;
			routingEngine->updateProperty(&mAltitude, mUuid);
		}

		mAltitude = VehicleProperty::AltitudeType(a);
	}
	catch(...)
	{
		DebugOut(DebugOut::Warning)<<"failed to parse altitude"<<endl;
	}
}

double Location::degsToDecimal(double degs)
{
	double deg;
	double min = 100.0 * modf(degs / 100.0, &deg);
	return deg + (min / 60.0);
}

bool readCallback(GIOChannel *source, GIOCondition condition, gpointer data)
{
//	DebugOut(5) << "Polling..." << condition << endl;

	if(condition & G_IO_ERR)
	{
		DebugOut(DebugOut::Error)<<"GpsNmeaSource polling error."<<endl;
	}

	if (condition & G_IO_HUP)
	{
		//Hang up. Returning false closes out the GIOChannel.
		//printf("Callback on G_IO_HUP\n");
		DebugOut(DebugOut::Warning)<<"socket hangup event..."<<endl;
		return false;
	}

	GpsNmeaSource* src = static_cast<GpsNmeaSource*>(data);

	src->canHasData();

	return true;
}

extern "C" AbstractSource * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new GpsNmeaSource(routingengine, config);
	
}

GpsNmeaSource::GpsNmeaSource(AbstractRoutingEngine *re, map<string, string> config)
	:AbstractSource(re,config), mUuid("33d86462-1708-4f78-a001-99ea8d55422b")
{
	location =new Location(re, mUuid);

	VehicleProperty::registerProperty(GPSTIME,[](){ return new BasicPropertyType<double>(GPSTIME,0); });

	addPropertySupport(VehicleProperty::Latitude, Zone::None);
	addPropertySupport(VehicleProperty::Longitude, Zone::None);
	addPropertySupport(VehicleProperty::Altitude, Zone::None);
	addPropertySupport(VehicleProperty::VehicleSpeed, Zone::None);
	addPropertySupport(VehicleProperty::Direction, Zone::None);
	addPropertySupport(GPSTIME, Zone::None);


	///test:

	if(config.find("test") != config.end())
	{
		Location location(routingEngine, mUuid);
		location.parse("GPRMC,061211,A,2351.9605,S,15112.5239,E,000.0,053.4,170303,009.9,E*6E");

		DebugOut(0)<<"lat: "<<location.latitude().toString()<<endl;

		g_assert(location.latitude().toString() == "-23.86600833");
		g_assert(location.gpsTime().toString() == "1050585131");

		location.parse("GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47");

		DebugOut(0)<<"alt: "<<location.altitude().toString()<<endl;
		DebugOut(0)<<"lat: "<<location.latitude().toString()<<endl;
		g_assert(location.altitude().toString() == "545.4");
		g_assert(location.latitude().toString() == "48.1173");

		location.parse("GPRMC,060136.00,A,3101.40475,N,12126.87095,E,0.760,,160114,,,A*74");
		DebugOut(0)<<"lon: "<<location.longitude().toString()<<endl;
		DebugOut(0)<<"lat: "<<location.latitude().toString()<<endl;
		
		//Test incomplete message:
		location.parse("GPRMC,023633.00,V,,,,,,,180314,,,N*75");
		DebugOut(0)<<"lon: "<<location.longitude().toString()<<endl;
		DebugOut(0)<<"lat: "<<location.latitude().toString()<<endl;
	}

	std::string btaddapter = config["bluetoothAdapter"];

	if(config.find("device")!= config.end())
	{
		std::string dev = config["device"];
		if(dev.find(":") != string::npos)
		{
			BluetoothDevice bt;
			dev = bt.getDeviceForAddress(dev, btaddapter);
		}

		device = new SerialPort(dev);

		if(!device->open())
		{
			DebugOut(DebugOut::Error)<<"Failed to open gps tty: "<<config["device"]<<endl;
			perror("Error");
			return;
		}

		DebugOut()<<"read from device: "<<device->read()<<endl;

		GIOChannel *chan = g_io_channel_unix_new(device->fileDescriptor());
		g_io_add_watch(chan, GIOCondition(G_IO_IN | G_IO_HUP | G_IO_ERR),(GIOFunc)readCallback, this);
		g_io_channel_set_close_on_unref(chan, true);
		g_io_channel_unref(chan); //Pass ownership of the GIOChannel to the watch.
	}
}

GpsNmeaSource::~GpsNmeaSource()
{
	device->close();
}

const string GpsNmeaSource::uuid()
{
	return mUuid;
}


void GpsNmeaSource::getPropertyAsync(AsyncPropertyReply *reply)
{
	DebugOut()<<"GpsNmeaSource: getPropertyAsync called for property: "<<reply->property<<endl;

	std::list<AbstractPropertyType*> f = location->fix();

	for(auto property : f)
	{
		if(property->name == reply->property)
		{
			reply->success = true;
			reply->value = property;
			reply->completed(reply);
			return;
		}
	}

	reply->success = false;
	reply->error = AsyncPropertyReply::InvalidOperation;
	reply->completed(reply);
}

void GpsNmeaSource::getRangePropertyAsync(AsyncRangePropertyReply *reply)
{

}

AsyncPropertyReply *GpsNmeaSource::setProperty(AsyncSetPropertyRequest request )
{

}

void GpsNmeaSource::subscribeToPropertyChanges(VehicleProperty::Property property)
{
	mRequests.push_back(property);
}

PropertyList GpsNmeaSource::supported()
{
	return mSupported;
}

int GpsNmeaSource::supportedOperations()
{
	return Get;
}

void GpsNmeaSource::canHasData()
{
	std::string data = device->read();

	std::vector<std::string> lines;

	boost::split(lines,data,boost::is_any_of("$"));

	for(int i = 0; i < lines.size(); i++)
	{
		if(checksum(lines[i]))
		{
			buffer = lines[i];
		}
		else
		{
			buffer += lines[i];
		}

		if(checksum(buffer))
		{
			/// we have a complete message.  parse it!
			DebugOut(7)<<"Complete message: "<<buffer<<endl;
			location->parse(buffer);
		}

		DebugOut(7)<<"buffer: "<<buffer<<endl;

	}
}

void GpsNmeaSource::unsubscribeToPropertyChanges(VehicleProperty::Property property)
{
	mRequests.remove(property);
}

void GpsNmeaSource::addPropertySupport(VehicleProperty::Property property, Zone::Type zone)
{
	mSupported.push_back(property);

	std::list<Zone::Type> zones;

	zones.push_back(zone);

	PropertyInfo info(0, zones);

	propertyInfoMap[property] = info;
}

bool GpsNmeaSource::checksum(std::string sentence)
{
	if(sentence.empty() || sentence.length() < 4)
	{
		return false;
	}

	int checksum = 0;

	for(auto i : sentence)
	{
		if(i == '*')
			break;
		if(i != '\n' || i != '\r')
			checksum ^= i;
	}

	std::string sentenceCheckStr = sentence.substr(sentence.length()-4,2);

	try
	{
		int sentenceCheck = lexical_cast<int>(sentenceCheckStr);

		return sentenceCheck == checksum;
	}
	catch(...)

	{
		return false;
	}
}
