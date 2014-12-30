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
		wantPropertyVariant(VehicleProperty::ExteriorBrightness, "ExteriorBrightness", AbstractProperty::Read);
	}
};

/** @interface Temperature : VehiclePropertyType **/
class Temperature: public DBusSink
{
public:
	Temperature(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Temperature", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::InteriorTemperature, "InteriorTemperature", AbstractProperty::Read);
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
		wantPropertyVariant(VehicleProperty::RainSensor, "RainIntensity", AbstractProperty::Read);
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
		wantPropertyVariant(VehicleProperty::WindowStatus, "Openness", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::Defrost, "Defrost", AbstractProperty::ReadWrite);
	}
};

class SideWindowStatusProperty: public DBusSink
{
public:
	SideWindowStatusProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("SideWindow", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::WindowStatus, "Openness", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::WindowLockStatus, "Lock", AbstractProperty::ReadWrite);
	}
};

class DefrostProperty: public DBusSink
{
public:
	DefrostProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Defrost", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::DefrostWindow, "DefrostWindow", AbstractProperty::ReadWrite);
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
