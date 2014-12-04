#ifndef DRIVINGSAFETY_H_
#define DRIVINGSAFETY_H_

#include "dbusplugin.h"
#include "abstractdbusinterface.h"
#include "abstractroutingengine.h"

class AntilockBrakingSystemProperty: public DBusSink
{
public:
	AntilockBrakingSystemProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("AntilockBrakingSystem", re, connection, map<string, string>())
	{
		/// TODO: Deprecated.  Remove in 0.14
		wantPropertyVariant(VehicleProperty::AntilockBrakingSystem, "AntilockBrakingSystem", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::AntilockBrakingSystem, "Engaged", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::AntilockBrakingSystemEnabled, "Enabled", AbstractProperty::Read);
	}
};

class TractionControlSystemProperty: public DBusSink
{
public:
	TractionControlSystemProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("TractionControlSystem", re, connection, map<string, string>())
	{
		/// TODO: Deprecated.  Remove in 0.14
		wantPropertyVariant(VehicleProperty::TractionControlSystem, "TractionControlSystem", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::TractionControlSystemEnabled, "Enabled", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::TractionControlSystem, "Engaged", AbstractProperty::Read);
	}
};


class VehicleTopSpeedLimitProperty: public DBusSink
{
public:
	VehicleTopSpeedLimitProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("VehicleTopSpeedLimit", re, connection, map<string, string>())
	{
		/// TODO: Deprecated.  Remove in 0.14
		wantPropertyVariant(VehicleProperty::VehicleTopSpeedLimit, "VehicleTopSpeedLimit", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::VehicleTopSpeedLimit, "Speed", AbstractProperty::Read);
	}
};

class AirbagStatusProperty: public DBusSink
{
public:
	AirbagStatusProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("AirbagStatus", re, connection, map<string, string>())
	{
		/// TODO: Deprecated in 0.14
		wantPropertyVariant(VehicleProperty::AirbagStatus, "AirbagStatus", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::AirbagActivated, "AirbagActivated", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::AirbagDeployed, "AirbagDeployed", AbstractProperty::Read);
	}
};

/// TODO: deprecated. remove in 0.14
class DoorStatusProperty: public DBusSink
{
public:
	DoorStatusProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("DoorStatus", re, connection, map<string, string>())
	{

		wantPropertyVariant(VehicleProperty::DoorStatus, "DoorStatus", AbstractProperty::Read);

		wantPropertyVariant(VehicleProperty::DoorLockStatus, "DoorLockStatus", AbstractProperty::Read);

		///TODO: Deprecated in 0.14
		wantPropertyVariant(VehicleProperty::ChildLockStatus, "ChildLockStatus", AbstractProperty::Read);
	}
};

class DoorProperty: public DBusSink
{
public:
	DoorProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Door", re, connection, map<string, string>())
	{

		wantPropertyVariant(VehicleProperty::DoorStatusW3C, "Status", AbstractProperty::Read);

		wantPropertyVariant(VehicleProperty::DoorLockStatus, "Lock", AbstractProperty::ReadWrite);

		///TODO: Deprecated in 0.14
		wantPropertyVariant(VehicleProperty::ChildLockStatus, "ChildLock", AbstractProperty::Read);
	}
};

/// TODO: Deprecated in 0.14
class SeatBeltStatusProperty: public DBusSink
{
public:
	SeatBeltStatusProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("SeatBelt", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::SeatBeltStatus, "Status", AbstractProperty::Read);
	}
};

/// TODO: Deprecated in 0.14
class OccupantStatusProperty: public DBusSink
{
public:
	OccupantStatusProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("OccupantStatus", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::OccupantStatus, "OccupantStatus", AbstractProperty::Read);
	}
};

class ObstacleDistanceProperty: public DBusSink
{
public:
	ObstacleDistanceProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("ObstacleDistance", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::ObstacleDistance, "ObstacleDistance", AbstractProperty::Read);
	}
};

class ElectronicStabilityControl: public DBusSink
{
public:
	ElectronicStabilityControl(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("ElectronicStabilityControl", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::ElectronicStabilityControlEnabled, "Enabled", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::ElectronicStabilityControlEngaged, "Engaged", AbstractProperty::Read);
	}
};

class ChildSafetyLock: public DBusSink
{
public:
	ChildSafetyLock(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("ChildSafetyLock", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::ChildLockStatus, "Lock", AbstractProperty::ReadWrite);
	}
};

class SeatProperty: public DBusSink
{
public:
	SeatProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Seat", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::OccupantStatusW3C, "Occupant", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatBeltStatus, "SeatBelt", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::OccupantName, "OccupantName", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::OccupantIdentificationType, "IdentificationType", AbstractProperty::ReadWrite);
	}
};

#endif

