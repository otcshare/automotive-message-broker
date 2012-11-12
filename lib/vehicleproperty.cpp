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

#define REGISTERPROPERTY(property, defaultValue) \
	registerProperty(property, []() { return new property ## Type(defaultValue); });

#define REGISTERPROPERTYWITHTYPE(property, type, defaultValue) \
	registerProperty(property, []() { return new type(defaultValue); });

using namespace std;

std::map<VehicleProperty::Property, VehicleProperty::PropertyTypeFactoryCallback> VehicleProperty::registeredPropertyFactoryMap;

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
const VehicleProperty::Property VehicleProperty::VehiclePowerMode = "VehiclePowerMode";
const VehicleProperty::Property VehicleProperty::TripMeterA = "TripMeterA";
const VehicleProperty::Property VehicleProperty::TripMeterB = "TripMeterB";
const VehicleProperty::Property VehicleProperty::TripMeterC = "TripMeterC";

std::list<VehicleProperty::Property> VehicleProperty::mCapabilities;

VehicleProperty::VehicleProperty()
{
	registerProperty( VehicleSpeed, [](){ return new VehicleSpeedType(0); });
	registerProperty(EngineSpeed, [](){ return new EngineSpeedType(0); });
	registerProperty(TransmissionShiftPosition, [](){ return new TransmissionShiftPositionType(Transmission::Neutral); });
	registerProperty(TransmissionGearPosition, [](){ return new TransmissionGearPositionType(Transmission::Neutral); });
	registerProperty(ThrottlePosition, [](){ return new ThrottlePositionType(0); });
	registerProperty(WheelBrake, [](){ return new WheelBrakeType(false); });
	registerProperty(SteeringWheelAngle, [](){ return new SteeringWheelAngleType(0); });
	registerProperty(TurnSignal, [](){ return new TurnSignalType(TurnSignals::Off); });
	registerProperty(ClutchStatus, [](){ return new ClutchStatusType(false); });
	registerProperty(EngineOilPressure, [](){ return new EngineOilPressureType(0); });
	registerProperty(EngineCoolantTemperature, [](){ return new EngineCoolantTemperatureType(0); });
	registerProperty(MachineGunTurretStatus, [](){ return new MachineGunTurretStatusType(false); });
	registerProperty(AccelerationX, [](){ return new AccelerationType(0); });
	registerProperty(AccelerationY, [](){ return new AccelerationType(0); });
	registerProperty(AccelerationZ, [](){ return new AccelerationType(0); });
	registerProperty(MassAirFlow, [](){ return new MassAirFlowType(0); });
	registerProperty(ButtonEvent, [](){ return new ButtonEventType(ButtonEvents::NoButton); });
	registerProperty(BatteryVoltage, [](){ return new BatteryVoltageType(0); });
	registerProperty(InteriorTemperature, [](){ return new InteriorTemperatureType(0); });
	registerProperty(EngineOilTemperature, [](){ return new EngineOilTemperatureType(0); });
	registerProperty(VIN, [](){ return new VINType(""); });
	registerProperty(WMI, [](){ return new WMIType(""); });
	REGISTERPROPERTYWITHTYPE(TirePressureLeftFront, TirePressureType, 0);
	REGISTERPROPERTYWITHTYPE(TirePressureRightFront, TirePressureType, 0);
	REGISTERPROPERTYWITHTYPE(TirePressureLeftRear, TirePressureType, 0);
	REGISTERPROPERTYWITHTYPE(TirePressureRightRear, TirePressureType, 0);
	registerProperty( VehiclePowerMode,[](){ return new VehiclePowerModeType(Power::Off); } );
	REGISTERPROPERTYWITHTYPE(TripMeterA,TripMeterType,0);
	REGISTERPROPERTYWITHTYPE(TripMeterB,TripMeterType,0);
	REGISTERPROPERTYWITHTYPE(TripMeterC,TripMeterType,0);
}

std::list<VehicleProperty::Property> VehicleProperty::capabilities()
{
	return mCapabilities;
}

AbstractPropertyType* VehicleProperty::getPropertyTypeForPropertyNameValue(VehicleProperty::Property name, std::string value)
{

	if(registeredPropertyFactoryMap.count(name) > 0)
	{
		VehicleProperty::PropertyTypeFactoryCallback cb = registeredPropertyFactoryMap[name];
		if ( cb != NULL )
		{
			AbstractPropertyType* type = cb();
			if(type == NULL)
				throw std::runtime_error("Cannot return NULL in a PropertyTypeFactory");

			type->fromString(value);

			return type;
		}

	}

	return nullptr;
}

void VehicleProperty::registerProperty(VehicleProperty::Property name, VehicleProperty::PropertyTypeFactoryCallback factory)
{
	registeredPropertyFactoryMap[name] = factory;
	mCapabilities.push_back(name);
}

VehicleProperty vehiclePropertyConstruct;
