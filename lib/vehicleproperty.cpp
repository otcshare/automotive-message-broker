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
#include <map>

using namespace std;

const VehicleProperty::Property VehicleProperty::NoValue = "NoValue";
const VehicleProperty::Property VehicleProperty::VehicleSpeed = "VehicleSpeed";
const VehicleProperty::Property VehicleProperty::EngineSpeed = "EngineSpeed";
const VehicleProperty::Property VehicleProperty::TransmissionShiftPosition = "TransmissionShiftPosition";
const VehicleProperty::Property VehicleProperty::TransmissionGearPosition = "TransmissionGearPostion";
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
const VehicleProperty::Property VehicleProperty::MassAirFlow = "MassAirFlow";
const VehicleProperty::Property VehicleProperty::ButtonEvent = "ButtonEvent";
const VehicleProperty::Property VehicleProperty::AirIntakeTemperature = "AirIntakeTemperature";
const VehicleProperty::Property VehicleProperty::BatteryVoltage = "BatteryVoltage";
const VehicleProperty::Property VehicleProperty::InteriorTemperature = "InteriorTemperature";
const VehicleProperty::Property VehicleProperty::EngineOilTemperature = "EngineOilTemperature";
const VehicleProperty::Property VehicleProperty::VIN = "VIN";
const VehicleProperty::Property VehicleProperty::WMI = "WMI";
const VehicleProperty::Property VehicleProperty::TirePressureLeftFront = "TirePressureLeftFront";
const VehicleProperty::Property VehicleProperty::TirePressureRightFront = "TirePressureRightFront";
const VehicleProperty::Property VehicleProperty::TirePressureLeftRear = "TirePressureLeftRear";
const VehicleProperty::Property VehicleProperty::TirePressureRightRear = "TirePressureRightRear";

VehicleProperty::VehicleProperty()
{

}

std::list<VehicleProperty::Property> VehicleProperty::capabilities()
{
	PropertyList mProperties;

	mProperties.push_back(VehicleSpeed);
	mProperties.push_back(EngineSpeed);
	mProperties.push_back(TransmissionShiftPosition);
	mProperties.push_back(TransmissionGearPosition);
	mProperties.push_back(ThrottlePosition);
	mProperties.push_back(WheelBrake);
	mProperties.push_back(SteeringWheelAngle);
	mProperties.push_back(TurnSignal);
	mProperties.push_back(ClutchStatus);
	mProperties.push_back((EngineOilPressure));
	mProperties.push_back(EngineCoolantTemperature);
	mProperties.push_back(AccelerationX);
	mProperties.push_back(AccelerationY);
	mProperties.push_back(AccelerationZ);
	mProperties.push_back(MassAirFlow);
	mProperties.push_back(ButtonEvent);
	mProperties.push_back(AirIntakeTemperature);
	mProperties.push_back(BatteryVoltage);
	mProperties.push_back(InteriorTemperature);
	mProperties.push_back(EngineOilTemperature);
	mProperties.push_back(MachineGunTurretStatus);
	mProperties.push_back(VIN);
	mProperties.push_back(WMI);
	mProperties.push_back(TirePressureLeftFront);
	mProperties.push_back(TirePressureRightFront);
	mProperties.push_back(TirePressureLeftRear);
	mProperties.push_back(TirePressureRightRear);

	return mProperties;
}

AbstractPropertyType* VehicleProperty::getPropertyTypeForPropertyNameValue(VehicleProperty::Property name, std::string value)
{

	if(name == VehicleSpeed ) return new VehicleSpeedType(value);
	else if(name == EngineSpeed) return new EngineSpeedType(value);
	else if(name == TransmissionShiftPosition) return new TransmissionShiftPositionType(value);
	else if(name == TransmissionGearPosition) return new TransmissionGearPositionType(value);
	else if(name == ThrottlePosition) return new ThrottlePositionType(value);
	else if(name == WheelBrake) return new WheelBrakeType(value);
	else if(name == SteeringWheelAngle) return new SteeringWheelAngleType(value);
	else if(name == TurnSignal) return new TurnSignalType(value);
	else if(name == ClutchStatus) return new ClutchStatusType(value);
	else if(name == EngineOilPressure) return new EngineOilPressureType(value);
	else if(name == EngineCoolantTemperature) return new EngineCoolantTemperatureType(value);
	else if(name == AccelerationX) return new AccelerationType(value);
	else if(name == AccelerationY) return new AccelerationType(value);
	else if(name == AccelerationZ) return new AccelerationType(value);
	else if(name == MassAirFlow) return new MassAirFlowType(value);
	else if(name == ButtonEvent) return new ButtonEventType(value);
	else if(name == AirIntakeTemperature) return new AirIntakeTemperatureType(value);
	else if(name == BatteryVoltage) return new BatteryVoltageType(value);
	else if(name == InteriorTemperature) return new InteriorTemperatureType(value);
	else if(name == EngineOilTemperature) return new EngineOilTemperatureType(value);
	else if(name == VIN) return new VINType(value);
	else if(name == WMI) return new WMIType(value);
	else if(name == TirePressureLeftFront) return new TirePressureType(value);
	else if(name == TirePressureRightFront) return new TirePressureType(value);
	else if(name == TirePressureLeftRear) return new TirePressureType(value);
	else if(name == TirePressureRightRear) return new TirePressureType(value);


	return nullptr;
}
