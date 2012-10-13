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

class VehicleSpeedProperty: public DBusSink
{
public:
	VehicleSpeedProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.vehicleSpeed","/org/automotive/runningstatus/vehicleSpeed", re, connection, map<string, string>())
	{
		wantProperty<uint16_t>(VehicleProperty::VehicleSpeed,"VehicleSpeed", "i", AbstractProperty::Read);
		supportedChanged(re->supported());
	}


};

class TirePressureProperty: public DBusSink
{
public:
	TirePressureProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.tirePressure","/org/automotive/maintainance/tirePressure", re, connection, map<string, string>())
	{
		wantProperty<uint16_t>(VehicleProperty::TirePressureLeftFront,"LeftFront", "q", AbstractProperty::Read);
		wantProperty<uint16_t>(VehicleProperty::TirePressureRightFront,"RightFront", "q", AbstractProperty::Read);
		wantProperty<uint16_t>(VehicleProperty::TirePressureLeftRear,"LeftRear", "q", AbstractProperty::Read);
		wantProperty<uint16_t>(VehicleProperty::TirePressureRightRear,"RightRear", "q", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

class VehiclePowerModeProperty: public DBusSink
{
public:
	VehiclePowerModeProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.vehiclePowerMode","/org/automotive/runningstatus/vehiclePowerMode", re, connection, map<string, string>())
	{
		wantProperty<uint16_t>(VehicleProperty::VehiclePowerMode, "VehiclePowerMode","b",AbstractProperty::Read);
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
		wantProperty<Transmission::TransmissionPositions>(VehicleProperty::TransmissionMode,
														  "Mode", "y", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

class TireTemperatureProperty: public DBusSink
{
public:
	TireTemperatureProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.tireTemperature","/org/automotive/maintainance/tireTemperature", re, connection, map<string, string>())
	{
		wantProperty<uint16_t>(VehicleProperty::TireTemperatureLeftFront,"LeftFront", "i", AbstractProperty::Read);
		wantProperty<uint16_t>(VehicleProperty::TireTemperatureRightFront,"RightFront", "i", AbstractProperty::Read);
		wantProperty<uint16_t>(VehicleProperty::TireTemperatureLeftRear,"LeftRear", "i", AbstractProperty::Read);
		wantProperty<uint16_t>(VehicleProperty::TireTemperatureRightRear,"RightRear", "i", AbstractProperty::Read);
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

#endif
