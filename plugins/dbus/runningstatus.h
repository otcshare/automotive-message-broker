/*
	Copyright (C) 2012  Intel Corporation

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _PROPERTIES_H_H_H_
#define _PROPERTIES_H_H_H_

#include "dbusplugin.h"
#include "abstractdbusinterface.h"
#include "abstractroutingengine.h"

/** @interface VehicleSpeed : VehiclePropertyType **/
class VehicleSpeedProperty: public DBusSink
{
public:
	VehicleSpeedProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("VehicleSpeed", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::VehicleSpeed, "Speed",  AbstractProperty::Read);
	}
};

/** @interface EngineSpeed : VehiclePropertyType **/
class EngineSpeedProperty: public DBusSink
{
public:
	EngineSpeedProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("EngineSpeed", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::EngineSpeed, "Speed", AbstractProperty::Read);
	}
};

/** @interface VehiclePowerMode : VehiclePropertyType **/
class VehiclePowerModeProperty: public DBusSink
{
public:
	VehiclePowerModeProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("VehiclePowerMode", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::VehiclePowerMode, "VehiclePowerMode",AbstractProperty::Read);
	}
};

/** @interface TripMeter : VehiclePropertyType **/
class TripMeterProperty: public DBusSink
{
public:
	TripMeterProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
	:DBusSink("TripMeter", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::TripMeters, "TripMeters", AbstractProperty::ReadWrite);
	}
};

/** @interface Acceleration : VehiclePropertyType **/
class AccelerationProperty: public DBusSink
{
public:
	AccelerationProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
	:DBusSink("Acceleration", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::AccelerationX, "X", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::AccelerationY, "Y", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::AccelerationZ, "Z", AbstractProperty::Read);
	}
};

/** @interface Transmission : VehiclePropertyType **/
class TransmissionProperty: public DBusSink
{
public:
	TransmissionProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
	:DBusSink("Transmission", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::TransmissionShiftPosition, "ShiftPosition", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::TransmissionGearPosition, "GearPosition", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::TransmissionGearPosition, "Gear", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::TransmissionModeW3C, "Mode", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::TransmissionGearType, "Type", AbstractProperty::Read);
	}
};

/** @interface CruiseControlStatus : VehiclePropertyType **/
class CruiseControlProperty: public DBusSink
{
public:
	CruiseControlProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
	:DBusSink("CruiseControlStatus", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::CruiseControlSpeed, "Speed", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::CruiseControlActive, "Status", AbstractProperty::Read);
	}
};

/** @interface WheelBrake : VehiclePropertyType **/
class BrakeOperation: public DBusSink
{
public:
	BrakeOperation(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
		:DBusSink("BrakeOperation", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::WheelBrake, "BrakePedalDepressed", AbstractProperty::Read);
	}
};

/** @interface LightStatus : VehiclePropertyType **/
class LightStatusProperty: public DBusSink
{
public:
	LightStatusProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
		:DBusSink("LightStatus", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::LightHead, "Head", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightRightTurn, "RightTurn", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightLeftTurn, "LeftTurn", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightBrake, "Brake", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightFog, "Fog", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightHazard, "Hazard", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightParking, "Parking", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightHighBeam, "HighBeam", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightAutomatic, "AutomaticHeadlights", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightDynamicHighBeam, "DynamicHighBeam", AbstractProperty::ReadWrite);
	}
};

class InteriorLightStatusProperty: public DBusSink
{
public:
	InteriorLightStatusProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
		:DBusSink("InteriorLightStatus", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::InteriorLightStatus, "Status", AbstractProperty::ReadWrite);
	}
};

/** @interface Horn : VehiclePropertyType **/
class HornProperty: public DBusSink
{
public:
	HornProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
		:DBusSink("Horn", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::Horn, "Status", AbstractProperty::Read);
	}
};

/** @interface Fuel : VehiclePropertyType **/
class FuelProperty: public DBusSink
{
public:
	FuelProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
		:DBusSink("Fuel", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::FuelLevel, "Level", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::FuelRange, "Range", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::FuelConsumption, "InstantConsumption", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::FuelEconomy, "InstantEconomy", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::FuelAverageEconomy, "AverageEconomy", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::FuelAverageConsumption, "AverageConsumption", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::FuelConsumptionSinceRestart, "FuelConsumedSinceRestart", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::FuelTimeSinceRestart, "TimeSinceRestart", AbstractProperty::ReadWrite);
	}
};

/** @interface EngineOil : VehiclePropertyType **/
class EngineOilProperty: public DBusSink
{
public:
	EngineOilProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("EngineOil", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::EngineOilRemaining, "Level", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::EngineOilTemperature, "Temperature", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::EngineOilPressure, "Pressure", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::EngineOilChangeIndicator, "Change", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::EngineOilLifeRemaining, "LifeRemaining", AbstractProperty::Read);
	}
};

/** @interface Location : VehiclePropertyType **/
class LocationProperty: public DBusSink
{
public:
	LocationProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("Location", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::Latitude, "Latitude", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::Longitude, "Longitude", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::Altitude, "Altitude", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::Direction, "Direction", AbstractProperty::Read);

	}
};

class SteeringWheel: public DBusSink
{
public:
	SteeringWheel(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("SteeringWheel", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::SteeringWheelAngleW3C, "Angle", AbstractProperty::Read);
	}
};

class ThrottlePosition: public DBusSink
{
public:
	ThrottlePosition(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("ThrottlePosition", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::ThrottlePosition, "Value", AbstractProperty::Read);
	}
};

class NightMode: public DBusSink
{
public:
	NightMode(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("NightMode", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::NightMode, "NightMode", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::NightMode, "Mode", AbstractProperty::Read);
	}
};

class DrivingMode: public DBusSink
{
public:
	DrivingMode(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("DrivingMode", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::DrivingMode, "DrivingMode", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::DrivingModeW3C, "Mode", AbstractProperty::Read);
	}
};

class PowertrainTorque: public DBusSink
{
public:
	PowertrainTorque(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("PowertrainTorque", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::PowertrainTorque, "Value", AbstractProperty::Read);
	}
};

class AcceleratorPedalPosition: public DBusSink
{
public:
	AcceleratorPedalPosition(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("AcceleratorPedalPosition", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::AcceleratorPedalPosition, "Value", AbstractProperty::Read);
	}
};

class Chime: public DBusSink
{
public:
	Chime(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("Chime", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::Chime, "Status", AbstractProperty::Read);
	}
};

class WheelTick: public DBusSink
{
public:
	WheelTick(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("WheelTick", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::WheelTick, "Value", AbstractProperty::Read);
	}
};

class IgnitionTime: public DBusSink
{
public:
	IgnitionTime(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("IgnitionTime", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::IgnitionTimeOn, "IgnitionTimeOn", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::IgnitionTimeOff, "IgnitionTimeOff", AbstractProperty::Read);
	}
};

class YawRate: public DBusSink
{
public:
	YawRate(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("YawRate", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::YawRate, "Value", AbstractProperty::Read);
	}
};

class ButtonEvent: public DBusSink
{
public:
	ButtonEvent(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("ButtonEvent", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::YawRate, "Value", AbstractProperty::Read);
	}
};

#endif
