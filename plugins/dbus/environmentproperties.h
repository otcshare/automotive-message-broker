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
		wantPropertyVariant(VehicleProperty::ExteriorBrightness, "ExteriorBrightness", VariantType::Read);
	}
};

/** @interface Temperature : VehiclePropertyType **/
class Temperature: public DBusSink
{
public:
	Temperature(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Temperature", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::InteriorTemperature, "InteriorTemperature", VariantType::Read);
		wantPropertyVariant(VehicleProperty::ExteriorTemperature, "ExteriorTemperature", VariantType::Read);
	}
};

/** @interface RainSensor : VehiclePropertyType **/
class RainSensor: public DBusSink
{
public:
	RainSensor(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("RainSensor", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::RainSensor, "RainIntensity", VariantType::Read);
	}
};

class WindshieldWiperStatus: public DBusSink
{
public:
	WindshieldWiperStatus(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("WiperStatus", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::WindshieldWiperSpeed, "WiperSpeed", VariantType::Read);
		wantPropertyVariant(VehicleProperty::WindshieldWiperSetting, "WiperSetting", VariantType::ReadWrite);
	}
};

class ClimateControlProperty: public DBusSink
{
public:
	ClimateControlProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("ClimateControl", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::AirflowDirectionW3C, "AirflowDirection", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::FanSpeed, "FanSpeedLevel", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::TargetTemperature, "TargetTemperature", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::AirConditioning, "AirConditioning", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::AirRecirculation, "AirRecirculation", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::Heater, "Heater", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::SteeringWheelHeater, "SteeringWheelHeater", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatHeater, "SeatHeater", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatCooler, "SeatCooler", VariantType::ReadWrite);
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
		wantPropertyVariant(VehicleProperty::WindowStatus, "Openness", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::Defrost, "Defrost", VariantType::ReadWrite);
	}
};

class SideWindowStatusProperty: public DBusSink
{
public:
	SideWindowStatusProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("SideWindow", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::WindowStatus, "Openness", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::WindowLockStatus, "Lock", VariantType::ReadWrite);
	}
};

class DefrostProperty: public DBusSink
{
public:
	DefrostProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Defrost", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::DefrostWindow, "DefrostWindow", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::DefrostMirror, "DefrostMirror", VariantType::ReadWrite);
	}
};

/** @interface Sunroof : VehiclePropertyType **/
class Sunroof: public DBusSink
{
public:
	Sunroof(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Sunroof", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::Sunroof, "Openness", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::SunroofTilt, "Tilt", VariantType::ReadWrite);
	}
};

/** @interface ConvertibleRoof : VehiclePropertyType **/
class ConvertibleRoof: public DBusSink
{
public:
	ConvertibleRoof(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("ConvertibleRoof", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::ConvertibleRoof, "Setting", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::ConvertibleRoofStatus, "Status", VariantType::Read);
	}
};

class AtmosphericPressure: public DBusSink
{
public:
	AtmosphericPressure(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("AtmosphericPressure", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::AtmosphericPressure, "Pressure", VariantType::ReadWrite);
	}
};
#endif
