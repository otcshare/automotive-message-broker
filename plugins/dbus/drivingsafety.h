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
		wantPropertyVariant(VehicleProperty::AntilockBrakingSystem, "AntilockBrakingSystem", VariantType::Read);
		wantPropertyVariant(VehicleProperty::AntilockBrakingSystem, "Engaged", VariantType::Read);
		wantPropertyVariant(VehicleProperty::AntilockBrakingSystemEnabled, "Enabled", VariantType::Read);
	}
};

class TractionControlSystemProperty: public DBusSink
{
public:
	TractionControlSystemProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("TractionControlSystem", re, connection, map<string, string>())
	{
		/// TODO: Deprecated.  Remove in 0.14
		wantPropertyVariant(VehicleProperty::TractionControlSystem, "TractionControlSystem", VariantType::Read);
		wantPropertyVariant(VehicleProperty::TractionControlSystemEnabled, "Enabled", VariantType::Read);
		wantPropertyVariant(VehicleProperty::TractionControlSystem, "Engaged", VariantType::Read);
	}
};


class VehicleTopSpeedLimitProperty: public DBusSink
{
public:
	VehicleTopSpeedLimitProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("VehicleTopSpeedLimit", re, connection, map<string, string>())
	{
		/// TODO: Deprecated.  Remove in 0.14
		wantPropertyVariant(VehicleProperty::VehicleTopSpeedLimit, "VehicleTopSpeedLimit", VariantType::Read);
		wantPropertyVariant(VehicleProperty::VehicleTopSpeedLimit, "Speed", VariantType::Read);
	}
};

class AirbagStatusProperty: public DBusSink
{
public:
	AirbagStatusProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("AirbagStatus", re, connection, map<string, string>())
	{
		/// TODO: Deprecated in 0.14
		wantPropertyVariant(VehicleProperty::AirbagStatus, "AirbagStatus", VariantType::Read);
		wantPropertyVariant(VehicleProperty::AirbagActivated, "AirbagActivated", VariantType::Read);
		wantPropertyVariant(VehicleProperty::AirbagDeployed, "AirbagDeployed", VariantType::Read);
	}
};

/// TODO: deprecated. remove in 0.14
class DoorStatusProperty: public DBusSink
{
public:
	DoorStatusProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("DoorStatus", re, connection, map<string, string>())
	{

		wantPropertyVariant(VehicleProperty::DoorStatus, "DoorStatus", VariantType::Read);

		wantPropertyVariant(VehicleProperty::DoorLockStatus, "DoorLockStatus", VariantType::Read);

		///TODO: Deprecated in 0.14
		wantPropertyVariant(VehicleProperty::ChildLockStatus, "ChildLockStatus", VariantType::Read);
	}
};

class DoorProperty: public DBusSink
{
public:
	DoorProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Door", re, connection, map<string, string>())
	{

		wantPropertyVariant(VehicleProperty::DoorStatusW3C, "Status", VariantType::Read);

		wantPropertyVariant(VehicleProperty::DoorLockStatus, "Lock", VariantType::ReadWrite);

		///TODO: Deprecated in 0.14
		wantPropertyVariant(VehicleProperty::ChildLockStatus, "ChildLock", VariantType::Read);
	}
};

/// TODO: Deprecated in 0.14
class SeatBeltStatusProperty: public DBusSink
{
public:
	SeatBeltStatusProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("SeatBelt", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::SeatBeltStatus, "Status", VariantType::Read);
	}
};

/// TODO: Deprecated in 0.14
class OccupantStatusProperty: public DBusSink
{
public:
	OccupantStatusProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("OccupantStatus", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::OccupantStatus, "OccupantStatus", VariantType::Read);
	}
};

class ObstacleDistanceProperty: public DBusSink
{
public:
	ObstacleDistanceProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("ObstacleDistance", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::ObstacleDistance, "ObstacleDistance", VariantType::Read);
	}
};

class ElectronicStabilityControl: public DBusSink
{
public:
	ElectronicStabilityControl(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("ElectronicStabilityControl", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::ElectronicStabilityControlEnabled, "Enabled", VariantType::Read);
		wantPropertyVariant(VehicleProperty::ElectronicStabilityControlEngaged, "Engaged", VariantType::Read);
	}
};

class ChildSafetyLock: public DBusSink
{
public:
	ChildSafetyLock(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("ChildSafetyLock", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::ChildLockStatus, "Lock", VariantType::ReadWrite);
	}
};

class SeatProperty: public DBusSink
{
public:
	SeatProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Seat", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::OccupantStatusW3C, "Occupant", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatBeltStatus, "SeatBelt", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::OccupantName, "OccupantName", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::OccupantIdentificationType, "IdentificationType", VariantType::ReadWrite);
	}
};

#endif

