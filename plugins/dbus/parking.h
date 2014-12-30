#ifndef _PARKING_H_
#define _PARKING_H_


#include "dbusplugin.h"
#include "abstractdbusinterface.h"
#include "abstractroutingengine.h"

/** @interface ParkingBrake : VehiclePropertyType **/
class ParkingBrakeProperty: public DBusSink
{
public:
	ParkingBrakeProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("ParkingBrake", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::ParkingBrakeStatusW3C, "Status", AbstractProperty::Read);
	}
};

/** @interface HazardLight : VehiclePropertyType **/
class HazardLightProperty: public DBusSink
{
public:
	HazardLightProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("HazardLight", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::HazardLightStatus, "HazardLight", AbstractProperty::ReadWrite);
	}
};

class LaneDepartureStatus: public DBusSink
{
public:
	LaneDepartureStatus(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("LaneDepartureDetection", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::LaneDepartureStatus, "Status", AbstractProperty::Read);
	}
};

class AlarmStatus: public DBusSink
{
public:
	AlarmStatus(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Alarm", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::AlarmStatus, "Status", AbstractProperty::ReadWrite);
	}
};

#endif
