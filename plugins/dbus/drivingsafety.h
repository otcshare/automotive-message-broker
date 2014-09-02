#ifndef DRIVINGSAFETY_H_
#define DRIVINGSAFETY_H_

#include "dbusplugin.h"
#include "abstractdbusinterface.h"
#include "abstractroutingengine.h"

class AntilockBrakingSystemProperty: public DBusSink
{
public:
	AntilockBrakingSystemProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("AntilockBrakingSystem", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::AntilockBrakingSystem, "AntilockBrakingSystem", "b", AbstractProperty::Read);
	}
};

class TractionControlSystemProperty: public DBusSink
{
public:
	TractionControlSystemProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("TractionControlSystem", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::TractionControlSystem, "TractionControlSystem", "b", AbstractProperty::Read);
	}
};


class VehicleTopSpeedLimitProperty: public DBusSink
{
public:
	VehicleTopSpeedLimitProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("VehicleTopSpeedLimit", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::VehicleTopSpeedLimit, "VehicleTopSpeedLimit", "q", AbstractProperty::Read);
	}
};

class AirbagStatusProperty: public DBusSink
{
public:
	AirbagStatusProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("AirbagStatus", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::AirbagStatus, "AirbagStatus", "y", AbstractProperty::Read);
	}
};

/// TODO: deprecated. remove in 0.13
class DoorStatusProperty: public DBusSink
{
public:
	DoorStatusProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("DoorStatus", re, connection, map<string, string>())
	{

		wantPropertyVariant(VehicleProperty::DoorStatus, "DoorStatus", "b", AbstractProperty::Read);

		wantPropertyVariant(VehicleProperty::DoorLockStatus, "DoorLockStatus", "b", AbstractProperty::Read);

		wantPropertyVariant(VehicleProperty::ChildLockStatus, "ChildLockStatus", "b", AbstractProperty::Read);
	}
};

class DoorProperty: public DBusSink
{
public:
	DoorProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Door", re, connection, map<string, string>())
	{

		wantPropertyVariant(VehicleProperty::DoorStatusW3C, "Status", AbstractProperty::Read);

		wantPropertyVariant(VehicleProperty::DoorLockStatus, "Lock", AbstractProperty::Read);

		wantPropertyVariant(VehicleProperty::ChildLockStatus, "ChildLock", AbstractProperty::Read);
	}
};

class SeatBeltStatusProperty: public DBusSink
{
public:
	SeatBeltStatusProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("SeatBelt", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::SeatBeltStatus, "Status", "b", AbstractProperty::Read);
	}
};

class OccupantStatusProperty: public DBusSink
{
public:
	OccupantStatusProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("OccupantStatus", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::OccupantStatus, "OccupantStatus", "i", AbstractProperty::Read);
	}
};

class ObstacleDistanceProperty: public DBusSink
{
public:
	ObstacleDistanceProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("ObstacleDistance", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::ObstacleDistance, "ObstacleDistance", "d", AbstractProperty::Read);
	}
};

#endif

