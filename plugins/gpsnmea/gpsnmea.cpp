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
#include "bluetooth5.h"
#include <listplusplus.h>
#include <superptr.hpp>
#include <ambplugin.h>

#include <dbusplugin.h>
#include <dbusexport.h>

#include <iostream>
#include <boost/assert.hpp>
#include <boost/algorithm/string.hpp>
#include <time.h>


using namespace std;

#include "debugout.h"
#include "abstractpropertytype.h"

#define GPSTIME "GpsTime"
#define GPSSPEED "GpsSpeed"
#define GpsFix "GpsFix"
#define GpsSatsUsed "GpsSatsUsed"
#define GpsNmea "GpsNmea"

template<typename T2>
inline T2 lexical_cast(const std::string &in) {
	T2 out;
	std::stringstream ss;
	ss << std::hex << in;
	ss >> out;
	return out;
}

class GpsInfo: public DBusSink
{
public:
	GpsInfo(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("GpsInfo", re, connection, map<string, string>())
	{
		wantPropertyVariant(GPSTIME, "GpsTime", VariantType::Read);
		wantPropertyVariant(GPSSPEED, "Speed", VariantType::Read);
		wantPropertyVariant(GpsFix, "Fix", VariantType::Read);
		wantPropertyVariant(GpsSatsUsed, "SattelitesUsed", VariantType::Read);
		wantPropertyVariant(GpsNmea, "RawNmea", VariantType::Read);
	}
};

class Location
{
public:
	enum FixType {
		NoFix = 1,
		Fix2D = 2,
		Fix3D = 3
	};

	Location(AmbPluginImpl *source, std::shared_ptr<AbstractPropertyType> lat,
			 std::shared_ptr<AbstractPropertyType> lon,
			 std::shared_ptr<AbstractPropertyType> alt,
			 std::shared_ptr<AbstractPropertyType> dir,
			 std::shared_ptr<AbstractPropertyType> spd,
			 std::shared_ptr<AbstractPropertyType> time,
			 std::shared_ptr<AbstractPropertyType> fix,
			 std::shared_ptr<AbstractPropertyType> satsUsed,
			 std::shared_ptr<AbstractPropertyType> vspd);

	void parse(std::string gprmc);

	AbstractPropertyType * latitude()
	{
		return mLatitude.get();
	}

	AbstractPropertyType * longitude()
	{
		return mLongitude.get();
	}

	AbstractPropertyType * altitude()
	{
		return mAltitude.get();
	}

	AbstractPropertyType * direction()
	{
		return mDirection.get();
	}

	AbstractPropertyType * speed()
	{
		return mSpeed.get();
	}

	AbstractPropertyType * gpsTime()
	{
		return mGpsTime.get();
	}

	std::list<AbstractPropertyType*> fix()
	{
		std::list<AbstractPropertyType*> l;

		l.push_back(mLatitude.get());
		l.push_back(mLongitude.get());
		l.push_back(mAltitude.get());
		l.push_back(mDirection.get());
		l.push_back(mSpeed.get());
		l.push_back(mGpsTime.get());

		return l;
	}

private: ///methods:

	void parseGprmc(string gprmc);
	void parseGpgga(string gpgga);
	void parseGpgsa(string gpgsa);

	void parseTime(std::string h, std::string m, std::string s, string dd, string mm, string yy);
	void parseLatitude(std::string d, std::string m, std::string ns);
	void parseLongitude(std::string d, string m, string ew);
	void parseSpeed(std::string spd);
	void parseDirection(std::string dir);
	void parseAltitude(std::string alt);

	double degsToDecimal(double degs);

private:

	std::shared_ptr<AbstractPropertyType> mLatitude;
	std::shared_ptr<AbstractPropertyType> mLongitude;
	std::shared_ptr<AbstractPropertyType> mAltitude;
	std::shared_ptr<AbstractPropertyType> mDirection;
	std::shared_ptr<AbstractPropertyType> mSpeed;
	std::shared_ptr<AbstractPropertyType> mVehicleSpeed;
	std::shared_ptr<AbstractPropertyType> mGpsTime;
	std::shared_ptr<AbstractPropertyType> mFix;
	std::shared_ptr<AbstractPropertyType> mSatelitesUsed;

	bool isActive;

