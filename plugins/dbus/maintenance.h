#ifndef _MAINTENANCE_H_
#define _MAINTENANCE_H_


#include "dbusplugin.h"
#include "abstractdbusinterface.h"
#include "abstractroutingengine.h"

/** @interface Odometer : VehiclePropertyType **/
class OdometerProperty: public DBusSink
{
public:
	OdometerProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Odometer", re, connection, map<string, string>())
	{
		/** @attributeName Odometer
		 *  @type unsigned long
		 *  @access readonly
		 *  @attributeComment \brief MUST return Distance traveled in km
		 **/
		wantPropertyVariant(VehicleProperty::Odometer, "Odometer", AbstractProperty::Read);

		wantPropertyVariant(VehicleProperty::DistanceTotal, "DistanceTotal", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::DistanceSinceStart, "DistanceSinceStart", AbstractProperty::Read);
	}
};

/** @interface Fluid : VehiclePropertyType **/

class FluidProperty : public DBusSink
{
public:
	FluidProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Fluid", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::TransmissionFluidLevel, "Transmission", AbstractProperty::Read);
	}
};

/** @interface Battery : VehiclePropertyType **/
class BatteryStatusProperty: public DBusSink
{
public:
	BatteryStatusProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("BatteryStatus", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::BatteryVoltage, "Voltage", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::BatteryCurrent, "Current", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::BatteryChargeLevel, "ChargeLevel", AbstractProperty::Read);
	}
};

/** @interface TirePressure : VehiclePropertyType **/
class TireProperty: public DBusSink
{
public:
	TireProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Tire", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::TirePressure, "Pressure", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::TirePressureLow, "PressureLow", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::TireTemperature, "Temperature", AbstractProperty::Read);
	}
};


class EngineCoolant: public DBusSink
{
public:
	EngineCoolant(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("EngineCoolant", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::EngineCoolantLevel, "Level", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::EngineCoolantTemperature, "Temperature", AbstractProperty::Read);
	}
};

class TransmissionOil: public DBusSink
{
public:
	TransmissionOil(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("TransmissionOil", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::TransmissionOilWear, "Wear", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::TransmissionOilTemperature, "Temperature", AbstractProperty::Read);
	}
};

class TransmissionClutch: public DBusSink
{
public:
	TransmissionClutch(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("TransmissionClutch", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::TransmissionClutchWear, "Wear", AbstractProperty::Read);
	}
};

class BrakeMaintenance: public DBusSink
{
public:
	BrakeMaintenance(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("BrakeMaintenance", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::BrakePadWear, "PadWear", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::BrakeFluidLevel, "FluidLevel", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::BrakeFluidLevelLow, "FluidLevelLow", AbstractProperty::Read);
	}
};

class WasherFluid: public DBusSink
{
public:
	WasherFluid(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("WasherFluid", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::WasherFluidLevel, "Level", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::WasherFluidLevelLow, "LevelLow", AbstractProperty::Read);
	}
};

class MalfunctionIndicator: public DBusSink
{
public:
	MalfunctionIndicator(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("MalfunctionIndicator", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::MalfunctionIndicatorOn, "On", AbstractProperty::Read);
	}
};

class Diagnostics: public DBusSink
{
public:
	Diagnostics(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Diagnostics", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::AccumulatedEngineRuntime, "AccumulatedEngineRuntime", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::DistanceWithMILOn, "DistanceWithMILOn", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::DistanceSinceCodeCleared, "DistanceSinceCodeCleared", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::TimeRunMILOn, "TimeRunMILOn", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::TimeTroubleCodeClear, "TimeTroubleCodeClear", AbstractProperty::Read);
	}
};

#endif
