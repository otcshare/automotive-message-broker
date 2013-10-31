#ifndef DRIVINGSAFETY_H_
#define DRIVINGSAFETY_H_

#include "dbusplugin.h"
#include "abstractdbusinterface.h"
#include "abstractroutingengine.h"

/** @interface AntilockBrakingSystem : VehiclePropertyType **/
class AntilockBrakingSystemProperty: public DBusSink
{
public:
	AntilockBrakingSystemProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("AntilockBrakingSystem", re, connection, map<string, string>())
	{

		/** @attributeName AntilockBrakingSystem
		 *  @type boolean
		 *  @access readonly
		 *  @attributeComment \brief MUST return whether Antilock Braking System is Idle (false) or Engaged (true)
		 **/
		wantPropertyVariant(VehicleProperty::AntilockBrakingSystem, "AntilockBrakingSystem", "b", AbstractProperty::Read);


		
	}
};

/** @interface TractionControlSystem : VehiclePropertyType **/
class TractionControlSystemProperty: public DBusSink
{
public:
	TractionControlSystemProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("TractionControlSystem", re, connection, map<string, string>())
	{

		/** @attributeName TractionControlSystem
		 *  @type boolean
		 *  @access readonly
		 *  @attributeComment \brief MUST return whether Traction Control System is Off (false) or On (true)
		 **/
		wantPropertyVariant(VehicleProperty::TractionControlSystem, "TractionControlSystem", "b", AbstractProperty::Read);


		
	}
};


/** @interface VehicleTopSpeedLimit : VehiclePropertyType **/
class VehicleTopSpeedLimitProperty: public DBusSink
{
public:
	VehicleTopSpeedLimitProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("VehicleTopSpeedLimit", re, connection, map<string, string>())
	{

		/** @attributeName VehicleTopSpeedLimit
		 *  @type unsigned short
		 *  @access readonly
		 *  @attributeComment \brief MUST returns top rated speed in km/h. 0 = no limit
		 **/
		wantPropertyVariant(VehicleProperty::VehicleTopSpeedLimit, "VehicleTopSpeedLimit", "q", AbstractProperty::Read);


		
	}
};

/** @interface AirbagStatus : VehiclePropertyType **/
class AirbagStatusProperty: public DBusSink
{
public:
	AirbagStatusProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("AirbagStatus", re, connection, map<string, string>())
	{

		/**
		 * @enum const unsigned short AIRBAGSTATUS_INACTIVE = 0;
		 * @enum const unsigned short AIRBAGSTATUS_ACTIVE = 1;
		 * @enum const unsigned short AIRBAGSTATUS_DEPLOYED = 2;
		 **/

		/** @attributeName AirbagStatus
		 *  @type object
		 *  @access readonly
		 *  @attributeComment \brief MUST returns airbag status (byte) (see AIRBAGSTATUS_*)
		 **/
		wantPropertyVariant(VehicleProperty::AirbagStatus, "AirbagStatus", "y", AbstractProperty::Read);


		//
	}
};

/** @interface DoorStatus : VehiclePropertyType **/
class DoorStatusProperty: public DBusSink
{
public:
	DoorStatusProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("DoorStatus", re, connection, map<string, string>())
	{
		/**
		 * @enum const unsigned short DOORLOCATION_DRIVER= 0;
		 * @enum const unsigned short DOORLOCATION_PASSENGER = 1;
		 * @enum const unsigned short DOORLOCATION_LEFTREAR = 2;
		 * @enum const unsigned short DOORLOCATION_RIGHTREAR = 3;
		 * @enum const unsigned short DOORLOCATION_TRUNK = 4;
		 * @enum const unsigned short DOORLOCATION_FUELCAP = 5;
		 * @enum const unsigned short DOORLOCATION_HOOD = 6;
		 * @enum const unsigned short DOORSTATUS_CLOSED = 0;
		 * @enum const unsigned short DOORSTATUS_OPEN = 1;
		 * @enum const unsigned short DOORSTATUS_AJAR = 2;
		 **/


		/** @attributeName DoorStatus
		 *  @type object
		 *  @access readonly
		 *  @attributeComment \brief MUST returns dictionary of Door (byte) and Status (byte).  See DOORLOCATION_* and DOORSTATUS_*
		 **/
		wantPropertyVariant(VehicleProperty::DoorStatus, "DoorStatus", "b", AbstractProperty::Read);

		/** @attributeName DoorLockStatus
		 *  @type object
		 *  @access readonly
		 *  @attributeComment \brief MUST returns dictionary of Door (byte) and Status (bool locked = true, unlocked = false).  See DOORLOCATION_*.
		 **/
		wantPropertyVariant(VehicleProperty::DoorLockStatus, "DoorLockStatus", "b", AbstractProperty::Read);

		/** @attributeName ChildLockStatus
		 *  @type boolean
		 *  @access readonly
		 *  @attributeComment \brief MUST returns Child lock status of rear doors.  active = true, inactive = false.
		 *  @attributeComment Setting this to 'true' will prevent the rear doors from being opened
		 *  @attributeComment from the inside.
		 **/
		wantPropertyVariant(VehicleProperty::ChildLockStatus, "ChildLockStatus", "b", AbstractProperty::Read);

		
	}
};