	AmbPluginImpl * parent;

};

Location::Location(AmbPluginImpl* source,
				   std::shared_ptr<AbstractPropertyType> lat,
				   std::shared_ptr<AbstractPropertyType> lon,
				   std::shared_ptr<AbstractPropertyType> alt,
				   std::shared_ptr<AbstractPropertyType> dir,
				   std::shared_ptr<AbstractPropertyType> spd,
				   std::shared_ptr<AbstractPropertyType> time,
				   std::shared_ptr<AbstractPropertyType> fix,
				   std::shared_ptr<AbstractPropertyType> satsUsed,
				   std::shared_ptr<AbstractPropertyType> vspd)
	:isActive(false), parent(false)
{
	mLatitude = lat;
	mLongitude = lon;
	mAltitude = alt;
	mDirection = dir;
	mSpeed = spd;
	mGpsTime = time;
	mFix = fix;
	mSatelitesUsed = satsUsed;
	mVehicleSpeed = vspd;
}

void Location::parse(string nmea)
{
	if(boost::algorithm::starts_with(nmea,"GPRMC") || boost::algorithm::starts_with(nmea,"GNRMC"))
	{
		parseGprmc(nmea);
	}
	else if(boost::algorithm::starts_with(nmea,"GPGGA"))
	{
		parseGpgga(nmea);
	}
	else if(boost::algorithm::starts_with(nmea, "GPGSA"))
	{
		parseGpgsa(nmea);
	}
	else
	{
		DebugOut(7)<<"unknown/unhandled message: "<<nmea<<endl;
	}
}

void Location::parseGprmc(string gprmc)
{
	DebugOut(7)<<"parsing gprmc message"<<endl;

	std::vector<std::string> tokens;
	boost::split(tokens, gprmc, boost::is_any_of(","));

	if(!tokens.size())
	{
		return;
	}

	if(tokens[2] == "A")
	{
		isActive = true;
	}

	if(tokens[1].empty() || tokens[9].empty() || tokens[3].empty() || tokens[4].empty() || tokens[5].empty() || tokens[6].empty() || tokens[7].empty() || tokens[8].empty())
	{
		return;
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

void Location::parseGpgsa(string gpgsa)
{
	std::vector<std::string> tokens;
	boost::split(tokens, gpgsa, boost::is_any_of(","));

	if(tokens.size() != 18)
	{
		DebugOut()<<"Invalid GPGSA message: "<<gpgsa<<endl;
		return;
	}

	Location::FixType fix = (Location::FixType)boost::lexical_cast<int>(tokens[2]);

	uint16_t numsats = 0;

	for(int i=3; i<15; i++)
	{
		std::string sat = tokens[i];
		if(!sat.empty())
		{
			numsats ++;
		}
	}

	if(mFix->value<Location::FixType>() != fix)
		parent->setValue(mFix, fix);

	if(mSatelitesUsed->value<uint16_t>() != numsats)
		parent->setValue(mSatelitesUsed, numsats);
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
		t.tm_mon = boost::lexical_cast<int>(mm) - 1;
		t.tm_year = boost::lexical_cast<int>(yy) + 100;

		time_t time = timegm(&t);

		if(mGpsTime->value<double>() != (double(time)))
		{
			parent->setValue(mGpsTime, double(time));
		}
	}
	catch(...)
	{
		DebugOut(5)<<"Failed to parse time "<<endl;
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

		if(mLatitude->value<double>() != dec)
		{
			parent->setValue(mLatitude, dec);
		}
	}
	catch(...)
	{
		DebugOut(5)<<"Failed to parse latitude"<<endl;
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

		if(mLongitude->value<double>() != dec)
		{
			parent->setValue(mLongitude, dec);
		}
	}
	catch(...)
	{
		DebugOut(5)<<"failed to parse longitude: "<<d<<" "<<m<<" "<<ew<<endl;
	}
}

void Location::parseSpeed(string spd)
{
	try
	{
		double s = boost::lexical_cast<double>(spd);

		///to kph:
		s *= 1.852;

		uint16_t speed = static_cast<uint16_t>(s);

		if(mSpeed->value<uint16_t>() != speed)
		{
			parent->setValue(mSpeed, speed);
		}
	}
	catch(...)
	{
		DebugOut(5)<<"failed to parse speed"<<endl;
	}
}

void Location::parseDirection(string dir)
{
	try {
		uint16_t d = boost::lexical_cast<double>(dir);

		if(mDirection->value<uint16_t>() != d)
		{
			parent->setValue(mDirection, d);
		}
	}
	catch(...)
	{
		DebugOut(5) << "Failed to parse direction: " << dir << endl;
	}
}

void Location::parseAltitude(string alt)
{
	try{

		if(alt.empty()) return;

		double a = boost::lexical_cast<double>(alt);

		if(mAltitude->value<double>() != a)
		{
			parent->setValue(mAltitude, a);
		}
	}
	catch(...)
	{
		DebugOut(5)<<"failed to parse altitude"<<endl;
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

extern "C" void create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	auto plugin = new AmbPlugin<GpsNmeaSource>(routingengine, config);
	plugin->init();
}

GpsNmeaSource::GpsNmeaSource(AbstractRoutingEngine *re, map<string, string> config, AbstractSource &parent)
	:AmbPluginImpl(re, config, parent), device(nullptr), mUuid("33d86462-1708-4f78-a001-99ea8d55422b"), bt(nullptr)
{
	int baudrate = 0;

	auto lat = addPropertySupport<VehicleProperty::LatitudeType>(Zone::None);
	auto lon = addPropertySupport<VehicleProperty::LongitudeType>(Zone::None);
	auto alt = addPropertySupport<VehicleProperty::AltitudeType>(Zone::None);
	auto spd = addPropertySupport(Zone::None, [](){ return new BasicPropertyType<uint16_t>(GPSSPEED, 0); });
	auto vspd = addPropertySupport<VehicleProperty::VehicleSpeedType>(Zone::None);
	auto dir = addPropertySupport<VehicleProperty::DirectionType>(Zone::None);
	auto time = addPropertySupport(Zone::None, [](){ return new BasicPropertyType<double>(GPSTIME, 0); });
	auto fix = addPropertySupport(Zone::None, []() { return new BasicPropertyType<Location::FixType>(GpsFix, Location::NoFix); });
	auto satsUsed = addPropertySupport(Zone::None, []() { return new BasicPropertyType<uint16_t>(GpsSatsUsed, 0); });
	rawNmea = addPropertySupport(Zone::None, []() { return new StringPropertyType(GpsNmea); });

	location = new Location(this, lat, lon, alt, dir, spd, time, fix, satsUsed, vspd);

	std::string btaddapter = config["bluetoothAdapter"];

	if(config.find("baudrate")!= config.end())
	{
		baudrate = boost::lexical_cast<int>( config["baudrate"] );
	}

	if(config.find("device")!= config.end())
	{
		std::string dev = config["device"];

		if(dev.find(":") != string::npos)
		{
#ifdef USE_BLUEZ5
			bt = new Bluetooth5();
			bt->getDeviceForAddress(dev, [this](int fd) {
				DebugOut() << "fd: " << fd << endl;
				device = new SerialPort(fd);
				int baudrate=0;

				if(baudrate!=0)
				{
					if((static_cast<SerialPort*>(device))->setSpeed(baudrate))
						DebugOut(DebugOut::Error)<<"Unsupported baudrate " << configuration["baudrate"] << endl;
				}

				DebugOut()<<"read from device: "<<device->read()<<endl;

				GIOChannel *chan = g_io_channel_unix_new(device->fileDescriptor());
				g_io_add_watch(chan, GIOCondition(G_IO_IN | G_IO_HUP | G_IO_ERR),(GIOFunc)readCallback, this);
				g_io_channel_set_close_on_unref(chan, true);
				g_io_channel_unref(chan);
			});
#else
			bt = new BluetoothDevice();
			dev = bt->getDeviceForAddress(dev, btaddapter);

			device = new SerialPort(dev);

			if(baudrate!=0)
			{
				if((static_cast<SerialPort*>(device))->setSpeed(baudrate))
					DebugOut(DebugOut::Error)<<"Unsupported baudrate " << config["baudrate"] << endl;
			}

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
			g_io_channel_unref(chan);
#endif
		}
		else
		{
			device = new SerialPort(dev);

			if(baudrate!=0)
			{
				if((static_cast<SerialPort*>(device))->setSpeed(baudrate))
					DebugOut(DebugOut::Error)<<"Unsupported baudrate " << config["baudrate"] << endl;
			}

			if(!device->open())
			{
				DebugOut(DebugOut::Error) << "Failed to open gps tty: " << config["device"] << endl;
				perror("Error");
				return;
			}

			DebugOut()<<"read from device: "<<device->read()<<endl;

			GIOChannel *chan = g_io_channel_unix_new(device->fileDescriptor());
			g_io_add_watch(chan, GIOCondition(G_IO_IN | G_IO_HUP | G_IO_ERR), (GIOFunc)readCallback, this);
			g_io_channel_set_close_on_unref(chan, true);
			g_io_channel_unref(chan);
		}
	}
}

GpsNmeaSource::~GpsNmeaSource()
{
	if(device && device->isOpen())
		device->close();
	if(bt)
		delete bt;
}

const string GpsNmeaSource::uuid() const
{
	return mUuid;
}

int GpsNmeaSource::supportedOperations() const
{
	return AbstractSource::Get;
}

void GpsNmeaSource::init()
{
	if(configuration.find("test") != configuration.end())
	{
		test();
	}

	routingEngine->subscribeToProperty(DBusConnected,[this](AbstractPropertyType* value)
	{
		if(value->name == DBusConnected)
		{
			if(value->value<bool>())
			{
				amb::Exporter::instance()->exportProperty<GpsInfo>(routingEngine);
			}
		}
	});
}

void GpsNmeaSource::canHasData()
{
	std::string data = device->read();

	tryParse(data);
}

void GpsNmeaSource::test()
{
	location->parse("GPRMC,061211,A,2351.9605,S,15112.5239,E,000.0,053.4,170303,009.9,E*6E");

	DebugOut(0)<<"lat: "<<location->latitude()->toString()<<endl;
	DebugOut(0)<<"lat: "<<location->gpsTime()->toString()<<endl;

	g_assert(location->latitude()->toString() == "-23.86600833");
	g_assert(location->gpsTime()->toString() == "1050585131");

	location->parse("GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47");

	DebugOut(0)<<"alt: "<<location->altitude()->toString()<<endl;
	DebugOut(0)<<"lat: "<<location->latitude()->toString()<<endl;
	g_assert(location->altitude()->toString() == "545.4");
	g_assert(location->latitude()->toString() == "48.1173");

	location->parse("GPRMC,060136.00,A,3101.40475,N,12126.87095,E,0.760,,160114,,,A*74");
	DebugOut(0)<<"lon: "<<location->longitude()->toString()<<endl;
	DebugOut(0)<<"lat: "<<location->latitude()->toString()<<endl;

	///test gpgsa
	location->parse("GPGSA,A,3,04,05,,09,12,,,24,,,,,2.5,1.3,2.1*39");

	//Test incomplete message:
	location->parse("GPRMC,023633.00,V,,,,,,,180314,,,N*75");
	DebugOut(0)<<"lon: "<<location->longitude()->toString()<<endl;
	DebugOut(0)<<"lat: "<<location->latitude()->toString()<<endl;

	std::string testChecksuming = "GPRMC,195617.00,V,,,,,,,310314,,,N*74";

	g_assert(checksum(testChecksuming));

	std::string multimessage1 = "GA,235320.00,4532.48633,N,12257.";
	std::string multimessage2 = "57383,W,";
	std::string multimessage3 = "1,03,7.53,51.6,M,-21.3,M,,*55";
	std::string multimessage4 = "GPGSA,A,";
	std::string multimessage5 = "2,27,23,19,,,,,,,,,,7.60";
	std::string multimessage6 = ",7.53,1.00*";
	std::string multimessage7 = "0E";

	bool multimessageParse = false;

	multimessageParse |= tryParse(multimessage1);
	multimessageParse |= tryParse(multimessage2);
	multimessageParse |= tryParse(multimessage3);
	multimessageParse |= tryParse(multimessage4);
	multimessageParse |= tryParse(multimessage5);
	multimessageParse |= tryParse(multimessage6);
	multimessageParse |= tryParse(multimessage7);

	g_assert(multimessageParse);

	//Test meaningingless message:
	location->parse("GPRMC,,V,,,,,,,,,,N*53");

	//test false message:

	g_assert(!checksum("GPRMC,172758.296,V"));
}

bool GpsNmeaSource::tryParse(string data)
{
	std::vector<std::string> lines;

	boost::split(lines, data, boost::is_any_of("$"));

	bool weFoundAMessage = false;

	for(auto line : lines)
	{
		if(checksum(line))
		{
			buffer = line;
		}
		else
		{
			buffer += line;
		}

		std::string::size_type pos = buffer.find('G');

		if(pos != std::string::npos && pos != 0)
		{
			///Throw the incomplete stuff away.  if it doesn't begin with "G" it'll never be complete
			buffer = buffer.substr(pos);
		}

		if(checksum(buffer))
		{
			/// we have a complete message.  parse it!
			DebugOut(7)<<"Complete message: "<<buffer<<endl;
			location->parse(buffer);
			boost::algorithm::erase_all(buffer, "\n");
			boost::algorithm::erase_all(buffer, "\r");
			setValue(rawNmea, buffer);
			weFoundAMessage = true;
			buffer = "";
		}
		else
		{
			if(pos == 0 )
			{
				std::string::size_type cs = buffer.find('*');
				if (cs != std::string::npos && cs != buffer.length()-1)
				{
					buffer = buffer.substr(cs+(buffer.length() - cs));
				}
			}
		}

		DebugOut(7)<<"buffer: "<<buffer<<endl;
	}

	return weFoundAMessage;
}

bool GpsNmeaSource::checksum(std::string sentence)
{
	if(sentence.empty() || sentence.length() < 4 || sentence.find("*") == string::npos || sentence.find("*") >= sentence.length()-2)
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

	std::string sentenceCheckStr = sentence.substr(sentence.find('*')+1,2);

	try
	{
		int sentenceCheck = lexical_cast<int>(sentenceCheckStr);

		return sentenceCheck == checksum;
	}
	catch(...)

	{
		return false;
	}

	return false;
}
