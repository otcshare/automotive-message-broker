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
		/** @attributeName VehicleSpeed
		 *  @type unsigned long
		 *  @access readonly
		 *  @attributeComment \brief  Must return Vehicle Speed in kilometers per hour.
		 **/
		wantPropertyVariant(VehicleProperty::VehicleSpeed,"Speed", "i", AbstractProperty::Read);

		/// Deprecated:
		wantPropertyVariant(VehicleProperty::VehicleSpeed,"VehicleSpeed", "i", AbstractProperty::Read);

	}


};

/** @interface EngineSpeed : VehiclePropertyType **/
class EngineSpeedProperty: public DBusSink
{
public:
	EngineSpeedProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("EngineSpeed", re, connection, map<string, string>())
	{
		/** @attributeName EngineSpeed
		 *  @type unsigned long
		 *  @access readonly
		 *  @attributeComment \brief  Must return Engine Speed in rotations per minute.
		 **/
		wantPropertyVariant(VehicleProperty::EngineSpeed,"Speed", "i", AbstractProperty::Read);

		/// Deprecated:
		wantPropertyVariant(VehicleProperty::EngineSpeed,"EngineSpeed", "i", AbstractProperty::Read);

	}


};

/** @interface VehiclePowerMode : VehiclePropertyType **/
class VehiclePowerModeProperty: public DBusSink
{
public:
	VehiclePowerModeProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("VehiclePowerMode", re, connection, map<string, string>())
	{
		/**
		 * @enum const unsigned short VEHICLEPOWERMODE_OFF = 0,
		 * @enum const unsigned short VEHICLEPOWERMODE_ACCESSORY1 = 1,
		 * @enum const unsigned short VEHICLEPOWERMODE_ACCESSORY2 = 2,
		 * @enum const unsigned short VEHICLEPOWERMODE_RUN = 3;
		 **/


		/** @attributeName VehiclePowerMode
		 *  @type octet
		 *  @access readonly
		 *  @attributeComment \brief  Must return Vehicle Power mode (see VEHICLEPOWERMODE)
		 **/
		wantPropertyVariant(VehicleProperty::VehiclePowerMode, "VehiclePowerMode","i",AbstractProperty::Read);

	}
};

/** @interface TripMeter : VehiclePropertyType **/
class TripMeterProperty: public DBusSink
{
public:
	TripMeterProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
	:DBusSink("TripMeter", re, connection, map<string, string>())
	{
		/** @attributeName TripMeters
		 *  @type sequence<unsigned long>
		 *  @attributeComment \brief  Must return trip metes. Changing any items in the array will reset the item's value to '0''.
		 **/
		wantPropertyVariant(VehicleProperty::TripMeters, "TripMeters", "aq", AbstractProperty::ReadWrite);


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

		wantPropertyVariant(VehicleProperty::TransmissionShiftPosition,
														  "ShiftPosition", "y", AbstractProperty::Read);

		wantPropertyVariant(VehicleProperty::TransmissionGearPosition,
														  "GearPosition", "y", AbstractProperty::Read);

		wantPropertyVariant(VehicleProperty::TransmissionGearPosition,
														  "Gear", "y", AbstractProperty::Read);

		wantPropertyVariant(VehicleProperty::TransmissionModeW3C,
														  "Mode", "y", AbstractProperty::Read);

		wantPropertyVariant(VehicleProperty::TransmissionGearType, "Type", "q", AbstractProperty::Read);
	}
};

/** @interface CruiseControlStatus : VehiclePropertyType **/
class CruiseControlProperty: public DBusSink
{
public:
	CruiseControlProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
	:DBusSink("CruiseControlStatus", re, connection, map<string, string>())
	{
		///TODO: deprecate Activated.  Remove in 0.14
		wantPropertyVariant(VehicleProperty::CruiseControlActive, "Activated", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::CruiseControlSpeed, "Speed", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::CruiseControlActive, "Status", AbstractProperty::Read);
	}
};

/** @interface WheelBrake : VehiclePropertyType **/
/// TODO: deprecated remove in 0.14
class WheelBrakeProperty: public DBusSink
{
public:
	WheelBrakeProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
		:DBusSink("WheelBrake", re, connection, map<string, string>())
	{
		/** @attributeName Engaged
		 *  @type boolean
		 *  @access readonly
		 *  @attributeComment \brief  Must return Wheel Brake status: Engaged = true, disengaged = false
		 **/
		wantPropertyVariant(VehicleProperty::WheelBrake, "Engaged", "b", AbstractProperty::Read);

	}
};

