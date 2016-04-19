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
		wantPropertyVariant(VehicleProperty::VehicleSpeed, "Speed",  VariantType::Read);
	}
};

/** @interface EngineSpeed : VehiclePropertyType **/
class EngineSpeedProperty: public DBusSink
{
public:
	EngineSpeedProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("EngineSpeed", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::EngineSpeed, "Speed", VariantType::Read);
	}
};

/** @interface VehiclePowerMode : VehiclePropertyType **/
class VehiclePowerModeProperty: public DBusSink
{
public:
	VehiclePowerModeProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("VehiclePowerMode", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::VehiclePowerMode, "VehiclePowerMode",VariantType::Read);
	}
};

/** @interface TripMeter : VehiclePropertyType **/
class TripMeterProperty: public DBusSink
{
public:
	TripMeterProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
	:DBusSink("TripMeter", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::TripMeters, "TripMeters", VariantType::ReadWrite);
	}
};

/** @interface Acceleration : VehiclePropertyType **/
class AccelerationProperty: public DBusSink
{
public:
	AccelerationProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
	:DBusSink("Acceleration", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::AccelerationX, "X", VariantType::Read);
		wantPropertyVariant(VehicleProperty::AccelerationY, "Y", VariantType::Read);
		wantPropertyVariant(VehicleProperty::AccelerationZ, "Z", VariantType::Read);
	}
};

/** @interface Transmission : VehiclePropertyType **/
class TransmissionProperty: public DBusSink
{
public:
	TransmissionProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
	:DBusSink("Transmission", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::TransmissionShiftPosition, "ShiftPosition", VariantType::Read);
		wantPropertyVariant(VehicleProperty::TransmissionGearPosition, "GearPosition", VariantType::Read);
		wantPropertyVariant(VehicleProperty::TransmissionGearPosition, "Gear", VariantType::Read);
		wantPropertyVariant(VehicleProperty::TransmissionModeW3C, "Mode", VariantType::Read);
		wantPropertyVariant(VehicleProperty::TransmissionGearType, "Type", VariantType::Read);
	}
};

/** @interface CruiseControlStatus : VehiclePropertyType **/
class CruiseControlProperty: public DBusSink
{
public:
	CruiseControlProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
	:DBusSink("CruiseControlStatus", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::CruiseControlSpeed, "Speed", VariantType::Read);
		wantPropertyVariant(VehicleProperty::CruiseControlActive, "Status", VariantType::Read);
	}
};

/** @interface WheelBrake : VehiclePropertyType **/
class BrakeOperation: public DBusSink
{
public:
	BrakeOperation(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
		:DBusSink("BrakeOperation", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::WheelBrake, "BrakePedalDepressed", VariantType::Read);
	}
};

/** @interface LightStatus : VehiclePropertyType **/
class LightStatusProperty: public DBusSink
{
public:
	LightStatusProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
		:DBusSink("LightStatus", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::LightHead, "Head", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightRightTurn, "RightTurn", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightLeftTurn, "LeftTurn", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightBrake, "Brake", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightFog, "Fog", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightHazard, "Hazard", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightParking, "Parking", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightHighBeam, "HighBeam", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightAutomatic, "AutomaticHeadlights", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightDynamicHighBeam, "DynamicHighBeam", VariantType::ReadWrite);
	}
};

class InteriorLightStatusProperty: public DBusSink
{
public:
	InteriorLightStatusProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
		:DBusSink("InteriorLightStatus", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::InteriorLightStatus, "Status", VariantType::ReadWrite);
	}
};

/** @interface Horn : VehiclePropertyType **/
class HornProperty: public DBusSink
{
public:
	HornProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
		:DBusSink("Horn", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::Horn, "Status", VariantType::Read);
	}
};

