#ifndef _MAINTENANCE_H_
#define _MAINTENANCE_H_


#include "dbusplugin.h"
#include "abstractdbusinterface.h"
#include "abstractroutingengine.h"

/** @interface Odometer : VehiclePropertyType **/
class OdometerProperty: public DBusSink
{
public:
	OdometerProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
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
	FluidProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Fluid", re, connection, map<string, string>())
	{
		/** @attributeName Transmission
		 *  @type unsigned short
		 *  @access readonly
		 *  @attributeComment \brief MUST return Transmission fluid level percentage. 0-100.
		 **/
		wantPropertyVariant(VehicleProperty::TransmissionFluidLevel, "Transmission", AbstractProperty::Read);

		/** @attributeName Brake
		 *  @type unsigned short
		 *  @access readonly
		 *  @attributeComment \brief MUST return Brake fluid level percentage. 0-100.
		 **/
		wantPropertyVariant(VehicleProperty::BrakeFluidLevel, "Brake", AbstractProperty::Read);

		/** @attributeName Washer
		 *  @type unsigned short
		 *  @access readonly
		 *  @attributeComment \brief MUST return Washer fluid level percentage. 0-100.
		 **/
		wantPropertyVariant(VehicleProperty::WasherFluidLevel, "Washer", AbstractProperty::Read);
	}
};

/** @interface Battery : VehiclePropertyType **/
/// TODO: deprecated.  remove in 0.13
class BatteryProperty: public DBusSink
{
public:
	BatteryProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Battery", re, connection, map<string, string>())
	{
		/** @attributeName Voltage
		 *  @type double
		 *  @access readonly
		 *  @attributeComment \brief MUST return battery voltage.
		 **/
		wantPropertyVariant(VehicleProperty::BatteryVoltage, "Voltage", "d", AbstractProperty::Read);

		/** @attributeName Current
		 *  @type double
		 *  @access readonly
		 *  @attributeComment \brief MUST return battery current in Amperes
		 **/
		wantPropertyVariant(VehicleProperty::BatteryCurrent, "Current", "d", AbstractProperty::Read);

	}
};

/** @interface Battery : VehiclePropertyType **/
class BatteryStatusProperty: public DBusSink
{
public:
	BatteryStatusProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("BatteryStatus", re, connection, map<string, string>())
	{
		/** @attributeName Voltage
		 *  @type double
		 *  @access readonly
		 *  @attributeComment \brief MUST return battery voltage.
		 **/
		wantPropertyVariant(VehicleProperty::BatteryVoltage, "Voltage", AbstractProperty::Read);

		/** @attributeName Current
		 *  @type double
		 *  @access readonly
		 *  @attributeComment \brief MUST return battery current in Amperes
		 **/
		wantPropertyVariant(VehicleProperty::BatteryCurrent, "Current", AbstractProperty::Read);

		wantPropertyVariant(VehicleProperty::BatteryChargeLevel, "ChargeLevel", AbstractProperty::Read);

	}
};

/** @interface TirePressure : VehiclePropertyType **/
class TireProperty: public DBusSink
{
public:
	TireProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
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
	EngineCoolant(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("EngineCoolant", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::EngineCoolantLevel, "Level", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::EngineCoolantTemperature, "Temperature", AbstractProperty::Read);
	}
};

#endif
