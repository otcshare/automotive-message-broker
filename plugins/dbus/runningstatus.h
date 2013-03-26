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
	VehicleSpeedProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.VehicleSpeed","/org/automotive/runningstatus/VehicleSpeed", re, connection, map<string, string>())
	{
		/** @attributeName VehicleSpeed
		 *  @type unsigned short
		 *  @access readonly
		 **/
		wantProperty<uint16_t>(VehicleProperty::VehicleSpeed,"VehicleSpeed", "i", AbstractProperty::Read);
		supportedChanged(re->supported());
	}


};

/** @interface EngineSpeed : VehiclePropertyType **/
class EngineSpeedProperty: public DBusSink
{
public:
	EngineSpeedProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.EngineSpeed","/org/automotive/runningstatus/EngineSpeed", re, connection, map<string, string>())
	{
		/** @attributeName EngineSpeed
		 *  @type unsigned short
		 *  @access readonly
		 **/
		wantProperty<uint16_t>(VehicleProperty::EngineSpeed,"EngineSpeed", "i", AbstractProperty::Read);
		supportedChanged(re->supported());
	}


};

/** @interface VehiclePowerMode : VehiclePropertyType **/
class VehiclePowerModeProperty: public DBusSink
{
public:
	VehiclePowerModeProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.VehiclePowerMode","/org/automotive/runningstatus/VehiclePowerMode", re, connection, map<string, string>())
	{
		/** @attributeName VehiclePowerMode
		 *  @type octet
		 *  @access readonly
		 **/
		wantProperty<Power::PowerModes>(VehicleProperty::VehiclePowerMode, "VehiclePowerMode","b",AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

/** @interface TripMeter : VehiclePropertyType **/
class TripMeterProperty: public DBusSink
{
public:
	TripMeterProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
	:DBusSink("org.automotive.TripMeter","/org/automotive/runningstatus/TripMeter", re, connection, map<string, string>())
	{
		/** @attributeName TripMeterA
		 *  @type unsigned short
		 *  setting this will reset the value to '0'.
		 **/
		wantProperty<uint16_t>(VehicleProperty::TripMeterA, "A", "q", AbstractProperty::ReadWrite);

		/** @attributeName TripMeterB
		 *  @type unsigned short
		 *  setting this will reset the value to '0'.
		 **/
		wantProperty<uint16_t>(VehicleProperty::TripMeterB, "B", "q", AbstractProperty::ReadWrite);

		/** @attributeName TripMeterC
		 *  @type unsigned short
		 *  setting this will reset the value to '0'.
		 **/
		wantProperty<uint16_t>(VehicleProperty::TripMeterC, "C", "q", AbstractProperty::ReadWrite);
		supportedChanged(re->supported());
	}
};

/** @interface Acceleration : VehiclePropertyType **/
class AccelerationProperty: public DBusSink
{
public:
	AccelerationProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
	:DBusSink("org.automotive.Acceleration","/org/automotive/runningstatus/Acceleration", re, connection, map<string, string>())
	{
		/** @attributeName X
		 *  @type unsigned short
		 *  @access readonly
		 **/
		wantProperty<uint16_t>(VehicleProperty::AccelerationX, "X", "q", AbstractProperty::Read);

		/** @attributeName Y
		 *  @type unsigned short
		 *  @access readonly
		 **/
		wantProperty<uint16_t>(VehicleProperty::AccelerationY, "Y", "q", AbstractProperty::Read);

		/** @attributeName Z
		 *  @type unsigned short
		 *  @access readonly
		 **/
		wantProperty<uint16_t>(VehicleProperty::AccelerationZ, "Z", "q", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

/** @interface Transmission : VehiclePropertyType **/
class TransmissionProperty: public DBusSink
{
public:
	TransmissionProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
	:DBusSink("org.automotive.Transmission","/org/automotive/runningstatus/Transmission", re, connection, map<string, string>())
	{
		/** @attributeName ShiftPosition
		 *  @type octet
		 *  @access readonly
		 **/
		wantProperty<Transmission::TransmissionPositions>(VehicleProperty::TransmissionShiftPosition,
														  "ShiftPosition", "y", AbstractProperty::Read);

		/** @attributeName GearPosition
		 *  @type octet
		 *  @access readonly
		 **/
		wantProperty<Transmission::TransmissionPositions>(VehicleProperty::TransmissionGearPosition,
														  "GearPosition", "y", AbstractProperty::Read);

		/** @attributeName Mode
		 *  @type octet
		 *  @access readonly
		 **/
		wantProperty<Transmission::Mode>(VehicleProperty::TransmissionMode,
														  "Mode", "y", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

/** @interface CruiseControlStatus : VehiclePropertyType **/
class CruiseControlProperty: public DBusSink
{
public:
	CruiseControlProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
	:DBusSink("org.automotive.CruiseControlStatus","/org/automotive/runningstatus/CruiseControlStatus", re, connection, map<string, string>())
	{
		/** @attributeName Activated
		 *  @type boolean
		 *  @access readonly
		 **/
		wantProperty<bool>(VehicleProperty::CruiseControlActive, "Activated", "b", AbstractProperty::Read);

		/** @attributeName Speed
		 *  @type unsigned short
		 *  @access readonly
		 **/
		wantProperty<uint16_t>(VehicleProperty::CruiseControlSpeed, "Speed", "q", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

/** @interface WheelBrake : VehiclePropertyType **/
class WheelBrakeProperty: public DBusSink
{
public:
	WheelBrakeProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
		:DBusSink("org.automotive.WheelBrake","/org/automotive/runningstatus/WheelBrake", re, connection, map<string, string>())
	{
		/** @attributeName Engaged
		 *  @type boolean
		 *  @access readonly
		 **/
		wantProperty<bool>(VehicleProperty::WheelBrake, "Engaged", "b", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

/** @interface LightStatus : VehiclePropertyType **/
class LightStatusProperty: public DBusSink
{
public:
	LightStatusProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
		:DBusSink("org.automotive.LightStatus","/org/automotive/runningstatus/LightStatus", re, connection, map<string, string>())
	{
		/** @attributeName Head
		 *  @type boolean
		 *  @access readonly
		 **/
		wantProperty<bool>(VehicleProperty::LightHead, "Head", "b", AbstractProperty::Read);

		/** @attributeName RightTurn
		 *  @type boolean
		 *  @access readonly
		 **/
		wantProperty<bool>(VehicleProperty::LightRightTurn, "RightTurn", "b", AbstractProperty::Read);

		/** @attributeName LeftTurn
		 *  @type boolean
		 *  @access readonly
		 **/
		wantProperty<bool>(VehicleProperty::LightLeftTurn, "LeftTurn", "b", AbstractProperty::Read);

		/** @attributeName Brake
		 *  @type boolean
		 *  @access readonly
		 **/
		wantProperty<bool>(VehicleProperty::LightBrake, "Brake", "b", AbstractProperty::Read);

		/** @attributeName Fog
		 *  @type boolean
		 *  @access readonly
		 **/
		wantProperty<bool>(VehicleProperty::LightFog, "Fog", "b", AbstractProperty::Read);

		/** @attributeName Hazard
		 *  @type boolean
		 *  @access readonly
		 **/
		wantProperty<bool>(VehicleProperty::LightHazard, "Hazard", "b", AbstractProperty::Read);

		/** @attributeName Parking
		 *  @type boolean
		 *  @access readonly
		 **/
		wantProperty<bool>(VehicleProperty::LightParking, "Parking", "b", AbstractProperty::Read);

		/** @attributeName HighBeam
		 *  @type boolean
		 *  @access readonly
		 **/
		wantProperty<bool>(VehicleProperty::LightHighBeam, "HighBeam", "b", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

/** @interface InteriorLightStatus : VehiclePropertyType **/
class InteriorLightStatusProperty: public DBusSink
{
public:
	InteriorLightStatusProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
		:DBusSink("org.automotive.InteriorLightStatus","/org/automotive/runningstatus/InteriorLightStatus", re, connection, map<string, string>())
	{
		/** @attributeName Passenger
		 *  @type boolean
		 *  @access readonly
		 **/
		wantProperty<bool>(VehicleProperty::InteriorLightPassenger, "Passenger", "b", AbstractProperty::Read);

		/** @attributeName Driver
		 *  @type boolean
		 *  @access readonly
		 **/
		wantProperty<bool>(VehicleProperty::InteriorLightPassenger, "Driver", "b", AbstractProperty::Read);

		/** @attributeName Center
		 *  @type boolean
		 *  @access readonly
		 **/
		wantProperty<bool>(VehicleProperty::InteriorLightCenter, "Center", "b", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

/** @interface Horn : VehiclePropertyType **/
class HornProperty: public DBusSink
{
public:
	HornProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
		:DBusSink("org.automotive.Horn","/org/automotive/runningstatus/Horn", re, connection, map<string, string>())
	{
		/** @attributeName On
		 *  @type boolean
		 *  @access readonly
		 **/
		wantProperty<bool>(VehicleProperty::Horn,"On","b",AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

/** @interface Fuel : VehiclePropertyType **/
class FuelProperty: public DBusSink
{
public:
	FuelProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
		:DBusSink("org.automotive.Fuel", "/org/automotive/runningstatus/Fuel", re, connection, map<string, string>())
	{
		/** @attributeName Level
		 *  @type unsigned short
		 *  @access readonly
		 **/
		wantProperty<uint16_t>(VehicleProperty::FuelLevel,"Level", "y", AbstractProperty::Read);

		/** @attributeName Range
		 *  @type unsigned short
		 *  @access readonly
		 **/
		wantProperty<uint16_t>(VehicleProperty::FuelRange,"Range", "q", AbstractProperty::Read);

		/** @attributeName InstantConsumption
		 *  @type unsigned short
		 *  @access readonly
		 **/
		wantProperty<uint16_t>(VehicleProperty::FuelConsumption,"InstantConsumption", "q", AbstractProperty::Read);

		/** @attributeName InstantEconomy
		 *  @type unsigned short
		 *  @access readonly
		 **/
		wantProperty<uint16_t>(VehicleProperty::FuelEconomy,"InstantEconomy", "q", AbstractProperty::Read);

		/** @attributeName AverageEconomy
		 *  @type unsigned short
		 *  setting this to any value should reset the counter to '0'
		 **/
		wantProperty<uint16_t>(VehicleProperty::FuelAverageEconomy,"AverageEconomy", "q", AbstractProperty::ReadWrite);
		supportedChanged(re->supported());
	}
};

/** @interface EngineOil : VehiclePropertyType **/
class EngineOilProperty: public DBusSink
{
public:
	EngineOilProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("org.automotive.EngineOil", "/org/automotive/runningstatus/EngineOil", re, connection, map<string, string>())
	{
		/** @attributeName Remaining
		 *  @type unsigned short
		 *  @access readonly
		 **/
		wantProperty<uint16_t>(VehicleProperty::EngineOilRemaining, "Remaining", "q", AbstractProperty::Read);

		/** @attributeName Temperature
		 *  @type long
		 *  @access readonly
		 **/
		wantProperty<int>(VehicleProperty::EngineOilTemperature, "Temperature", "i", AbstractProperty::Read);

		/** @attributeName Pressure
		 *  @type unsigned short
		 *  @access readonly
		 **/
		wantProperty<uint16_t>(VehicleProperty::EngineOilPressure, "Pressure", "q", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

/** @interface Location : VehiclePropertyType **/
class LocationProperty: public DBusSink
{
public:
	LocationProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("org.automotive.Location", "/org/automotive/runningstatus/Location", re, connection, map<string, string>())
	{
		/** @attributeName Latitude
		 *  @type double
		 *  @access readonly
		 **/
		wantProperty<double>(VehicleProperty::Latitude, "Latitude", "d", AbstractProperty::Read);

		/** @attributeName Longitude
		 *  @type double
		 *  @access readonly
		 **/
		wantProperty<double>(VehicleProperty::Longitude, "Longitude", "d", AbstractProperty::Read);

		/** @attributeName Altitude
		 *  @type double
		 *  @access readonly
		 **/
		wantProperty<double>(VehicleProperty::Altitude, "Altitude", "d", AbstractProperty::Read);

		/** @attributeName Direction
		 *  @type unsigned short
		 *  @access readonly
		 **/
		wantProperty<uint16_t>(VehicleProperty::Direction, "Direction", "q", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

#endif