/** @interface Fuel : VehiclePropertyType **/
class FuelProperty: public DBusSink
{
public:
	FuelProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
		:DBusSink("Fuel", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::FuelLevel, "Level", VariantType::Read);
		wantPropertyVariant(VehicleProperty::FuelRange, "Range", VariantType::Read);
		wantPropertyVariant(VehicleProperty::FuelConsumption, "InstantConsumption", VariantType::Read);
		wantPropertyVariant(VehicleProperty::FuelEconomy, "InstantEconomy", VariantType::Read);
		wantPropertyVariant(VehicleProperty::FuelAverageEconomy, "AverageEconomy", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::FuelAverageConsumption, "AverageConsumption", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::FuelConsumptionSinceRestart, "FuelConsumedSinceRestart", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::FuelTimeSinceRestart, "TimeSinceRestart", VariantType::ReadWrite);
	}
};

/** @interface EngineOil : VehiclePropertyType **/
class EngineOilProperty: public DBusSink
{
public:
	EngineOilProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("EngineOil", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::EngineOilRemaining, "Level", VariantType::Read);
		wantPropertyVariant(VehicleProperty::EngineOilTemperature, "Temperature", VariantType::Read);
		wantPropertyVariant(VehicleProperty::EngineOilPressure, "Pressure", VariantType::Read);
		wantPropertyVariant(VehicleProperty::EngineOilChangeIndicator, "Change", VariantType::Read);
		wantPropertyVariant(VehicleProperty::EngineOilLifeRemaining, "LifeRemaining", VariantType::Read);
	}
};

/** @interface Location : VehiclePropertyType **/
class LocationProperty: public DBusSink
{
public:
	LocationProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("Location", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::Latitude, "Latitude", VariantType::Read);
		wantPropertyVariant(VehicleProperty::Longitude, "Longitude", VariantType::Read);
		wantPropertyVariant(VehicleProperty::Altitude, "Altitude", VariantType::Read);
		wantPropertyVariant(VehicleProperty::Direction, "Direction", VariantType::Read);

	}
};

class SteeringWheel: public DBusSink
{
public:
	SteeringWheel(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("SteeringWheel", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::SteeringWheelAngleW3C, "Angle", VariantType::Read);
	}
};

class ThrottlePosition: public DBusSink
{
public:
	ThrottlePosition(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("ThrottlePosition", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::ThrottlePosition, "Value", VariantType::Read);
	}
};

class NightMode: public DBusSink
{
public:
	NightMode(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("NightMode", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::NightMode, "NightMode", VariantType::Read);
		wantPropertyVariant(VehicleProperty::NightMode, "Mode", VariantType::Read);
	}
};

class DrivingMode: public DBusSink
{
public:
	DrivingMode(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("DrivingMode", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::DrivingMode, "DrivingMode", VariantType::Read);
		wantPropertyVariant(VehicleProperty::DrivingModeW3C, "Mode", VariantType::Read);
	}
};

class PowertrainTorque: public DBusSink
{
public:
	PowertrainTorque(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("PowertrainTorque", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::PowertrainTorque, "Value", VariantType::Read);
	}
};

class AcceleratorPedalPosition: public DBusSink
{
public:
	AcceleratorPedalPosition(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("AcceleratorPedalPosition", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::AcceleratorPedalPosition, "Value", VariantType::Read);
	}
};

class Chime: public DBusSink
{
public:
	Chime(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("Chime", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::Chime, "Status", VariantType::Read);
	}
};

class WheelTick: public DBusSink
{
public:
	WheelTick(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("WheelTick", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::WheelTick, "Value", VariantType::Read);
	}
};

class IgnitionTime: public DBusSink
{
public:
	IgnitionTime(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("IgnitionTime", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::IgnitionTimeOn, "IgnitionTimeOn", VariantType::Read);
		wantPropertyVariant(VehicleProperty::IgnitionTimeOff, "IgnitionTimeOff", VariantType::Read);
	}
};

class YawRate: public DBusSink
{
public:
	YawRate(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("YawRate", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::YawRate, "Value", VariantType::Read);
	}
};

class ButtonEvent: public DBusSink
{
public:
	ButtonEvent(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("ButtonEvent", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::YawRate, "Value", VariantType::Read);
	}
};

#endif
