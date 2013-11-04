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
		:DBusSink("SecurityAlert", re, connection, map<string, string>())
	{

		/** @attributeName SecurityAlert
		 *  @type boolean
		 *  @access readonly
		 *  @attributeComment \brief MUST return
		 **/
		wantProperty<Security::Status>(VehicleProperty::SecurityAlertStatus,"SecurityAlert", "i", AbstractProperty::Read);

		
	}
};

/** @interface ParkingBrake : VehiclePropertyType **/
class ParkingBrakeProperty: public DBusSink
{
public:
	ParkingBrakeProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("ParkingBrake", re, connection, map<string, string>())
	{
		/** @attributeName ParkingBrake
		 *  @type boolean
		 *  @access readonly
		 *  @attributeComment must return status of parking brake:  Engaged = true, Disengaged = false.
		 **/
		wantProperty<bool>(VehicleProperty::ParkingBrakeStatus,"ParkingBrake", "b", AbstractProperty::Read);

		
	}
};

/** @interface ParkingLight : VehiclePropertyType **/
class ParkingLightProperty: public DBusSink
{
public:
	ParkingLightProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
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
	HazardLightProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
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

#endif
