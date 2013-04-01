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
		:DBusSink("org.automotive.Odometer","/org/automotive/maintenance/Odometer", re, connection, map<string, string>())
	{
		/** @attributeName Odometer
		 *  @type unsigned long
		 *  @access readonly
		 *  @attributeComment \brief MUST return Distance traveled in km
		 **/
		wantProperty<uint>(VehicleProperty::Odometer, "Odometer", "i", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

/** @interface Fluid : VehiclePropertyType **/
class FluidProperty : public DBusSink
{
public:
	FluidProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.Fluid","/org/automotive/maintenance/Fluid", re, connection, map<string, string>())
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
		:DBusSink("org.automotive.Battery","/org/automotive/maintenance/Battery", re, connection, map<string, string>())
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
		supportedChanged(re->supported());
	}
};

/** @interface TirePressure : VehiclePropertyType **/
class TirePressureProperty: public DBusSink
{
public:
	TirePressureProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.TirePressure","/org/automotive/maintenance/TirePressure", re, connection, map<string, string>())
	{
		/** @attributeName LeftFront
		 *  @type double
		 *  @access readonly
		 *  @attributeComment \brief MUST return left front tire pressure in kPa.
		 **/
		wantProperty<double>(VehicleProperty::TirePressureLeftFront, "LeftFront", "d", AbstractProperty::Read);

		/** @attributeName RightFront
		 *  @type double
		 *  @access readonly
		 *  @attributeComment \brief MUST return right front tire pressure in kPa.
		 **/
		wantProperty<double>(VehicleProperty::TirePressureRightFront, "RightFront", "d", AbstractProperty::Read);

		/** @attributeName LeftRear
		 *  @type double
		 *  @access readonly
		 *  @attributeComment \brief MUST return left rear tire pressure in kPa.
		 **/
		wantProperty<double>(VehicleProperty::TirePressureLeftRear, "LeftRear", "d", AbstractProperty::Read);

		/** @attributeName RightRear
		 *  @type double
		 *  @access readonly
		 *  @attributeComment \brief MUST return right rear tire pressure in kPa.
		 **/
		wantProperty<double>(VehicleProperty::TirePressureRightRear, "RightRear", "d", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

/** @interface TireTemperature : VehiclePropertyType **/
class TireTemperatureProperty: public DBusSink
{
public:
	TireTemperatureProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.TireTemperature","/org/automotive/maintenance/TireTemperature", re, connection, map<string, string>())
	{
		/** @attributeName LeftFront
		 *  @type double
		 *  @access readonly
		 *  @attributeComment \brief MUST return left front tire temperature in Celcius.
		 **/
		wantProperty<double>(VehicleProperty::TireTemperatureLeftFront, "LeftFront", "d", AbstractProperty::Read);

		/** @attributeName RightFront
		 *  @type double
		 *  @access readonly
		 *  @attributeComment \brief MUST return right front tire temperature in Celcius.
		 **/
		wantProperty<double>(VehicleProperty::TireTemperatureRightFront, "RightFront", "d", AbstractProperty::Read);

		/** @attributeName LeftRear
		 *  @type double
		 *  @access readonly
		 * @attributeComment \brief MUST return left rear tire temperature in Celcius.
		 **/
		wantProperty<double>(VehicleProperty::TireTemperatureLeftRear, "LeftRear", "d", AbstractProperty::Read);

		/** @attributeName RightRear
		 *  @type double
		 *  @access readonly
		 *  @attributeComment \brief MUST return right rear tire temperature in Celcius.
		 **/
		wantProperty<double>(VehicleProperty::TireTemperatureRightRear, "RightRear", "d", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

#endif
