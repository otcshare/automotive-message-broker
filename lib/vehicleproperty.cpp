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


#include "vehicleproperty.h"

using namespace std;

 const VehicleProperty::Property VehicleProperty::NoValue = "NoValue";
 const VehicleProperty::Property VehicleProperty::VehicleSpeed = "VehicleSpeed";
 const VehicleProperty::Property VehicleProperty::EngineSpeed = "EngineSpeed";
 const VehicleProperty::Property VehicleProperty::TransmissionShiftPosition = "TransmissionShiftPosition";
 const VehicleProperty::Property VehicleProperty::ThrottlePosition = "ThrottlePosition";
 const VehicleProperty::Property VehicleProperty::WheelBrake = "WheelBrake";
 const VehicleProperty::Property VehicleProperty::SteeringWheelAngle = "SteeringWheelAngle";
 const VehicleProperty::Property VehicleProperty::TurnSignal = "TurnSignal";
 const VehicleProperty::Property VehicleProperty::ClutchStatus = "ClutchStatus";
 const VehicleProperty::Property VehicleProperty::EngineOilPressure = "EngineOilPressure";
 const VehicleProperty::Property VehicleProperty::EngineCoolantTemperature = "EngineCoolantTemperature";
 const VehicleProperty::Property VehicleProperty::MachineGunTurretStatus = "MachineGunTurretStatus";
 const VehicleProperty::Property VehicleProperty::AccelerationX = "AccelerationX";
 const VehicleProperty::Property VehicleProperty::AccelerationY = "AccelerationY";
 const VehicleProperty::Property VehicleProperty::AccelerationZ = "AccelerationZ";

VehicleProperty::VehicleProperty()
{

}

