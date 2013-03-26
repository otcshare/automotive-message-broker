#ifndef _PARKING_H_
#define _PARKING_H_


#include "dbusplugin.h"
#include "abstractdbusinterface.h"
#include "abstractroutingengine.h"

/** @interface SecurityAlert : VehiclePropertyType **/
class SecurityAlertProperty: public DBusSink
{
public:
	SecurityAlertProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.SecurityAlert","/org/automotive/parking/SecurityAlert", re, connection, map<string, string>())
	{

		/** @attributeName SecurityAlert
		 *  @type boolean
		 *  @access readonly
		 **/
		wantProperty<Security::Status>(VehicleProperty::SecurityAlertStatus,"SecurityAlert", "i", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

/** @interface ParkingBrake : VehiclePropertyType **/
class ParkingBrakeProperty: public DBusSink
{
public:
	ParkingBrakeProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.ParkingBrake","/org/automotive/parking/ParkingBrake", re, connection, map<string, string>())
	{
		/** @attributeName ParkingBrake
		 *  @type boolean
		 *  @access readonly
		 **/
		wantProperty<bool>(VehicleProperty::ParkingBrakeStatus,"ParkingBrake", "b", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

/** @interface ParkingLight : VehiclePropertyType **/
class ParkingLightProperty: public DBusSink
{
public:
	ParkingLightProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.ParkingLight","/org/automotive/parking/ParkingLight", re, connection, map<string, string>())
	{
		/** @attributeName ParkingLight
		 *  @type boolean
		 *  @access readonly
		 **/
		wantProperty<bool>(VehicleProperty::ParkingLightStatus,"ParkingLight", "b", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

/** @interface HazardLight : VehiclePropertyType **/
class HazardLightProperty: public DBusSink
{
public:
	HazardLightProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.HazardLight","/org/automotive/parking/HazardLight", re, connection, map<string, string>())
	{
		/** @attributeName HazardLight
		 *  @type boolean
		 *  @access readonly
		 **/
		wantProperty<bool>(VehicleProperty::HazardLightStatus,"HazardLight", "b", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

#endif
