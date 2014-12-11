#ifndef _PARKING_H_
#define _PARKING_H_


#include "dbusplugin.h"
#include "abstractdbusinterface.h"
#include "abstractroutingengine.h"

///TODO Deprecated in 0.14.  Use AlarmStatus
class SecurityAlertProperty: public DBusSink
{
public:
	SecurityAlertProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("SecurityAlert", re, connection, map<string, string>())
	{
		wantProperty<Security::Status>(VehicleProperty::SecurityAlertStatus,"SecurityAlert", "i", AbstractProperty::Read);
	}
};

/** @interface ParkingBrake : VehiclePropertyType **/
class ParkingBrakeProperty: public DBusSink
{
public:
	ParkingBrakeProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("ParkingBrake", re, connection, map<string, string>())
	{
		/** @attributeName ParkingBrake
		 *  @type boolean
		 *  @access readonly
		 *  @attributeComment must return status of parking brake:  Engaged = true, Disengaged = false.
		 **/
		wantProperty<bool>(VehicleProperty::ParkingBrakeStatus,"ParkingBrake", "b", AbstractProperty::Read);

		wantPropertyVariant(VehicleProperty::ParkingBrakeStatusW3C,"Status", AbstractProperty::Read);


	}
};

/** @interface ParkingLight : VehiclePropertyType **/
///TODO: Deprecated in 0.14
class ParkingLightProperty: public DBusSink
{
public:
	ParkingLightProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("ParkingLight", re, connection, map<string, string>())
	{
		/** @attributeName ParkingLight
		 *  @type boolean
		 *  @access readonly
		 *  @attributeComment must return status of parking light:  Engaged = true, Disengaged = false.
		 **/
		wantProperty<bool>(VehicleProperty::ParkingLightStatus,"ParkingLight", "b", AbstractProperty::Read);
	}
};

/** @interface HazardLight : VehiclePropertyType **/
class HazardLightProperty: public DBusSink
{
public:
	HazardLightProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("HazardLight", re, connection, map<string, string>())
	{
		/** @attributeName HazardLight
		 *  @type boolean
		 *  @access readonly
		 *  @attributeComment must return status of hazard light:  Engaged = true, Disengaged = false.
		 **/
		wantProperty<bool>(VehicleProperty::HazardLightStatus,"HazardLight", "b", AbstractProperty::ReadWrite);
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
