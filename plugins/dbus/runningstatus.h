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

/** @interface: EngineSpeedProperty **/
class VehicleSpeedProperty: public DBusSink
{
public:
	VehicleSpeedProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.vehicleSpeed","/org/automotive/runningstatus/vehicleSpeed", re, connection, map<string, string>())
	{
		/** @attributeName EngineSpeed
		 *  @type unsigned short
		 *  @access readonly
		 **/
		wantProperty<uint16_t>(VehicleProperty::VehicleSpeed,"VehicleSpeed", "i", AbstractProperty::Read);
		supportedChanged(re->supported());
	}


};

/** @interface: EngineSpeedProperty **/
class EngineSpeedProperty: public DBusSink
{
public:
	EngineSpeedProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.engineSpeed","/org/automotive/runningstatus/engineSpeed", re, connection, map<string, string>())
	{
		/** @attributeName EngineSpeed
		 *  @type unsigned short
		 *  @access readonly
		 **/
		wantProperty<uint16_t>(VehicleProperty::EngineSpeed,"EngineSpeed", "i", AbstractProperty::Read);
		supportedChanged(re->supported());
	}


};

class VehiclePowerModeProperty: public DBusSink
{
public:
	VehiclePowerModeProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.vehiclePowerMode","/org/automotive/runningstatus/vehiclePowerMode", re, connection, map<string, string>())
	{
		wantProperty<Power::PowerModes>(VehicleProperty::VehiclePowerMode, "VehiclePowerMode","b",AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

class TripMeterProperty: public DBusSink
{
public:
	TripMeterProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
	:DBusSink("org.automotive.tripMeter","/org/automotive/runningstatus/tripMeter", re, connection, map<string, string>())
	{
		wantProperty<uint16_t>(VehicleProperty::TripMeterA, "A", "q", AbstractProperty::ReadWrite);
		wantProperty<uint16_t>(VehicleProperty::TripMeterB, "B", "q", AbstractProperty::ReadWrite);
		wantProperty<uint16_t>(VehicleProperty::TripMeterC, "C", "q", AbstractProperty::ReadWrite);
		supportedChanged(re->supported());
	}
};

class AccelerationProperty: public DBusSink
{
public:
	AccelerationProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
	:DBusSink("org.automotive.acceleration","/org/automotive/runningstatus/acceleration", re, connection, map<string, string>())
	{
		wantProperty<uint16_t>(VehicleProperty::AccelerationX, "X", "q", AbstractProperty::Read);
		wantProperty<uint16_t>(VehicleProperty::AccelerationY, "Y", "q", AbstractProperty::Read);
		wantProperty<uint16_t>(VehicleProperty::AccelerationZ, "Z", "q", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

class TransmissionProperty: public DBusSink
{
public:
	TransmissionProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
	:DBusSink("org.automotive.transmission","/org/automotive/runningstatus/transmission", re, connection, map<string, string>())
	{
		wantProperty<Transmission::TransmissionPositions>(VehicleProperty::TransmissionShiftPosition,
														  "ShiftPosition", "y", AbstractProperty::Read);
		wantProperty<Transmission::TransmissionPositions>(VehicleProperty::TransmissionGearPosition,
														  "GearPosition", "y", AbstractProperty::Read);
		wantProperty<Transmission::Mode>(VehicleProperty::TransmissionMode,
														  "Mode", "y", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};


class CruiseControlProperty: public DBusSink
{
public:
	CruiseControlProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
	:DBusSink("org.automotive.cruiseControlStatus","/org/automotive/runningstatus/cruiseControlStatus", re, connection, map<string, string>())
	{
		wantProperty<bool>(VehicleProperty::CruiseControlActive, "Activated", "b", AbstractProperty::Read);
		wantProperty<uint16_t>(VehicleProperty::CruiseControlSpeed, "Speed", "q", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

class WheelBrakeProperty: public DBusSink
{
public:
	WheelBrakeProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
		:DBusSink("org.automotive.wheelBrake","/org/automotive/runningstatus/wheelBrake", re, connection, map<string, string>())
	{
		wantProperty<bool>(VehicleProperty::WheelBrake, "Engaged", "b", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};
class LightStatusProperty: public DBusSink
{
public:
	LightStatusProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
		:DBusSink("org.automotive.lightStatus","/org/automotive/runningstatus/lightStatus", re, connection, map<string, string>())
	{
		wantProperty<bool>(VehicleProperty::LightHead, "Head", "b", AbstractProperty::Read);
		wantProperty<bool>(VehicleProperty::LightRightTurn, "RightTurn", "b", AbstractProperty::Read);
		wantProperty<bool>(VehicleProperty::LightLeftTurn, "LeftTurn", "b", AbstractProperty::Read);
		wantProperty<bool>(VehicleProperty::LightBrake, "Brake", "b", AbstractProperty::Read);
		wantProperty<bool>(VehicleProperty::LightFog, "Fog", "b", AbstractProperty::Read);
		wantProperty<bool>(VehicleProperty::LightHazard, "Hazard", "b", AbstractProperty::Read);
		wantProperty<bool>(VehicleProperty::LightParking, "Parking", "b", AbstractProperty::Read);
		wantProperty<bool>(VehicleProperty::LightHighBeam, "HighBeam", "b", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

class InteriorLightStatusProperty: public DBusSink
{
public:
	InteriorLightStatusProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
		:DBusSink("org.automotive.interiorLightStatus","/org/automotive/runningstatus/interiorLightStatus", re, connection, map<string, string>())
	{
		wantProperty<bool>(VehicleProperty::InteriorLightPassenger, "Passenger", "b", AbstractProperty::Read);
		wantProperty<bool>(VehicleProperty::InteriorLightPassenger, "Driver", "b", AbstractProperty::Read);
		wantProperty<bool>(VehicleProperty::InteriorLightCenter, "Center", "b", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

class HornProperty: public DBusSink
{
public:
	HornProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
		:DBusSink("org.automotive.horn","/org/automotive/runningstatus/horn", re, connection, map<string, string>())
	{
		wantProperty<bool>(VehicleProperty::Horn,"On","b",AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

class FuelProperty: public DBusSink
{
public:
	FuelProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
		:DBusSink("org.automotive.fuel", "/org/automotive/runningstatus/fuel", re, connection, map<string, string>())
	{
		wantProperty<uint16_t>(VehicleProperty::FuelLevel,"Level", "y", AbstractProperty::Read);
		wantProperty<uint16_t>(VehicleProperty::FuelRange,"Range", "q", AbstractProperty::Read);
		wantProperty<uint16_t>(VehicleProperty::FuelConsumption,"InstantConsumption", "q", AbstractProperty::Read);
		wantProperty<uint16_t>(VehicleProperty::FuelEconomy,"InstantEconomy", "q", AbstractProperty::Read);
		wantProperty<uint16_t>(VehicleProperty::FuelAverageEconomy,"AverageEconomy", "q", AbstractProperty::ReadWrite);
		supportedChanged(re->supported());
	}
};

class EngineOilProperty: public DBusSink
{
public:
	EngineOilProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("org.automotive.engineOil", "/org/automotive/runningstatus/engineOil", re, connection, map<string, string>())
	{
		wantProperty<uint16_t>(VehicleProperty::EngineOilRemaining, "Remaining", "y", AbstractProperty::Read);
		wantProperty<int>(VehicleProperty::EngineOilTemperature, "Temperature", "i", AbstractProperty::Read);
		wantProperty<uint16_t>(VehicleProperty::EngineOilPressure, "Pressure", "y", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

class LocationProperty: public DBusSink
{
public:
	LocationProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
			:DBusSink("org.automotive.location", "/org/automotive/runningstatus/location", re, connection, map<string, string>())
	{
		wantProperty<double>(VehicleProperty::Latitude, "Latitude", "d", AbstractProperty::Read);
		wantProperty<double>(VehicleProperty::Longitude, "Longitude", "d", AbstractProperty::Read);
		wantProperty<double>(VehicleProperty::Altitude, "Altitude", "d", AbstractProperty::Read);
		wantProperty<uint>(VehicleProperty::Direction, "Direction", "y", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

#endif