/** @interface WheelBrake : VehiclePropertyType **/
class BrakeOperation: public DBusSink
{
public:
	BrakeOperation(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
		:DBusSink("BrakeOperation", re, connection, map<string, string>())
	{
		/** @attributeName Engaged
		 *  @type boolean
		 *  @access readonly
		 *  @attributeComment \brief  Must return Wheel Brake status: Engaged = true, disengaged = false
		 **/
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
		wantPropertyVariant(VehicleProperty::LightHead, "Head", "b", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightRightTurn, "RightTurn", "b", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightLeftTurn, "LeftTurn", "b", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightBrake, "Brake", "b", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightFog, "Fog", "b", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightHazard, "Hazard", "b", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightParking, "Parking", "b", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::LightHighBeam, "HighBeam", "b", AbstractProperty::ReadWrite);
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
		/// TODO: deprecated remove in 0.14
		wantPropertyVariant(VehicleProperty::InteriorLightPassenger, "Passenger", AbstractProperty::Read);
		/// TODO: deprecated remove in 0.14
		wantPropertyVariant(VehicleProperty::InteriorLightPassenger, "Driver", AbstractProperty::Read);
		/// TODO: deprecated remove in 0.14
		wantPropertyVariant(VehicleProperty::InteriorLightCenter, "Center", AbstractProperty::Read);

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
		/// TODO: deprecated remove in 0.14
		wantPropertyVariant(VehicleProperty::Horn,"On",AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::Horn,"Status",AbstractProperty::Read);

	}
};

/** @interface Fuel : VehiclePropertyType **/
class FuelProperty: public DBusSink
{
public:
	FuelProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
		:DBusSink("Fuel", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::FuelLevel,"Level", AbstractProperty::Read);

		wantPropertyVariant(VehicleProperty::FuelRange,"Range", AbstractProperty::Read);

		wantPropertyVariant(VehicleProperty::FuelConsumption,"InstantConsumption", AbstractProperty::Read);

		wantPropertyVariant(VehicleProperty::FuelEconomy,"InstantEconomy", AbstractProperty::Read);

		wantPropertyVariant(VehicleProperty::FuelAverageEconomy,"AverageEconomy", AbstractProperty::ReadWrite);

		wantPropertyVariant(VehicleProperty::FuelAverageConsumption,"AverageConsumption", AbstractProperty::ReadWrite);

		wantPropertyVariant(VehicleProperty::FuelConsumptionSinceRestart,"FuelConsumedSinceRestart", AbstractProperty::ReadWrite);

		wantPropertyVariant(VehicleProperty::FuelTimeSinceRestart,"TimeSinceRestart", AbstractProperty::ReadWrite);

	}
};

/** @interface EngineOil : VehiclePropertyType **/
class EngineOilProperty: public DBusSink
{
public:
	EngineOilProperty(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("EngineOil", re, connection, map<string, string>())
	{
		///TODO depricated.  Use Level.  Remove in 0.14
		wantPropertyVariant(VehicleProperty::EngineOilRemaining, "Remaining", AbstractProperty::Read);
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
		/** @attributeName Latitude
		 *  @type double
		 *  @access readonly
		 *  @attributeComment \brief  Must return latitude in Deg.Min (-180, +180)
		 **/
		wantPropertyVariant(VehicleProperty::Latitude, "Latitude", "d", AbstractProperty::Read);

		/** @attributeName Longitude
		 *  @type double
		 *  @access readonly
		 *  @attributeComment \brief  Must return longitude in Deg.Min (-90, +90)
		 **/
		wantPropertyVariant(VehicleProperty::Longitude, "Longitude", "d", AbstractProperty::Read);

		/** @attributeName Altitude
		 *  @type double
		 *  @access readonly
		 *  @attributeComment \brief  Must return altitude in meters above sea-level (0).
		 **/
		wantPropertyVariant(VehicleProperty::Altitude, "Altitude", "d", AbstractProperty::Read);

		/** @attributeName Direction
		 *  @type unsigned short
		 *  @access readonly
		 *  @attributeComment \brief  Must return direction in Degrees  (0-360)
		 **/
		wantPropertyVariant(VehicleProperty::Direction, "Direction", "q", AbstractProperty::Read);

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
		wantPropertyVariant(VehicleProperty::NightMode, "NightMode", "b", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::NightMode, "Mode", "b", AbstractProperty::Read);
	}
};

class DrivingMode: public DBusSink
{
public:
	DrivingMode(VehicleProperty::Property, AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("DrivingMode", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::DrivingMode, "DrivingMode", "i", AbstractProperty::Read);
		wantPropertyVariant(VehicleProperty::DrivingModeW3C, "Mode", "b", AbstractProperty::Read);
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
