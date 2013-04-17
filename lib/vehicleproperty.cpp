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
#include "listplusplus.h"
#include "debugout.h"
#include "mappropertytype.hpp"
#include <map>


#define REGISTERPROPERTY(property, defaultValue) \
	registerPropertyPriv(property, []() { return new property ## Type(defaultValue); });

#define REGISTERPROPERTYWITHTYPE(property, type, defaultValue) \
	registerPropertyPriv(property, []() { return new type(defaultValue); });

using namespace std;

std::map<VehicleProperty::Property, VehicleProperty::PropertyTypeFactoryCallback> VehicleProperty::registeredPropertyFactoryMap;

VehicleProperty* VehicleProperty::thereCanOnlyBeOne = nullptr;

const VehicleProperty::Property VehicleProperty::NoValue = "NoValue";
const VehicleProperty::Property VehicleProperty::VehicleSpeed = "VehicleSpeed";
const VehicleProperty::Property VehicleProperty::EngineSpeed = "EngineSpeed";
const VehicleProperty::Property VehicleProperty::TransmissionShiftPosition = "TransmissionShiftPosition";
const VehicleProperty::Property VehicleProperty::TransmissionGearPosition = "TransmissionGearPostion";
const VehicleProperty::Property VehicleProperty::TransmissionMode = "TransmissionMode";
const VehicleProperty::Property VehicleProperty::ThrottlePosition = "ThrottlePosition";
const VehicleProperty::Property VehicleProperty::WheelBrake = "WheelBrake";
const VehicleProperty::Property VehicleProperty::WheelBrakePressure = "WheelBrakePressure";
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
const VehicleProperty::Property VehicleProperty::BatteryCurrent = "BatteryCurrent";
const VehicleProperty::Property VehicleProperty::InteriorTemperature = "InteriorTemperature";
const VehicleProperty::Property VehicleProperty::EngineOilTemperature = "EngineOilTemperature";
const VehicleProperty::Property VehicleProperty::VIN = "VIN";
const VehicleProperty::Property VehicleProperty::WMI = "WMI";
const VehicleProperty::Property VehicleProperty::TirePressureLeftFront = "TirePressureLeftFront";
const VehicleProperty::Property VehicleProperty::TirePressureRightFront = "TirePressureRightFront";
const VehicleProperty::Property VehicleProperty::TirePressureLeftRear = "TirePressureLeftRear";
const VehicleProperty::Property VehicleProperty::TirePressureRightRear = "TirePressureRightRear";
const VehicleProperty::Property VehicleProperty::TireTemperatureLeftFront = "TireTemperatureLeftFront";
const VehicleProperty::Property VehicleProperty::TireTemperatureRightFront = "TireTemperatureRightFront";
const VehicleProperty::Property VehicleProperty::TireTemperatureLeftRear = "TireTemperatureLeftRear";
const VehicleProperty::Property VehicleProperty::TireTemperatureRightRear = "TireTemperatureRightRear";
const VehicleProperty::Property VehicleProperty::VehiclePowerMode = "VehiclePowerMode";
const VehicleProperty::Property VehicleProperty::TripMeterA = "TripMeterA";
const VehicleProperty::Property VehicleProperty::TripMeterB = "TripMeterB";
const VehicleProperty::Property VehicleProperty::TripMeterC = "TripMeterC";
const VehicleProperty::Property VehicleProperty::CruiseControlActive = "CruiseControlActive";
const VehicleProperty::Property VehicleProperty::CruiseControlSpeed = "CruiseControlSpeed";
const VehicleProperty::Property VehicleProperty::LightHead = "LightHead";
const VehicleProperty::Property VehicleProperty::LightLeftTurn = "LightLeftTurn";
const VehicleProperty::Property VehicleProperty::LightRightTurn = "LightRightTurn";
const VehicleProperty::Property VehicleProperty::LightParking = "LightParking";
const VehicleProperty::Property VehicleProperty::LightFog = "LightFog";
const VehicleProperty::Property VehicleProperty::LightHazard= "LightHazard";
const VehicleProperty::Property VehicleProperty::LightHighBeam = "LightHighBeam";
const VehicleProperty::Property VehicleProperty::LightBrake= "LightBrake";
const VehicleProperty::Property VehicleProperty::InteriorLightDriver = "InteriorLightDriver";
const VehicleProperty::Property VehicleProperty::InteriorLightCenter = "InteriorLightCenter";
const VehicleProperty::Property VehicleProperty::InteriorLightPassenger = "InteriorLightPassenger";
const VehicleProperty::Property VehicleProperty::EngineLoad = "EngineLoad";
const VehicleProperty::Property VehicleProperty::Horn = "Horn";
const VehicleProperty::Property VehicleProperty::FuelLevel = "FuelLevel";
const VehicleProperty::Property VehicleProperty::FuelConsumption = "FuelConsumption";
const VehicleProperty::Property VehicleProperty::FuelRange = "FuelRange";
const VehicleProperty::Property VehicleProperty::FuelEconomy = "FuelEconomy";
const VehicleProperty::Property VehicleProperty::FuelAverageEconomy = "FuelAverageEconomy";
const VehicleProperty::Property VehicleProperty::FuelType = "FuelType";
const VehicleProperty::Property VehicleProperty::FuelPositionSide = "FuelPositionSide";
const VehicleProperty::Property VehicleProperty::EngineOilRemaining = "EngineOilRemaining";
const VehicleProperty::Property VehicleProperty::ExteriorBrightness = "ExteriorBrightness";
const VehicleProperty::Property VehicleProperty::Latitude = "Latitude";
const VehicleProperty::Property VehicleProperty::Longitude = "Longitude";
const VehicleProperty::Property VehicleProperty::Direction = "Direction";
const VehicleProperty::Property VehicleProperty::Altitude = "Altitude";
const VehicleProperty::Property VehicleProperty::VehicleWidth = "VehicleWidth";
const VehicleProperty::Property VehicleProperty::VehicleHeight = "VehicleHeight";
const VehicleProperty::Property VehicleProperty::VehicleLength = "VehicleLength";
const VehicleProperty::Property VehicleProperty::VehicleType = "VehicleType";
const VehicleProperty::Property VehicleProperty::DoorsPerRow = "DoorsPerRow";
const VehicleProperty::Property VehicleProperty::TransmissionGearType = "TransmissionGearType";
const VehicleProperty::Property VehicleProperty::FrontWheelRadius = "FrontWheelRadius";
const VehicleProperty::Property VehicleProperty::RearWheelRadius = "RearWheelRadius";
const VehicleProperty::Property VehicleProperty::WheelTrack = "WheelTrack";
const VehicleProperty::Property VehicleProperty::Odometer = "Odometer";
const VehicleProperty::Property VehicleProperty::TransmissionFluidLevel = "TransmissionFluidLevel";
const VehicleProperty::Property VehicleProperty::BrakeFluidLevel = "BrakeFluidLevel";
const VehicleProperty::Property VehicleProperty::WasherFluidLevel = "WasherFluidLevel";
const VehicleProperty::Property VehicleProperty::SecurityAlertStatus = "SecurityAlertStatus";
const VehicleProperty::Property VehicleProperty::ParkingBrakeStatus = "ParkingBrakeStatus";
const VehicleProperty::Property VehicleProperty::ParkingLightStatus = "ParkingLightStatus";
const VehicleProperty::Property VehicleProperty::HazardLightStatus = "HazardLightStatus";
const VehicleProperty::Property VehicleProperty::AirbagStatus = "AirbagStatus";
const VehicleProperty::Property VehicleProperty::AntilockBrakingSystem = "AntilockBrakingSystem";
const VehicleProperty::Property VehicleProperty::TractionControlSystem = "TractionControlSystem";
const VehicleProperty::Property VehicleProperty::VehicleTopSpeedLimit = "VehicleTopSpeedLimit";
const VehicleProperty::Property VehicleProperty::DoorStatus = "DoorStatus";
const VehicleProperty::Property VehicleProperty::DoorLockStatus = "DoorLockStatus";
const VehicleProperty::Property VehicleProperty::SeatBeltStatus = "SeatBeltStatus";
const VehicleProperty::Property VehicleProperty::WindowLockStatus = "WindowLockStatus";
const VehicleProperty::Property VehicleProperty::OccupantStatus = "OccupantStatus";
const VehicleProperty::Property VehicleProperty::ObstacleDistance = "ObstacleDistance";


std::list<VehicleProperty::Property> VehicleProperty::mCapabilities;
std::list<VehicleProperty::Property> VehicleProperty::mCustomProperties;

VehicleProperty::VehicleProperty()
{
	registerPropertyPriv( VehicleSpeed, [](){ return new VehicleSpeedType(0); });
	registerPropertyPriv(EngineSpeed, [](){ return new EngineSpeedType(0); });
	registerPropertyPriv(TransmissionShiftPosition, [](){ return new TransmissionShiftPositionType(Transmission::Neutral); });
	registerPropertyPriv(TransmissionGearPosition, [](){ return new TransmissionGearPositionType(Transmission::Neutral); });
	REGISTERPROPERTY(TransmissionMode,Transmission::Normal);
	registerPropertyPriv(ThrottlePosition, [](){ return new ThrottlePositionType(0); });
	registerPropertyPriv(WheelBrake, [](){ return new WheelBrakeType(false); });
	REGISTERPROPERTY(WheelBrakePressure,0);
	registerPropertyPriv(SteeringWheelAngle, [](){ return new SteeringWheelAngleType(0); });
	registerPropertyPriv(TurnSignal, [](){ return new TurnSignalType(TurnSignals::Off); });
	registerPropertyPriv(ClutchStatus, [](){ return new ClutchStatusType(false); });
	registerPropertyPriv(EngineOilPressure, [](){ return new EngineOilPressureType(0); });
	registerPropertyPriv(EngineOilTemperature, [](){ return new EngineOilTemperatureType(0); });
	REGISTERPROPERTY(EngineOilRemaining,0);
	registerPropertyPriv(EngineCoolantTemperature, [](){ return new EngineCoolantTemperatureType(0); });
	registerPropertyPriv(MachineGunTurretStatus, [](){ return new MachineGunTurretStatusType(false); });
	registerPropertyPriv(AccelerationX, [](){ return new AccelerationType(0); });
	registerPropertyPriv(AccelerationY, [](){ return new AccelerationType(0); });
	registerPropertyPriv(AccelerationZ, [](){ return new AccelerationType(0); });
	registerPropertyPriv(MassAirFlow, [](){ return new MassAirFlowType(0); });
	registerPropertyPriv(ButtonEvent, [](){ return new ButtonEventType(ButtonEvents::NoButton); });
	REGISTERPROPERTY(AirIntakeTemperature,0)
	registerPropertyPriv(BatteryVoltage, [](){ return new BatteryVoltageType(0); });
	REGISTERPROPERTY(BatteryCurrent,0);
	registerPropertyPriv(InteriorTemperature, [](){ return new InteriorTemperatureType(0); });
	registerPropertyPriv(VIN, [](){ return new VINType(""); });
	registerPropertyPriv(WMI, [](){ return new WMIType(""); });
	REGISTERPROPERTYWITHTYPE(TirePressureLeftFront, TirePressureType, 0);
	REGISTERPROPERTYWITHTYPE(TirePressureRightFront, TirePressureType, 0);
	REGISTERPROPERTYWITHTYPE(TirePressureLeftRear, TirePressureType, 0);
	REGISTERPROPERTYWITHTYPE(TirePressureRightRear, TirePressureType, 0);
	REGISTERPROPERTYWITHTYPE(TireTemperatureLeftFront,TireTemperatureType,0);
	REGISTERPROPERTYWITHTYPE(TireTemperatureRightFront,TireTemperatureType,0);
	REGISTERPROPERTYWITHTYPE(TireTemperatureLeftRear,TireTemperatureType,0);
	REGISTERPROPERTYWITHTYPE(TireTemperatureRightRear,TireTemperatureType,0);
	registerPropertyPriv( VehiclePowerMode,[](){ return new VehiclePowerModeType(Power::Off); } );
	REGISTERPROPERTYWITHTYPE(TripMeterA,TripMeterType,0);
	REGISTERPROPERTYWITHTYPE(TripMeterB,TripMeterType,0);
	REGISTERPROPERTYWITHTYPE(TripMeterC,TripMeterType,0);
	REGISTERPROPERTY(CruiseControlActive, false);
	REGISTERPROPERTY(CruiseControlSpeed,0);
	REGISTERPROPERTYWITHTYPE(LightHead, LightStatusType, false);
	REGISTERPROPERTYWITHTYPE(LightLeftTurn,LightStatusType, false);
	REGISTERPROPERTYWITHTYPE(LightRightTurn, LightStatusType, false);
	REGISTERPROPERTYWITHTYPE(LightParking, LightStatusType, false);
	REGISTERPROPERTYWITHTYPE(LightFog, LightStatusType, false);
	REGISTERPROPERTYWITHTYPE(LightHazard, LightStatusType, false);
	REGISTERPROPERTYWITHTYPE(LightHighBeam, LightStatusType, false);
	REGISTERPROPERTYWITHTYPE(LightBrake, LightStatusType, false);
	REGISTERPROPERTYWITHTYPE(InteriorLightDriver, LightStatusType, false);
	REGISTERPROPERTYWITHTYPE(InteriorLightPassenger, LightStatusType, false);
	REGISTERPROPERTYWITHTYPE(InteriorLightCenter, LightStatusType, false);
	REGISTERPROPERTY(EngineLoad,0);
	REGISTERPROPERTY(Horn, false);
	REGISTERPROPERTY(FuelLevel, 0);
	REGISTERPROPERTY(FuelRange, 0);
	REGISTERPROPERTY(FuelConsumption, 0);
	REGISTERPROPERTY(FuelEconomy, 0);
	REGISTERPROPERTY(FuelAverageEconomy, 0);
	REGISTERPROPERTY(FuelType, Fuel::Unknown);
	REGISTERPROPERTY(FuelPositionSide, Fuel::UnknownPosition);
	REGISTERPROPERTY(ExteriorBrightness, 0);
	REGISTERPROPERTYWITHTYPE(VehicleWidth,VehicleSizeType,0);
	REGISTERPROPERTYWITHTYPE(VehicleHeight,VehicleSizeType,0);
	REGISTERPROPERTYWITHTYPE(VehicleLength,VehicleSizeType,0);
	REGISTERPROPERTY(Latitude,0);
	REGISTERPROPERTY(Longitude,0);
	REGISTERPROPERTY(Altitude,0);
	REGISTERPROPERTY(Direction,0);
	REGISTERPROPERTY(VehicleType,Vehicle::Unknown);
	registerPropertyPriv(DoorsPerRow,[]() { BasicPropertyType<uint16_t> d(0); return new DoorsPerRowType(&d); });
	REGISTERPROPERTY(TransmissionGearType,Transmission::Unknown);
	REGISTERPROPERTYWITHTYPE(FrontWheelRadius, WheelRadiusType, 0);
	REGISTERPROPERTYWITHTYPE(RearWheelRadius, WheelRadiusType, 0);
	REGISTERPROPERTY(WheelTrack,0);
	REGISTERPROPERTY(Odometer,0);
	REGISTERPROPERTY(TransmissionFluidLevel,0);
	REGISTERPROPERTY(BrakeFluidLevel,0);
	REGISTERPROPERTY(WasherFluidLevel,0);
	REGISTERPROPERTY(SecurityAlertStatus,Security::Idle);
	REGISTERPROPERTY(ParkingBrakeStatus,false);
	REGISTERPROPERTY(ParkingLightStatus,false);
	REGISTERPROPERTY(HazardLightStatus,false);
	registerPropertyPriv(AirbagStatus,[]()
	{
		BasicPropertyType<Airbag::Location> a(Airbag::Driver);
		BasicPropertyType<Airbag::Status> b(Airbag::Inactive);
		AirbagStatusType* t = new AirbagStatusType();
		t->append(a,b);

		return t;
	});

	REGISTERPROPERTY(AntilockBrakingSystem,false);
	REGISTERPROPERTY(TractionControlSystem,false);
	REGISTERPROPERTY(VehicleTopSpeedLimit,0);

	registerPropertyPriv(DoorStatus,[]()
	{
		DoorStatusType* t = new DoorStatusType();
		t->append(Door::Driver,Door::Closed);

		return t;
	});

	registerPropertyPriv(DoorLockStatus,[]()
	{
		DoorLockStatusType* t = new DoorLockStatusType();
		t->append(Door::Driver,false);

		return t;
	});

	registerPropertyPriv(SeatBeltStatus,[]()
	{
		SeatBeltStatusType* t = new SeatBeltStatusType();
		t->append(Seat::Driver,false);

		return t;
	});

	registerPropertyPriv(SeatBeltStatus,[]()
	{
		SeatBeltStatusType* t = new SeatBeltStatusType();
		t->append(Seat::Driver,false);

		return t;
	});

	registerPropertyPriv(OccupantStatus,[]()
	{
		OccupantStatusType* t = new OccupantStatusType();
		t->append(Seat::Driver,false);

		return t;
	});

	registerPropertyPriv(WindowLockStatus,[]()
	{
		WindowLockStatusType* t = new WindowLockStatusType();
		t->append(Window::Driver,false);

		return t;
	});

}

void VehicleProperty::factory()
{
	if(!thereCanOnlyBeOne)
		thereCanOnlyBeOne = new VehicleProperty();
}

std::list<VehicleProperty::Property> VehicleProperty::capabilities()
{
	return mCapabilities;
}

std::list<VehicleProperty::Property> VehicleProperty::customProperties()
{
	return mCustomProperties;
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

			if(value != "" )
				type->fromString(value);

			return type;
		}
	}

	return nullptr;
}

bool VehicleProperty::registerProperty(VehicleProperty::Property name, VehicleProperty::PropertyTypeFactoryCallback factory)
{

	mCustomProperties.push_back(name);

	return registerPropertyPriv(name, factory);
}

bool VehicleProperty::registerPropertyPriv(VehicleProperty::Property name, VehicleProperty::PropertyTypeFactoryCallback factory)
{
	if(ListPlusPlus<Property>(&mCapabilities).contains(name))
	{
		DebugOut(0)<<__FUNCTION__<<" ERROR: property '"<<name<<"'' already registered."<<endl;
		return false;
	}

	registeredPropertyFactoryMap[name] = factory;
	mCapabilities.push_back(name);

	return true;

}


