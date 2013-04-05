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
		:DBusSink("org.automotive.AntilockBrakingSystem","/org/automotive/drivingSafety/AntilockBrakingSystem", re, connection, map<string, string>())
	{

		/** @attributeName AntilockBrakingSystem
		 *  @type boolean
		 *  @access readonly
		 *  @attributeComment \brief MUST return whether Antilock Braking System is Idle (false) or Engaged (true)
		 **/
		wantPropertyVariant(VehicleProperty::AntilockBrakingSystem, "AntilockBrakingSystem", "b", AbstractProperty::Read);


		supportedChanged(re->supported());
	}
};

/** @interface TractionControlSystem : VehiclePropertyType **/
class TractionControlSystemProperty: public DBusSink
{
public:
	TractionControlSystemProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.TractionControlSystem","/org/automotive/drivingSafety/TractionControlSystem", re, connection, map<string, string>())
	{

		/** @attributeName TractionControlSystem
		 *  @type boolean
		 *  @access readonly
		 *  @attributeComment \brief MUST return whether Traction Control System is Off (false) or On (true)
		 **/
		wantPropertyVariant(VehicleProperty::TractionControlSystem, "TractionControlSystem", "b", AbstractProperty::Read);


		supportedChanged(re->supported());
	}
};


/** @interface VehicleTopSpeedLimit : VehiclePropertyType **/
class VehicleTopSpeedLimitProperty: public DBusSink
{
public:
	VehicleTopSpeedLimitProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.VehicleTopSpeedLimit","/org/automotive/drivingSafety/VehicleTopSpeedLimit", re, connection, map<string, string>())
	{

		/** @attributeName VehicleTopSpeedLimit
		 *  @type unsigned short
		 *  @access readonly
		 *  @attributeComment \brief MUST returns top rated speed in km/h. 0 = no limit
		 **/
		wantPropertyVariant(VehicleProperty::VehicleTopSpeedLimit, "VehicleTopSpeedLimit", "q", AbstractProperty::Read);


		supportedChanged(re->supported());
	}
};

/** @interface AirbagStatus : VehiclePropertyType **/
class AirbagStatusProperty: public DBusSink
{
public:
	AirbagStatusProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.AirbagStatus","/org/automotive/drivingSafety/AirbagStatus", re, connection, map<string, string>())
	{

		/**
		 * @enum const unsigned short AIRBAGLOCATION_DRIVER = 0;
		 * @enum const unsigned short AIRBAGLOCATION_PASSENGER= 1;
		 * @enum const unsigned short AIRBAGLOCATION_LEFTSIDE = 2;
		 * @enum const unsigned short AIRBAGLOCATION_RIGHTSIDE = 3;
		 * @enum const unsigned short AIRBAGSTATUS_INACTIVE = 0;
		 * @enum const unsigned short AIRBAGSTATUS_ACTIVE = 1;
		 * @enum const unsigned short AIRBAGSTATUS_DEPLOYED = 2;
		 **/

		/** @attributeName AirbagStatus
		 *  @type object
		 *  @access readonly
		 *  @attributeComment \brief MUST returns a dictionary of Airbag (byte) and Status (byte) (see AIRBAGLOCATION_* and AIRBAGSTATUS_*)
		 **/
		wantPropertyVariant(VehicleProperty::AirbagStatus, "AirbagStatus", "a{yy}", AbstractProperty::Read);


		supportedChanged(re->supported());
	}
};

/** @interface DoorStatus : VehiclePropertyType **/
class DoorStatusProperty: public DBusSink
{
public:
	DoorStatusProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.DoorStatus","/org/automotive/drivingSafety/DoorStatus", re, connection, map<string, string>())
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
		wantPropertyVariant(VehicleProperty::DoorStatus, "DoorStatus", "a(yy)", AbstractProperty::Read);

		/** @attributeName DoorLockStatus
		 *  @type object
		 *  @access readonly
		 *  @attributeComment \brief MUST returns dictionary of Door (byte) and Status (bool locked = true, unlocked = false).  See DOORLOCATION_*.
		 **/
		wantPropertyVariant(VehicleProperty::DoorLockStatus, "DoorLockStatus", "a(yb)", AbstractProperty::Read);

		/** @attributeName ChildLockStatus
		 *  @type boolean
		 *  @access readonly
		 *  @attributeComment \brief MUST returns Child lock status of rear doors.  active = true, inactive = false.
		 *  @attributeComment Setting this to 'true' will prevent the rear doors from being opened
		 *  @attributeComment from the inside.
		 **/
		wantPropertyVariant(VehicleProperty::DoorLockStatus, "ChildLockStatus", "b", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

/** @interface SeatBeltStatus : VehiclePropertyType **/
class SeatBeltStatusProperty: public DBusSink
{
public:
	SeatBeltStatusProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.SeatBeltStatus","/org/automotive/drivingSafety/SeatBeltStatus", re, connection, map<string, string>())
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
		 *  @attributeComment \brief MUST returns dictionary of Seat Belt (byte, see SEATBELTLOCATION_*) and Status (bool: Fasten = true, Unfastened = false)
		 **/
		wantPropertyVariant(VehicleProperty::SeatBeltStatus, "SeatBeltStatus", "a(yb)", AbstractProperty::Read);


		supportedChanged(re->supported());
	}
};

/** @interface OccupantStatus : VehiclePropertyType **/
class OccupantStatusProperty: public DBusSink
{
public:
	OccupantStatusProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.OccupantStatus","/org/automotive/drivingSafety/OccupantStatus", re, connection, map<string, string>())
	{


		/**
		 * @enum const unsigned short OCCUPANTLOCATION_DRIVER = 0;
		 * @enum const unsigned short OCCUPANTLOCATION_PASSENGER= 1;
		 * @enum const unsigned short OCCUPANTLOCATION_LEFTSIDE = 2;
		 * @enum const unsigned short OCCUPANTLOCATION_RIGHTSIDE = 3;
		 * @enum const unsigned short OCCUPANTSTATUS_VACANT = 0;
		 * @enum const unsigned short OCCUPANTSTATUS_CHILD = 1;
		 * @enum const unsigned short OCCUPANTSTATUS_ADULT = 2;
		 **/

		/** @attributeName OccupantStatus
		 *  @type object
		 *  @access readonly
		 *  @attributeComment \brief MUST returns dictionary of Occupant (byte see OCCUPANTLOCATION_*) and Status (byte, see OCCUPANTSTATUS_*)
		 **/
		wantPropertyVariant(VehicleProperty::OccupantStatus, "OccupantStatus", "a(yy)", AbstractProperty::Read);


		supportedChanged(re->supported());
	}
};

/** @interface ObstacleDistance : VehiclePropertyType **/
class ObstacleDistanceProperty: public DBusSink
{
public:
	ObstacleDistanceProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.ObstacleDistance","/org/automotive/drivingSafety/ObstacleDistance", re, connection, map<string, string>())
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
		 *  @attributeComment \brief MUST returns dictionary of Distance Sensor (byte, see DISTANCESENSORLOCATION_*) and distance (double) in m.
		 **/
		wantPropertyVariant(VehicleProperty::ObstacleDistance, "ObstacleDistance", "a{yd}", AbstractProperty::Read);


		supportedChanged(re->supported());
	}
};

#endif

