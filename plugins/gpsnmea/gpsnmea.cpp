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

#include <iostream>
#include <boost/assert.hpp>
#include <boost/regex.hpp>
#include <time.h>

using namespace std;

#include "debugout.h"
#include "abstractpropertytype.h"

std::string gprmcRegEx = "[\\$]?GPRMC,([0-1][0-9]|2[0-3])([0-5][0-9])([0-5][0-9])," /** time hh mm ss **/
		"([AV])," /** Status A= Active, V= Void **/
		"([0-8][0-9]|90)([0-5][0-9]\\.[0-9]{4})," /** latitude **/
		"([NS])," /** lat North or South **/
		"(180|0[0-9]{2}|1[0-7][0-9])([0-5][0-9]\\.[0-9]{4})," /** longitude **/
		"([EW])," /** lon E or W **/
		"([0-9]{3}\\.[0-9])," /** Speed in knots **/
		"(3[0-5][0-9]|[0-2][0-9]{2}\\.[0-9])," /** Direction **/
		"(3[0-1]|[1-2][0-9]|0[1-9])(0[1-9]|1[0-2])([0-9]{2}),"
		"(180|[1][0-7][0-9]|[0][0-9]{2}\\.[0-9])," /** Magnetic variation **/
		"([EW])" /** Magnetic Direction **/
		"\\*([0-9,A-F]{2})";

class Location
{
public:
	Location();

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

private: ///methods:

	void parseGprmc(string gprmc);

	void parseTime(std::string h, std::string m, std::string s);
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

	boost::regex regularExpression;
	bool isActive;

};

Location::Location()
	:mGpsTime("GpsTime",0), isActive(false)
{

}

void Location::parse(string nmea)
{
	if(boost::algorithm::starts_with(nmea,"$GPRMC"))
	{
		parseGprmc(nmea);
	}
}

void Location::parseGprmc(string gprmc)
{
	regularExpression.assign(gprmcRegEx);

	boost::smatch tokens;

	if (boost::regex_match (gprmc, tokens, regularExpression) )
	{
		parseTime(tokens[1],tokens[2],tokens[3]);

		if(tokens[4] == "A")
		{
			isActive = true;
		}

		parseLatitude(tokens[5], tokens[6], tokens[7]);
		parseLongitude(tokens[8], tokens[9], tokens[10]);
		parseSpeed(tokens[11]);
		parseDirection(tokens[12]);
	}
}

void Location::parseTime(string h, string m, string s)
{
	tm t;
	t.tm_hour = boost::lexical_cast<int>(h);
	t.tm_min = boost::lexical_cast<int>(m);
	t.tm_sec = boost::lexical_cast<int>(s);

	time_t time = mktime(&t);

	mGpsTime.setValue((double)time);
}

void Location::parseLatitude(string d, string m, string ns)
{
	double degs = boost::lexical_cast<double>(d + m);
	double dec = degsToDecimal(degs);

	if(ns == "S")
		dec *= -1;

	mLatitude.setValue(dec);
}

void Location::parseLongitude(string d, string m, string ew)
{
	double degs = boost::lexical_cast<double>(d + m);
	double dec = degsToDecimal(degs);

	if(ew == "E")
		dec *= -1;

	mLongitude.setValue(dec);
}

void Location::parseSpeed(string spd)
{
	double s = boost::lexical_cast<double>(spd);

	///to kph:
	s *= 1.852;

	mSpeed = VehicleProperty::VehicleSpeedType(s);
}

void Location::parseDirection(string dir)
{
	uint16_t d = boost::lexical_cast<double>(dir);
	mDirection = VehicleProperty::DirectionType(d);
}

void Location::parseAltitude(string alt)
{

}

double Location::degsToDecimal(double degs)
{
	double deg;
	double min = 100.0 * modf(degs / 100.0, &deg);
	return deg + (min / 60.0);
}

extern "C" AbstractSource * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new GpsNmeaSource(routingengine, config);
	
}

GpsNmeaSource::GpsNmeaSource(AbstractRoutingEngine *re, map<string, string> config)
	:AbstractSource(re,config)
{
	addPropertySupport(VehicleProperty::Latitude, Zone::None);
	addPropertySupport(VehicleProperty::Longitude, Zone::None);
	//addPropertySupport(VehicleProperty::Altitude, Zone::None);
	addPropertySupport(VehicleProperty::VehicleSpeed, Zone::None);
	addPropertySupport(VehicleProperty::Direction, Zone::None);


	///test:

	Location location;
	location.parse("$GPRMC,061211,A,2351.9605,S,15112.5239,E,000.0,053.4,170303,009.9,E*6E");

	DebugOut()<<location.latitude().toString()<<endl;
}

string GpsNmeaSource::uuid()
{
	return "33d86462-1708-4f78-a001-99ea8d55422b";
}


void GpsNmeaSource::getPropertyAsync(AsyncPropertyReply *reply)
{
	DebugOut()<<"GpsNmeaSource: getPropertyAsync called for property: "<<reply->property<<endl;

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
