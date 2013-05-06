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
		 *  @type unsigned long
		 *  @access readonly
		 *  @attributeComment \brief  Must return Vehicle Speed in kilometers per hour.
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
		 *  @type unsigned long
		 *  @access readonly
		 *  @attributeComment \brief  Must return Engine Speed in rotations per minute.
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
		/** @attributeName TripMeters
		 *  @type sequence<unsigned long>
		 *  @attributeComment \brief  Must return trip metes. Changing any items in the array will reset the item's value to '0''.
		 **/
		wantPropertyVariant(VehicleProperty::TripMeters, "TripMeters", "aq", AbstractProperty::ReadWrite);

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
		 *  @attributeComment \brief  Must return acceleration on the "X" axis as 1/1000 G (gravitational force).
		 **/
		wantProperty<uint16_t>(VehicleProperty::AccelerationX, "X", "q", AbstractProperty::Read);

		/** @attributeName Y
		 *  @type unsigned short
		 *  @access readonly
		 *  @attributeComment \brief  Must return acceleration on the "Y" axis as 1/1000 G (gravitational force).
		 **/
		wantProperty<uint16_t>(VehicleProperty::AccelerationY, "Y", "q", AbstractProperty::Read);

		/** @attributeName Z
		 *  @type unsigned short
		 *  @access readonly
		 *  @attributeComment \brief  Must return acceleration on the "Z" axis as 1/1000 G (gravitational force).
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
		/**
		 * @enum const unsigned short TRANSMISSIONPOSITION_NEUTRAL = 0;
		 * @enum const unsigned short TRANSMISSIONPOSITION_FIRST = 1;
		 * @enum const unsigned short TRANSMISSIONPOSITION_SECOND = 2;
		 * @enum const unsigned short TRANSMISSIONPOSITION_THIRD = 3;
		 * @enum const unsigned short TRANSMISSIONPOSITION_FORTH = 4;
		 * @enum const unsigned short TRANSMISSIONPOSITION_FIFTH = 5;
		 * @enum const unsigned short TRANSMISSIONPOSITION_SIXTH = 6;
		 * @enum const unsigned short TRANSMISSIONPOSITION_SEVENTH = 7;
		 * @enum const unsigned short TRANSMISSIONPOSITION_EIGHTH = 8;
		 * @enum const unsigned short TRANSMISSIONPOSITION_NINTH = 9;
		 * @enum const unsigned short TRANSMISSIONPOSITION_TENTH = 10;
		 * @enum const unsigned short TRANSMISSIONPOSITION_CVT = 64;
		 * @enum const unsigned short TRANSMISSIONPOSITION_REVERSE = 128;
		 * @enum const unsigned short TRANSMISSIONPOSITION_PARK = 255;
		 **/



		/** @ attributeName ShiftPosition
		 *  @ type octet
		 *  @ access readonly
		 *  @ attributeComment \brief  Must return transmission shift position
		 **/
		//wantProperty<Transmission::TransmissionPositions>(VehicleProperty::TransmissionShiftPosition,
			//											  "ShiftPosition", "y", AbstractProperty::Read);

		/** @attributeName GearPosition
		 *  @type octet
		 *  @access readonly
		 *  @attributeComment \brief  Must return transmission gear position (see TRANSMISSIONPOSITION)
		 **/
		wantProperty<Transmission::TransmissionPositions>(VehicleProperty::TransmissionGearPosition,
														  "GearPosition", "y", AbstractProperty::Read);

		/**
		 * @enum const unsigned short TRANSMISSIONMODE_NORMAL = 0;
		 * @enum const unsigned short TRANSMISSIONMODE_SPORT = 1;
		 * @enum const unsigned short TRANSMISSIONMODE_ECONOMY = 2;
		 * @enum const unsigned short TRANSMISSIONMODE_OEMCUSTOM1 = 3;
		 * @enum const unsigned short TRANSMISSIONMODE_OEMCUSTOM2 = 4;
		 **/

		/** @attributeName Mode
		 *  @type octet
		 *  @attributeComment \brief  Must return transmission Mode (see TRANSMISSIONMODE_*)
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
		 *  @attributeComment \brief  Must return whether or not the Cruise Control system is active (true) or inactive (false)
		 **/
		wantProperty<bool>(VehicleProperty::CruiseControlActive, "Activated", "b", AbstractProperty::Read);

		/** @attributeName Speed
		 *  @type unsigned short
		 *  @access readonly
		 *  @attributeComment \brief  Must return target Cruise Control speed in kilometers per hour (kph).
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
		 *  @attributeComment \brief  Must return Wheel Brake status: Engaged = true, disengaged = false
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
		 *  @attributeComment \brief  Must return headlight status: on = true, off = false.
		 **/
		wantProperty<bool>(VehicleProperty::LightHead, "Head", "b", AbstractProperty::Read);

		/** @attributeName RightTurn
		 *  @type boolean
		 *  @access readonly
		 *  @attributeComment \brief  Must return right turn signal status: on = true, off = false.
		 **/
		wantProperty<bool>(VehicleProperty::LightRightTurn, "RightTurn", "b", AbstractProperty::Read);

		/** @attributeName LeftTurn
		 *  @type boolean
		 *  @access readonly
		 *  @attributeComment \brief  Must return left turn signal status: on = true, off = false.
		 **/
		wantProperty<bool>(VehicleProperty::LightLeftTurn, "LeftTurn", "b", AbstractProperty::Read);

		/** @attributeName Brake
		 *  @type boolean
		 *  @access readonly
		 *  @attributeComment \brief  Must return  brake signal light status: on = true, off = false.
		 **/
		wantProperty<bool>(VehicleProperty::LightBrake, "Brake", "b", AbstractProperty::Read);

		/** @attributeName Fog
		 *  @type boolean
		 *  @access readonly
		 *  @attributeComment \brief  Must return fog light status: on = true, off = false.
		 **/
		wantProperty<bool>(VehicleProperty::LightFog, "Fog", "b", AbstractProperty::Read);

		/** @attributeName Hazard
		 *  @type boolean
		 *  @access readonly
		 *  @attributeComment \brief  Must return hazard light status: on = true, off = false.
		 **/
		wantProperty<bool>(VehicleProperty::LightHazard, "Hazard", "b", AbstractProperty::Read);

		/** @attributeName Parking
		 *  @type boolean
		 *  @access readonly
		 *  @attributeComment \brief  Must return parking light status: on = true, off = false.
		 **/
		wantProperty<bool>(VehicleProperty::LightParking, "Parking", "b", AbstractProperty::Read);

		/** @attributeName HighBeam
		 *  @type boolean
		 *  @access readonly
		 *  @attributeComment \brief  Must return high beam status: on = true, off = false.
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
		 *  @attributeComment \brief  Must return passenger interior light status: on = true, off = false
		 **/
		wantProperty<bool>(VehicleProperty::InteriorLightPassenger, "Passenger", "b", AbstractProperty::Read);

		/** @attributeName Driver
		 *  @type boolean
		 *  @access readonly
		 *  @attributeComment \brief  Must return driver interior light status: on = true, off = false
		 **/
		wantProperty<bool>(VehicleProperty::InteriorLightPassenger, "Driver", "b", AbstractProperty::Read);

		/** @attributeName Center
		 *  @type boolean
		 *  @access readonly
		 *  @attributeComment \brief  Must return center interior light status: on = true, off = false
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
		 *  @attributeComment \brief  Must return horn status: on = true, off = false
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
		 *  @attributeComment \brief  Must return fuel level as a percentage of fullness.
		 **/
		wantProperty<uint16_t>(VehicleProperty::FuelLevel,"Level", "y", AbstractProperty::Read);

		/** @attributeName Range
		 *  @type unsigned short
		 *  @access readonly
		 *  @attributeComment \brief  Must return estimated fuel range in kilometers.
		 **/
		wantProperty<uint16_t>(VehicleProperty::FuelRange,"Range", "q", AbstractProperty::Read);

		/** @attributeName InstantConsumption
		 *  @type unsigned short
		 *  @access readonly
		 *  @attributeComment \brief  Must return instant fuel consumption in milliliters of fuel per second.
		 **/
		wantProperty<uint16_t>(VehicleProperty::FuelConsumption,"InstantConsumption", "q", AbstractProperty::Read);

		/** @attributeName InstantEconomy
		 *  @type unsigned short
		 *  @access readonly
		 *  @attributeComment \brief  Must return instant fuel 'economy' in kilometers per liter of fuel.
		 **/
		wantProperty<uint16_t>(VehicleProperty::FuelEconomy,"InstantEconomy", "q", AbstractProperty::Read);

		/** @attributeName AverageEconomy
		 *  @type unsigned short
		 *  @attributeComment \brief  Must return average fuel 'economy' in kilometers per liter of fuel since last reset.  Setting this to any value should reset the counter to '0'
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
		 *  @attributeComment \brief  Must return remaining engine oil as percentage of fullness.
		 **/
		wantProperty<uint16_t>(VehicleProperty::EngineOilRemaining, "Remaining", "q", AbstractProperty::Read);

		/** @attributeName Temperature
		 *  @type long
		 *  @access readonly
		 *  @attributeComment \brief  Must return Engine Oil Temperature in Celcius.
		 **/
		wantProperty<int>(VehicleProperty::EngineOilTemperature, "Temperature", "i", AbstractProperty::Read);

		/** @attributeName Pressure
		 *  @type unsigned short
		 *  @access readonly
		 *  @attributeComment \brief  Must return Engine Oil Pressure in kPa.
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
		supportedChanged(re->supported());
	}
};

#endif
