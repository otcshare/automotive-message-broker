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
		wantPropertyVariant(VehicleProperty::Odometer, "Odometer", VariantType::Read);

		wantPropertyVariant(VehicleProperty::DistanceTotal, "DistanceTotal", VariantType::Read);
		wantPropertyVariant(VehicleProperty::DistanceSinceStart, "DistanceSinceStart", VariantType::Read);
	}
};

/** @interface Fluid : VehiclePropertyType **/

class FluidProperty : public DBusSink
{
public:
	FluidProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Fluid", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::TransmissionFluidLevel, "Transmission", VariantType::Read);
	}
};

/** @interface Battery : VehiclePropertyType **/
class BatteryStatusProperty: public DBusSink
{
public:
	BatteryStatusProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("BatteryStatus", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::BatteryVoltage, "Voltage", VariantType::Read);
		wantPropertyVariant(VehicleProperty::BatteryCurrent, "Current", VariantType::Read);
		wantPropertyVariant(VehicleProperty::BatteryChargeLevel, "ChargeLevel", VariantType::Read);
	}
};

/** @interface TirePressure : VehiclePropertyType **/
class TireProperty: public DBusSink
{
public:
	TireProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Tire", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::TirePressure, "Pressure", VariantType::Read);
		wantPropertyVariant(VehicleProperty::TirePressureLow, "PressureLow", VariantType::Read);
		wantPropertyVariant(VehicleProperty::TireTemperature, "Temperature", VariantType::Read);
	}
};


class EngineCoolant: public DBusSink
{
public:
	EngineCoolant(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("EngineCoolant", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::EngineCoolantLevel, "Level", VariantType::Read);
		wantPropertyVariant(VehicleProperty::EngineCoolantTemperature, "Temperature", VariantType::Read);
	}
};

class TransmissionOil: public DBusSink
{
public:
	TransmissionOil(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("TransmissionOil", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::TransmissionOilWear, "Wear", VariantType::Read);
		wantPropertyVariant(VehicleProperty::TransmissionOilTemperature, "Temperature", VariantType::Read);
	}
};

class TransmissionClutch: public DBusSink
{
public:
	TransmissionClutch(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("TransmissionClutch", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::TransmissionClutchWear, "Wear", VariantType::Read);
	}
};

class BrakeMaintenance: public DBusSink
{
public:
	BrakeMaintenance(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("BrakeMaintenance", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::BrakePadWear, "PadWear", VariantType::Read);
		wantPropertyVariant(VehicleProperty::BrakeFluidLevel, "FluidLevel", VariantType::Read);
		wantPropertyVariant(VehicleProperty::BrakeFluidLevelLow, "FluidLevelLow", VariantType::Read);
	}
};

class WasherFluid: public DBusSink
{
public:
	WasherFluid(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("WasherFluid", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::WasherFluidLevel, "Level", VariantType::Read);
		wantPropertyVariant(VehicleProperty::WasherFluidLevelLow, "LevelLow", VariantType::Read);
	}
};

class MalfunctionIndicator: public DBusSink
{
public:
	MalfunctionIndicator(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("MalfunctionIndicator", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::MalfunctionIndicatorOn, "On", VariantType::Read);
	}
};

class Diagnostics: public DBusSink
{
public:
	Diagnostics(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Diagnostic", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::AccumulatedEngineRuntime, "AccumulatedEngineRuntime", VariantType::Read);
		wantPropertyVariant(VehicleProperty::DistanceWithMILOn, "DistanceWithMILOn", VariantType::Read);
		wantPropertyVariant(VehicleProperty::DistanceSinceCodeCleared, "DistanceSinceCodeCleared", VariantType::Read);
		wantPropertyVariant(VehicleProperty::TimeRunMILOn, "TimeRunMILOn", VariantType::Read);
		wantPropertyVariant(VehicleProperty::TimeTroubleCodeClear, "TimeTroubleCodeClear", VariantType::Read);
	}
};

#endif