/** @interface SeatBeltStatus : VehiclePropertyType **/
class SeatBeltStatusProperty: public DBusSink
{
public:
	SeatBeltStatusProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("SeatBeltStatus", re, connection, map<string, string>())
	{
		/**
		 * @enum const unsigned short SEATBELTLOCATION_DRIVER= 0;
		 * @enum const unsigned short SEATBELTLOCATION_MIDDLEFRONT = 1;
		 * @enum const unsigned short SEATBELTLOCATION_PASSENGER = 2;
		 * @enum const unsigned short SEATBELTLOCATION_LEFTREAR = 3;
		 * @enum const unsigned short SEATBELTLOCATION_MIDDLEREAR = 4;
		 * @enum const unsigned short SEATBELTLOCATION_RIGHTREAR = 5;
		 **/

		/** @attributeName SeatBeltStatus
		 *  @type object
		 *  @access readonly
		 *  @attributeComment \brief MUST returns dictionary of Seat Belt (byte, see SEATBELTLOCATION) and Status (bool: Fasten = true, Unfastened = false)
		 **/
		wantPropertyVariant(VehicleProperty::SeatBeltStatus, "SeatBeltStatus", "b", AbstractProperty::Read);


		
	}
};

/** @interface OccupantStatus : VehiclePropertyType **/
class OccupantStatusProperty: public DBusSink
{
public:
	OccupantStatusProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("OccupantStatus", re, connection, map<string, string>())
	{


		/**
		 * @enum const unsigned short OCCUPANTLOCATION_DRIVER = 0;
		 * @enum const unsigned short OCCUPANTLOCATION_FRONTMIDDLE = 1;
		 * @enum const unsigned short OCCUPANTLOCATION_PASSENGER= 2;
		 * @enum const unsigned short OCCUPANTLOCATION_LEFTREAR = 3;
		 * @enum const unsigned short OCCUPANTLOCATION_MIDDLEREAR = 4;
		 * @enum const unsigned short OCCUPANTLOCATION_RIGHTREAR = 5;
		 * @enum const unsigned short OCCUPANTSTATUS_VACANT = 0;
		 * @enum const unsigned short OCCUPANTSTATUS_CHILD = 1;
		 * @enum const unsigned short OCCUPANTSTATUS_ADULT = 2;
		 **/

		/** @attributeName OccupantStatus
		 *  @type object
		 *  @access readonly
		 *  @attributeComment \brief MUST returns dictionary of Occupant (byte see OCCUPANTLOCATION) and Status (byte, see OCCUPANTSTATUS_*)
		 **/
		wantPropertyVariant(VehicleProperty::OccupantStatus, "OccupantStatus", "i", AbstractProperty::Read);


		
	}
};

/** @interface ObstacleDistance : VehiclePropertyType **/
class ObstacleDistanceProperty: public DBusSink
{
public:
	ObstacleDistanceProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("ObstacleDistance", re, connection, map<string, string>())
	{
		/**
		 * @enum const unsigned short DISTANCESENSORLOCATION_LEFTFRONT = 0;
		 * @enum const unsigned short DISTANCESENSORLOCATION_RIGHTFRONT = 1;
		 * @enum const unsigned short DISTANCESENSORLOCATION_LEFTREAR = 2;
		 * @enum const unsigned short DISTANCESENSORLOCATION_RIGHTREAR = 3;
		 * @enum const unsigned short DISTANCESENSORLOCATION_LEFTBLINDSPOT = 4;
		 * @enum const unsigned short DISTANCESENSORLOCATION_RIGHTBLINDSPOT = 5;
		 **/

		/** @attributeName ObstacleDistance
		 *  @type object
		 *  @access readonly
		 *  @attributeComment \brief MUST returns dictionary of Distance Sensor (byte, see DISTANCESENSORLOCATION) and distance (double) in m.
		 **/
		wantPropertyVariant(VehicleProperty::ObstacleDistance, "ObstacleDistance", "d", AbstractProperty::Read);


		
	}
};

#endif

