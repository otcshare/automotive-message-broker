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
		wantProperty<uint>(VehicleProperty::Odometer, "Odometer", "i", AbstractProperty::Read);

		
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
		wantProperty<uint16_t>(VehicleProperty::TransmissionFluidLevel, "Transmission", "q", AbstractProperty::Read);

		/** @attributeName Brake
		 *  @type unsigned short
		 *  @access readonly
		 *  @attributeComment \brief MUST return Brake fluid level percentage. 0-100.
		 **/
		wantProperty<uint16_t>(VehicleProperty::BrakeFluidLevel, "Brake", "q", AbstractProperty::Read);

		/** @attributeName Washer
		 *  @type unsigned short
		 *  @access readonly
		 *  @attributeComment \brief MUST return Washer fluid level percentage. 0-100.
		 **/
		wantProperty<uint16_t>(VehicleProperty::WasherFluidLevel, "Washer", "q", AbstractProperty::Read);
	}
};

/** @interface Battery : VehiclePropertyType **/
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
		wantProperty<double>(VehicleProperty::BatteryVoltage, "Voltage", "d", AbstractProperty::Read);

		/** @attributeName Current
		 *  @type double
		 *  @access readonly
		 *  @attributeComment \brief MUST return battery current in Amperes
		 **/
		wantProperty<double>(VehicleProperty::BatteryCurrent, "Current", "d", AbstractProperty::Read);
		
	}
};

/** @interface TirePressure : VehiclePropertyType **/
class TireProperty: public DBusSink
{
public:
	TireProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Tire", re, connection, map<string, string>())
	{
		wantProperty<double>(VehicleProperty::TirePressure, "Pressure", "d", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::TireTemperature, "Temperature", "d", AbstractProperty::Read);
	}
};

#endif
