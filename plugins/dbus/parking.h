#ifndef _PARKING_H_
#define _PARKING_H_


#include "dbusplugin.h"
#include "abstractdbusinterface.h"
#include "abstractroutingengine.h"

class SecurityAlertProperty: public DBusSink
{
public:
	SecurityAlertProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.securityAlert","/org/automotive/parking/securityAlert", re, connection, map<string, string>())
	{
		wantProperty<bool>(VehicleProperty::SecurityAlertStatus,"SecurityAlert", "b", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};


class ParkingBrakeProperty: public DBusSink
{
public:
	ParkingBrakeProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.parkingBrake","/org/automotive/parking/parkingBrake", re, connection, map<string, string>())
	{
		wantProperty<bool>(VehicleProperty::ParkingBrakeStatus,"ParkingBrake", "b", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

class ParkingLightProperty: public DBusSink
{
public:
	ParkingLightProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.parkingLight","/org/automotive/parking/parkingLight", re, connection, map<string, string>())
	{
		wantProperty<bool>(VehicleProperty::ParkingLightStatus,"ParkingLight", "b", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};


class HazardLightProperty: public DBusSink
{
public:
	HazardLightProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.parkingLight","/org/automotive/parking/parkingLight", re, connection, map<string, string>())
	{
		wantProperty<bool>(VehicleProperty::HazardLightStatus,"HazardLight", "b", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

#endif
