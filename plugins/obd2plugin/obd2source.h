
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


#ifndef OBD2SOURCE_H
#define OBD2SOURCE_H



#include <abstractsource.h>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include "obdlib.h"
#include <glib.h>

#include "obdpid.h"

class ObdRequest
{
public:
  VehicleProperty::Property property;
  std::string req;
  std::string arg;
};


class CommandRequest
{
public:
  std::string req;
  std::vector<std::string> arglist;
};

class ObdReply
{
public:
  VehicleProperty::Property property;
  std::string req;
  std::string reply;
};



class Obd2Amb
{
public:

	typedef function<std::string (std::string)> ConversionFunction;

	typedef std::vector<unsigned char> ByteArray;
	Obd2Amb()
	{
<<<<<<< HEAD
		propertyPidMap[VehicleProperty::VehicleSpeed] = "010D1\r";
		propertyPidMap[VehicleProperty::EngineSpeed] = "010C1\r";
		propertyPidMap[VehicleProperty::MassAirFlow] = "01101\r";
		propertyPidMap[VehicleProperty::AirIntakeTemperature] = "010F1\r";
		propertyPidMap[VehicleProperty::ThrottlePosition] = "01111\r";
		propertyPidMap[VehicleProperty::BatteryVoltage] = "ATRV\r";
		propertyPidMap[VehicleProperty::EngineCoolantTemperature]  = "01051\r";
		propertyPidMap[VehicleProperty::BatteryVoltage] = "ATRV\r";
		propertyPidMap[VehicleProperty::EngineLoad] = "01041/r";
		propertyPidMap[VehicleProperty::VIN] = "0902/r";
		propertyPidMap[VehicleProperty::WMI] = "0902/r";
		propertyPidMap[VehicleProperty::EngineOilTemperature] = "015C1\r";
		propertyPidMap[VehicleProperty::InteriorTemperature] = "01461\r";
		propertyPidMap[VehicleProperty::FuelConsumption] = "01101\r";


		propertyConversionMap[VehicleProperty::VehicleSpeed] = [](std::string input)
		{
			///velocity is used in other equations.  We'll save it off in a static variable:
			stringstream vssConvert(input);

			vssConvert>>velocity;

			return input;
		};
=======
		supportedPidsList.push_back(new VehicleSpeedPid());
		supportedPidsList.push_back(new EngineSpeedPid());
		supportedPidsList.push_back(new MassAirFlowPid());
		supportedPidsList.push_back(new VinPid());
		supportedPidsList.push_back(new WmiPid());
		supportedPidsList.push_back(new FuelConsumptionPid());
		supportedPidsList.push_back(new EngineCoolantPid());
		supportedPidsList.push_back(new AirIntakeTemperaturePid());
	}
>>>>>>> release

	~Obd2Amb()
	{
		for(auto itr = supportedPidsList.begin(); itr != supportedPidsList.end(); itr++)
		{
			delete *itr;
		}
	}

	ObdPid* createPidFromReply(ByteArray replyVector)
	{
		for(auto itr = supportedPidsList.begin(); itr != supportedPidsList.end(); itr++)
		{
			if (!(*itr)->tryParse(replyVector))
			{
				continue;
			}
			
			ObdPid* pid = (*itr)->create();
			pid->tryParse(replyVector);
			return pid;
		}
		return 0;
	}
	ObdPid* createPidforProperty(VehicleProperty::Property property)
	{
		for(auto itr = supportedPidsList.begin(); itr != supportedPidsList.end(); itr++)
		{
			VehicleProperty::Property p = (*itr)->property;
			if(p == property)
			{
				ObdPid* obj = *itr;
				return obj->create();
			}
		}
		return NULL;
	}

<<<<<<< HEAD


	map<VehicleProperty::Property, ConversionFunction> propertyConversionMap;
	map<VehicleProperty::Property, std::string> propertyPidMap;

private:

	static uint16_t velocity;
	static double fuelConsumptionOldTime;
=======
	std::list<ObdPid*> supportedPidsList;
>>>>>>> release
};

class OBD2Source : public AbstractSource
{

public:
	OBD2Source(AbstractRoutingEngine* re, map<string, string> config);
	~OBD2Source();
	string uuid();
	int portHandle;
	void getPropertyAsync(AsyncPropertyReply *reply);
	void getRangePropertyAsync(AsyncRangePropertyReply *reply){}
	AsyncPropertyReply * setProperty(AsyncSetPropertyRequest request);
	void subscribeToPropertyChanges(VehicleProperty::Property property);
	void unsubscribeToPropertyChanges(VehicleProperty::Property property);
	PropertyList supported();

	int supportedOperations();

	PropertyList queuedRequests;
	bool clientConnected;
	PropertyList activeRequests;
	void engineSpeed(double speed);
	void vehicleSpeed(int speed);
	void mafValue(double maf);
	void engineCoolantTemp(int temp);
	PropertyList removeRequests;
	void setSupported(PropertyList list);
	void propertyChanged(VehicleProperty::Property property, AbstractPropertyType* value, string uuid) {}
	void supportedChanged(PropertyList) {}
	GAsyncQueue* commandQueue;
	GAsyncQueue* subscriptionAddQueue;
	GAsyncQueue* subscriptionRemoveQueue;
	GAsyncQueue* singleShotQueue;
	GAsyncQueue* responseQueue;
	std::list<std::string> m_blacklistPidList;
	std::map<std::string,int> m_blacklistPidCountMap;
	void setConfiguration(map<string, string> config);
	//void randomizeProperties();
	std::string m_port;
	std::string m_baud;
	bool m_isBluetooth;
	std::string m_btDeviceAddress;
	std::string m_btAdapterAddress;
	map<VehicleProperty::Property,AsyncPropertyReply*> propertyReplyMap;
	void updateProperty(VehicleProperty::Property property,AbstractPropertyType *value);
	obdLib * obd;
	bool m_threadLive;
	GThread *m_gThread;

private:
	PropertyList m_supportedProperties;
	GMutex *threadQueueMutex;
	VehicleProperty::Property Obd2Connect;
	typedef BasicPropertyType<bool> Obd2ConnectType;

};

#endif // OBD2SOURCE_H
