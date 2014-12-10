#ifndef ENVIRONMENTPROPERTIES_H_
#define ENVIRONMENTPROPERTIES_H_

#include "dbusplugin.h"
#include "abstractdbusinterface.h"
#include "abstractroutingengine.h"

/** @interface ExteriorBrightness : VehiclePropertyType **/
class ExteriorBrightnessProperty: public DBusSink
{
public:
	ExteriorBrightnessProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("ExteriorBrightness", re, connection, map<string, string>())
	{
		/**
		 * @attributeName ExteriorBrightness
		 * @type unsigned long
		 * @access readonly
		 * @attributeComment \brief Must return the brightness outside the vehicle in lux.
		 */
		wantPropertyVariant(VehicleProperty::ExteriorBrightness,"ExteriorBrightness", "q", AbstractProperty::Read);

	}
};

/** @interface Temperature : VehiclePropertyType **/
class Temperature: public DBusSink
{
public:
	Temperature(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Temperature", re, connection, map<string, string>())
	{

		///TODO Depricated in 0.14.  Use InteriorTemperature
		wantPropertyVariant(VehicleProperty::InteriorTemperature, "Interior", AbstractProperty::Read);

		wantPropertyVariant(VehicleProperty::InteriorTemperature, "InteriorTemperature", AbstractProperty::Read);

		///TODO Depricated in 0.14.  Use ExteriorTemperature
		wantPropertyVariant(VehicleProperty::ExteriorTemperature, "Exterior", AbstractProperty::Read);

		wantPropertyVariant(VehicleProperty::ExteriorTemperature, "ExteriorTemperature", AbstractProperty::Read);


	}
};

/** @interface RainSensor : VehiclePropertyType **/
class RainSensor: public DBusSink
{
public:
	RainSensor(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("RainSensor", re, connection, map<string, string>())
	{
		///TODO: Depricated in 0.14
		wantPropertyVariant(VehicleProperty::RainSensor, "RainSensor", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::RainSensor, "RainIntensity", AbstractProperty::Read);
	}
};

/** @interface WindshieldWiper : VehiclePropertyType **/
///TODO: depricated in 0.14
class WindshieldWiper: public DBusSink
{
public:
	WindshieldWiper(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("WindshieldWiper", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::WindshieldWiper, "WindshieldWiper", AbstractProperty::ReadWrite);
	}
};

class WindshieldWiperStatus: public DBusSink
{
public:
	WindshieldWiperStatus(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("WiperStatus", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::WindshieldWiperSpeed, "WiperSpeed", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::WindshieldWiperSetting, "WiperSetting", AbstractProperty::ReadWrite);
	}
};

/** @interface HVAC : VehiclePropertyType **/
///TODO Depricated in 0.14.  Use ClimateControl
class HVACProperty: public DBusSink
{
public:
	HVACProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("HVAC", re, connection, map<string, string>())
	{
		/**
		 * @enum const unsigned short AIRFLOWDIRECTION_FRONTPANEL = 0;
		 * @enum const unsigned short AIRFLOWDIRECTION_FLOORDUCT= 1;
		 * @enum const unsigned short AIRFLOWDIRECTION_FRONT = 0x02;
		 * @enum const unsigned short AIRFLOWDIRECTION_DEFROSTER = 0x04;
		 **/

		/**
		 * @attributeName AirflowDirection
		 * @type unsigned short
		 * @access readwrite
		 * @attributeComment \brief Must return airflow direction.  See AIRFLOWDIRECTION_*.
		 */
		/// TODO: Deprecated.  Remove in 0.13
		wantPropertyVariant(VehicleProperty::AirflowDirection, "AirflowDirection", AbstractProperty::ReadWrite);

		/**
		 * @attributeName FanSpeed
		 * @type unsigned short
		 * @access readwrite
		 * @attributeComment \brief Must return speed of the fan (0-7)
		 */
		wantPropertyVariant(VehicleProperty::FanSpeed, "FanSpeed", "y", AbstractProperty::ReadWrite);

		/**
		 * @attributeName TargetTemperature
		 * @type unsigned short
		 * @access readwrite
		 * @attributeComment \brief Must return target desired temperature in celcius.
		 */
		wantPropertyVariant(VehicleProperty::TargetTemperature, "TargetTemperature", "y", AbstractProperty::ReadWrite);

		/**
		 * @attributeName AirConditioning
		 * @type boolean
		 * @access readwrite
		 * @attributeComment \brief Must return air conditioning on (true) / off (false).
		 */
		wantPropertyVariant(VehicleProperty::AirConditioning, "AirConditioning", "b", AbstractProperty::ReadWrite);

		/**
		 * @attributeName AirRecirculation
		 * @type boolean
		 * @access readwrite
		 * @attributeComment \brief Must return air recirculation on (true) / off (false).
		 */
		wantPropertyVariant(VehicleProperty::AirRecirculation, "AirRecirculation", "b", AbstractProperty::ReadWrite);

		/**
		 * @attributeName Heater
		 * @type boolean
		 * @access readwrite
		 * @attributeComment \brief Must return heater on (true) / off (false).
		 */
		wantPropertyVariant(VehicleProperty::Heater, "Heater", "b", AbstractProperty::ReadWrite);

		/**
		 * @attributeName SteeringWheelHeater
		 * @type boolean
		 * @access readwrite
		 * @attributeComment \brief Must return air recirculation on (true) / off (false).
		 */
		wantPropertyVariant(VehicleProperty::SteeringWheelHeater, "SteeringWheelHeater", "b", AbstractProperty::ReadWrite);

		/**
		 * @attributeName SeatHeater
		 * @type boolean
		 * @access readwrite
		 * @attributeComment \brief Must return seat heater status: on (true) / off (false).
		 */
		wantPropertyVariant(VehicleProperty::SeatHeater, "SeatHeater", "b", AbstractProperty::ReadWrite);

		/**
		 * @attributeName SeatCooler
		 * @type boolean
		 * @access readwrite
		 * @attributeComment \brief Must return seat heater status: on (true) / off (false).
		 */
		wantPropertyVariant(VehicleProperty::SeatCooler, "SeatCooler", "b", AbstractProperty::ReadWrite);


	}
};

class ClimateControlProperty: public DBusSink
{
public:
	ClimateControlProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("ClimateControl", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::AirflowDirectionW3C, "AirflowDirection", AbstractProperty::ReadWrite);

		wantPropertyVariant(VehicleProperty::FanSpeed, "FanSpeedLevel", AbstractProperty::ReadWrite);

		wantPropertyVariant(VehicleProperty::TargetTemperature, "TargetTemperature", AbstractProperty::ReadWrite);

		wantPropertyVariant(VehicleProperty::AirConditioning, "AirConditioning", AbstractProperty::ReadWrite);

		wantPropertyVariant(VehicleProperty::AirRecirculation, "AirRecirculation", AbstractProperty::ReadWrite);

		wantPropertyVariant(VehicleProperty::Heater, "Heater", AbstractProperty::ReadWrite);

		wantPropertyVariant(VehicleProperty::SteeringWheelHeater, "SteeringWheelHeater", AbstractProperty::ReadWrite);

		wantPropertyVariant(VehicleProperty::SeatHeater, "SeatHeater", AbstractProperty::ReadWrite);

		wantPropertyVariant(VehicleProperty::SeatCooler, "SeatCooler", AbstractProperty::ReadWrite);
	}
};

/** @interface WindowStatus : VehiclePropertyType **/
/// TODO: Depricated in 0.14.  Use SideWindow
class WindowStatusProperty: public DBusSink
{
public:
	WindowStatusProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("WindowStatus", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::WindowStatus,"Openness", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::Defrost, "Defrost", AbstractProperty::ReadWrite);
	}
};

class SideWindowStatusProperty: public DBusSink
{
public:
	SideWindowStatusProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("SideWindow", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::WindowStatus,"Openness", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::WindowLockStatus, "Lock", AbstractProperty::ReadWrite);
	}
};

class DefrostProperty: public DBusSink
{
public:
	DefrostProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Defrost", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::DefrostWindow,"DefrostWindow", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::DefrostMirror, "DefrostMirror", AbstractProperty::ReadWrite);
	}
};

/** @interface Sunroof : VehiclePropertyType **/
class Sunroof: public DBusSink
{
public:
	Sunroof(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Sunroof", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::Sunroof, "Openness", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SunroofTilt, "Tilt", AbstractProperty::ReadWrite);
	}
};

/** @interface ConvertibleRoof : VehiclePropertyType **/
class ConvertibleRoof: public DBusSink
{
public:
	ConvertibleRoof(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("ConvertibleRoof", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::ConvertibleRoof, "Setting", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::ConvertibleRoofStatus, "Status", AbstractProperty::Read);
	}
};

class AtmosphericPressure: public DBusSink
{
public:
	AtmosphericPressure(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("AtmosphericPressure", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::AtmosphericPressure, "Pressure", AbstractProperty::ReadWrite);
	}
};
#endif
