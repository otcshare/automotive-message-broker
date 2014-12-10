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
#include "superptr.hpp"
#include <map>


#define REGISTERPROPERTY(property, defaultValue) \
	registerPropertyPriv(property, []() { return new property ## Type(defaultValue); });

#define REGISTERPROPERTYWITHTYPE(property, type, defaultValue) \
	registerPropertyPriv(property, []() { return new type(property, defaultValue); });

using namespace std;

std::map<VehicleProperty::Property, VehicleProperty::PropertyTypeFactoryCallback> VehicleProperty::registeredPropertyFactoryMap;

std::unique_ptr<VehicleProperty> VehicleProperty::thereCanOnlyBeOne(nullptr);

const char* ButtonEvents::W3C::Home = "home";
const char* ButtonEvents::W3C::Back = "back";
const char* ButtonEvents::W3C::Search = "search";
const char* ButtonEvents::W3C::Call = "call";
const char* ButtonEvents::W3C::EndCall = "end_call";
const char* ButtonEvents::W3C::MediaPlay = "media_play";
const char* ButtonEvents::W3C::MediaPause = "media_pause";
const char* ButtonEvents::W3C::MediaPrevious = "media_previous";
const char* ButtonEvents::W3C::MediaNext = "media_next";
const char* ButtonEvents::W3C::VoiceRecognize = "voice_regocnize";
const char* ButtonEvents::W3C::Enter = "enter";
const char* ButtonEvents::W3C::Left = "left";
const char* ButtonEvents::W3C::Right = "right";
const char* ButtonEvents::W3C::Up = "up";
const char* ButtonEvents::W3C::Down = "down";

const char* Transmission::W3C::Park = "park";
const char* Transmission::W3C::Reverse = "reverse";
const char* Transmission::W3C::Neutral = "neutral";
const char* Transmission::W3C::Low = "low";
const char* Transmission::W3C::Drive = "drive";
const char* Transmission::W3C::Overdrive = "overdrive";

const char* Door::W3C::Closed = "closed";
const char* Door::W3C::Open = "open";
const char* Door::W3C::Ajar = "ajar";

const char* Seat::W3C::Vacant = "vacant";
const char* Seat::W3C::Child = "child";
const char* Seat::W3C::Adult = "adult";

const char* Seat::W3C::Pin = "pin";
const char* Seat::W3C::Keyfob = "keyfob";
const char* Seat::W3C::Bluetooth = "Bluetooth";
const char* Seat::W3C::NFC = "NFC";
const char* Seat::W3C::Fingerprint = "fingerprint";
const char* Seat::W3C::Camera = "camera";
const char* Seat::W3C::Voice = "voice";

const char* HVAC::W3C::FloorPanel = "floorpanel";
const char* HVAC::W3C::FloorDuct = "floorduct";
const char* HVAC::W3C::Bilevel = "bilevel";
const char* HVAC::W3C::DefrostFloor = "defrostfloor";

const char* Drive::W3C::Auto = "auto";
const char* Drive::W3C::Comfort = "comfort";
const char* Drive::W3C::Sport = "sport";
const char* Drive::W3C::Eco = "eco";
const char* Drive::W3C::Manual = "manual";
const char* Drive::W3C::Winter = "winter";

const char * WiperSpeedSetting::W3C::Off = "off";
const char * WiperSpeedSetting::W3C::Once = "once";
const char * WiperSpeedSetting::W3C::Slowest = "slowest";
const char * WiperSpeedSetting::W3C::Slow = "slow";
const char * WiperSpeedSetting::W3C::Middle = "middle";
const char * WiperSpeedSetting::W3C::Fast = "fast";
const char * WiperSpeedSetting::W3C::Fastest = "fastest";
const char * WiperSpeedSetting::W3C::Auto = "auto";

const char * ConvertibleRoofW3C::Closed = "closed";
const char * ConvertibleRoofW3C::Closing = "closing";
const char * ConvertibleRoofW3C::Opened = "opened";
const char * ConvertibleRoofW3C::Opening = "opening";

const VehicleProperty::Property VehicleProperty::NoValue = "NoValue";
const VehicleProperty::Property VehicleProperty::VehicleSpeed = "VehicleSpeed";
const VehicleProperty::Property VehicleProperty::EngineSpeed = "EngineSpeed";
const VehicleProperty::Property VehicleProperty::TransmissionShiftPosition = "TransmissionShiftPosition";
const VehicleProperty::Property VehicleProperty::TransmissionGearPosition = "TransmissionGearPostion";
const VehicleProperty::Property VehicleProperty::TransmissionMode = "TransmissionMode";
const VehicleProperty::Property VehicleProperty::TransmissionModeW3C = "TransmissionModeW3C";
const VehicleProperty::Property VehicleProperty::ThrottlePosition = "ThrottlePosition";
const VehicleProperty::Property VehicleProperty::WheelBrake = "WheelBrake";
const VehicleProperty::Property VehicleProperty::WheelBrakePressure = "WheelBrakePressure";
const VehicleProperty::Property VehicleProperty::SteeringWheelAngle = "SteeringWheelAngle";
const VehicleProperty::Property VehicleProperty::SteeringWheelAngleW3C = "SteeringWheelAngleW3C";
const VehicleProperty::Property VehicleProperty::TurnSignal = "TurnSignal";
const VehicleProperty::Property VehicleProperty::ClutchStatus = "ClutchStatus";
const VehicleProperty::Property VehicleProperty::EngineOilPressure = "EngineOilPressure";
const VehicleProperty::Property VehicleProperty::EngineCoolantTemperature = "EngineCoolantTemperature";
const VehicleProperty::Property VehicleProperty::EngineCoolantLevel = "EngineCoolantLevel";
const VehicleProperty::Property VehicleProperty::MachineGunTurretStatus = "MachineGunTurretStatus";
const VehicleProperty::Property VehicleProperty::AccelerationX = "AccelerationX";
const VehicleProperty::Property VehicleProperty::AccelerationY = "AccelerationY";
const VehicleProperty::Property VehicleProperty::AccelerationZ = "AccelerationZ";
const VehicleProperty::Property VehicleProperty::MassAirFlow = "MassAirFlow";
const VehicleProperty::Property VehicleProperty::ButtonEvent = "ButtonEvent";
const VehicleProperty::Property VehicleProperty::AirIntakeTemperature = "AirIntakeTemperature";
const VehicleProperty::Property VehicleProperty::BatteryVoltage = "BatteryVoltage";
const VehicleProperty::Property VehicleProperty::BatteryCurrent = "BatteryCurrent";
const VehicleProperty::Property VehicleProperty::BatteryChargeLevel = "BatteryChargeLevel";
const VehicleProperty::Property VehicleProperty::InteriorTemperature = "InteriorTemperature";
const VehicleProperty::Property VehicleProperty::ExteriorTemperature = "ExteriorTemperature";
const VehicleProperty::Property VehicleProperty::EngineOilTemperature = "EngineOilTemperature";
const VehicleProperty::Property VehicleProperty::EngineOilLifeRemaining = "EngineOilLifeRemaining";
const VehicleProperty::Property VehicleProperty::EngineOilChangeIndicator= "EngineOilChangeIndicator";
const VehicleProperty::Property VehicleProperty::VIN = "VIN";
const VehicleProperty::Property VehicleProperty::WMI = "WMI";
const VehicleProperty::Property VehicleProperty::TirePressure = "TirePressure";
const VehicleProperty::Property VehicleProperty::TirePressureLow = "TirePressureLow";
const VehicleProperty::Property VehicleProperty::TireTemperature = "TireTemperature";
const VehicleProperty::Property VehicleProperty::VehiclePowerMode = "VehiclePowerMode";
const VehicleProperty::Property VehicleProperty::TripMeters = "TripMeters";
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
const VehicleProperty::Property VehicleProperty::LightAutomatic= "LightAutomatic";
const VehicleProperty::Property VehicleProperty::LightDynamicHighBeam= "LightDynamicHighBeam";
const VehicleProperty::Property VehicleProperty::InteriorLightDriver = "InteriorLightDriver";
const VehicleProperty::Property VehicleProperty::InteriorLightCenter = "InteriorLightCenter";
const VehicleProperty::Property VehicleProperty::InteriorLightPassenger = "InteriorLightPassenger";
const VehicleProperty::Property VehicleProperty::InteriorLightStatus = "InteriorLightStatus";
const VehicleProperty::Property VehicleProperty::EngineLoad = "EngineLoad";
const VehicleProperty::Property VehicleProperty::Horn = "Horn";
const VehicleProperty::Property VehicleProperty::FuelLevel = "FuelLevel";
const VehicleProperty::Property VehicleProperty::FuelConsumption = "FuelConsumption";
const VehicleProperty::Property VehicleProperty::FuelRange = "FuelRange";
const VehicleProperty::Property VehicleProperty::FuelEconomy = "FuelEconomy";
const VehicleProperty::Property VehicleProperty::FuelAverageEconomy = "FuelAverageEconomy";
const VehicleProperty::Property VehicleProperty::FuelAverageConsumption = "FuelAverageConsumption";
const VehicleProperty::Property VehicleProperty::FuelConsumptionSinceRestart = "FuelConsumptionSinceRestart";
const VehicleProperty::Property VehicleProperty::FuelTimeSinceRestart = "FuelTimeSinceRestart";
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
const VehicleProperty::Property VehicleProperty::BrakePressure = "BrakePressure";
const VehicleProperty::Property VehicleProperty::Odometer = "Odometer";
const VehicleProperty::Property VehicleProperty::DistanceTotal = "DistanceTotal";
const VehicleProperty::Property VehicleProperty::DistanceSinceStart = "DistanceSinceStart";
const VehicleProperty::Property VehicleProperty::TransmissionFluidLevel = "TransmissionFluidLevel";
const VehicleProperty::Property VehicleProperty::BrakeFluidLevel = "BrakeFluidLevel";
const VehicleProperty::Property VehicleProperty::WasherFluidLevel = "WasherFluidLevel";
const VehicleProperty::Property VehicleProperty::WasherFluidLevelLow = "WasherFluidLevelLow";
const VehicleProperty::Property VehicleProperty::SecurityAlertStatus = "SecurityAlertStatus";
const VehicleProperty::Property VehicleProperty::ParkingBrakeStatus = "ParkingBrakeStatus";
const VehicleProperty::Property VehicleProperty::ParkingLightStatus = "ParkingLightStatus";
const VehicleProperty::Property VehicleProperty::HazardLightStatus = "HazardLightStatus";
const VehicleProperty::Property VehicleProperty::AirbagStatus = "AirbagStatus";
const VehicleProperty::Property VehicleProperty::AirbagDeployed = "AirbagDeployed";
const VehicleProperty::Property VehicleProperty::AirbagActivated = "AirbagActivated";
const VehicleProperty::Property VehicleProperty::AntilockBrakingSystem = "AntilockBrakingSystem";
const VehicleProperty::Property VehicleProperty::AntilockBrakingSystemEnabled = "AntilockBrakingSystemEnabled";
const VehicleProperty::Property VehicleProperty::TractionControlSystem = "TractionControlSystem";
const VehicleProperty::Property VehicleProperty::TractionControlSystemEnabled = "TractionControlSystemEnabled";
const VehicleProperty::Property VehicleProperty::VehicleTopSpeedLimit = "VehicleTopSpeedLimit";
const VehicleProperty::Property VehicleProperty::DoorStatus = "DoorStatus";
const VehicleProperty::Property VehicleProperty::DoorStatusW3C = "DoorStatusW3C";
const VehicleProperty::Property VehicleProperty::DoorLockStatus = "DoorLockStatus";
const VehicleProperty::Property VehicleProperty::ChildLockStatus = "ChildLockStatus";
const VehicleProperty::Property VehicleProperty::SeatBeltStatus = "SeatBeltStatus";
const VehicleProperty::Property VehicleProperty::WindowLockStatus = "WindowLockStatus";
const VehicleProperty::Property VehicleProperty::OccupantStatus = "OccupantStatus";
const VehicleProperty::Property VehicleProperty::OccupantStatusW3C = "OccupantStatusW3C";
const VehicleProperty::Property VehicleProperty::ObstacleDistance = "ObstacleDistance";
const VehicleProperty::Property VehicleProperty::RainSensor = "RainSensor";
const VehicleProperty::Property VehicleProperty::WindshieldWiper = "WindshieldWiper";
const VehicleProperty::Property VehicleProperty::WindshieldWiperSpeed = "WindshieldWiperSpeed";
const VehicleProperty::Property VehicleProperty::WindshieldWiperSetting = "WindshieldWiperSetting";
const VehicleProperty::Property VehicleProperty::AirflowDirection = "AirflowDirection";
const VehicleProperty::Property VehicleProperty::AirflowDirectionW3C = "AirflowDirectionW3C";
const VehicleProperty::Property VehicleProperty::FanSpeed = "FanSpeed";
const VehicleProperty::Property VehicleProperty::TargetTemperature = "TargetTemperature";
const VehicleProperty::Property VehicleProperty::AirConditioning = "AirConditioning";
const VehicleProperty::Property VehicleProperty::AirRecirculation = "AirRecirculation";
const VehicleProperty::Property VehicleProperty::Heater = "Heater";
const VehicleProperty::Property VehicleProperty::Defrost = "Defrost";
const VehicleProperty::Property VehicleProperty::DefrostWindow = "DefrostWindow";
const VehicleProperty::Property VehicleProperty::DefrostMirror = "DefrostMirror";
const VehicleProperty::Property VehicleProperty::SteeringWheelHeater = "SteeringWheelHeater";
const VehicleProperty::Property VehicleProperty::SeatHeater = "SeatHeater";
const VehicleProperty::Property VehicleProperty::SeatCooler = "SeatCooler";
const VehicleProperty::Property VehicleProperty::WindowStatus = "WindowStatus";
const VehicleProperty::Property VehicleProperty::Sunroof = "Sunroof";
const VehicleProperty::Property VehicleProperty::SunroofTilt = "SunroofTilt";
const VehicleProperty::Property VehicleProperty::ConvertibleRoof = "ConvertibleRoof";
const VehicleProperty::Property VehicleProperty::ConvertibleRoofStatus = "ConvertibleRoofStatus";
const VehicleProperty::Property VehicleProperty::NightMode = "NightMode";
const VehicleProperty::Property VehicleProperty::DrivingMode = "DrivingMode";
const VehicleProperty::Property VehicleProperty::DrivingModeW3C = "DrivingModeW3C";
const VehicleProperty::Property VehicleProperty::KeyId = "KeyId";
const VehicleProperty::Property VehicleProperty::Language = "Language";
const VehicleProperty::Property VehicleProperty::MeasurementSystem = "MeasurementSystem";
const VehicleProperty::Property VehicleProperty::MirrorSettingPan = "MirrorSettingPan";
const VehicleProperty::Property VehicleProperty::MirrorSettingTilt= "MirrorSettingTilt";
const VehicleProperty::Property VehicleProperty::SteeringWheelPositionSlide = "SteeringWheelPositionSlide";
const VehicleProperty::Property VehicleProperty::SteeringWheelPositionTilt = "SteeringWheelPositionTilt";
const VehicleProperty::Property VehicleProperty::SeatPositionRecline = "SeatPositionRecline";
const VehicleProperty::Property VehicleProperty::SeatPositionSlide = "SeatPositionSlide";
const VehicleProperty::Property VehicleProperty::SeatPositionCushionHeight = "SeatPositionCushionHeight";
const VehicleProperty::Property VehicleProperty::SeatPositionHeadrest = "SeatPositionHeadrest";
const VehicleProperty::Property VehicleProperty::SeatPositionBackCushion = "SeatPositionBackCushion";
const VehicleProperty::Property VehicleProperty::SeatPositionSideCushion = "SeatPositionSideCushion";
const VehicleProperty::Property VehicleProperty::DashboardIllumination = "DashboardIllumination";
const VehicleProperty::Property VehicleProperty::GeneratedVehicleSoundMode = "GeneratedVehicleSoundMode";
const VehicleProperty::Property VehicleProperty::DriverId = "DriverId";
const VehicleProperty::Property VehicleProperty::PowertrainTorque = "PowertrainTorque";
const VehicleProperty::Property VehicleProperty::AcceleratorPedalPosition = "AcceleratorPedalPosition";
const VehicleProperty::Property VehicleProperty::Chime = "Chime";
const VehicleProperty::Property VehicleProperty::WheelTick = "WheelTick";
const VehicleProperty::Property VehicleProperty::IgnitionTimeOn = "IgnitionTimeOn";
const VehicleProperty::Property VehicleProperty::IgnitionTimeOff = "IgnitionTimeOff";
const VehicleProperty::Property VehicleProperty::YawRate = "YawRate";
const VehicleProperty::Property VehicleProperty::ButtonEventW3C = "ButtonEventW3C";
const VehicleProperty::Property VehicleProperty::TransmissionOilWear = "TransmissionOilWear";
const VehicleProperty::Property VehicleProperty::TransmissionOilTemperature = "TransmissionOilTemperature";
const VehicleProperty::Property VehicleProperty::TransmissionClutchWear = "TransmissionClutchWear";
const VehicleProperty::Property VehicleProperty::BrakePadWear = "BrakePadWear";
const VehicleProperty::Property VehicleProperty::BrakeFluidLevelLow = "BrakeFluidLevelLow";
const VehicleProperty::Property VehicleProperty::MalfunctionIndicatorOn = "MalfunctionIndicatorOn";
const VehicleProperty::Property VehicleProperty::AccumulatedEngineRuntime = "AccumulatedEngineRuntime";
const VehicleProperty::Property VehicleProperty::DistanceSinceCodeCleared = "DistanceSinceCodeCleared";
const VehicleProperty::Property VehicleProperty::DistanceWithMILOn = "DistanceWithMILOn";
const VehicleProperty::Property VehicleProperty::TimeRunMILOn = "TimeRunMILOn";
const VehicleProperty::Property VehicleProperty::TimeTroubleCodeClear = "TimeTroubleCodeClear";
const VehicleProperty::Property VehicleProperty::VehicleDriveMode = "VehicleDriveMode";
const VehicleProperty::Property VehicleProperty::ActiveNoiseControlMode = "ActiveNoiseControlMode";
const VehicleProperty::Property VehicleProperty::AvailableSounds = "AvailableSounds";
const VehicleProperty::Property VehicleProperty::EngineSoundEnhancementMode = "EngineSoundEnhancementMode";
const VehicleProperty::Property VehicleProperty::ElectronicStabilityControlEnabled = "ElectronicStabilityControlEnabled";
const VehicleProperty::Property VehicleProperty::ElectronicStabilityControlEngaged = "ElectronicStabilityControlEngaged";
const VehicleProperty::Property VehicleProperty::OccupantIdentificationType = "OccupantIdentificationType";
const VehicleProperty::Property VehicleProperty::OccupantName = "OccupantName";
const VehicleProperty::Property VehicleProperty::AtmosphericPressure = "AtmosphericPressure";

PropertyList VehicleProperty::mCapabilities;
PropertyList VehicleProperty::mCustomProperties;

VehicleProperty::VehicleProperty()
{
	REGISTERPROPERTY( VehicleSpeed, 0);
	REGISTERPROPERTY(EngineSpeed, 0);
	REGISTERPROPERTY(TransmissionShiftPosition, Transmission::Neutral);
	REGISTERPROPERTY(TransmissionGearPosition, Transmission::Neutral);
	REGISTERPROPERTY(TransmissionMode, Transmission::Normal);
	REGISTERPROPERTY(TransmissionModeW3C, "neutral");
	REGISTERPROPERTY(ThrottlePosition, 0);
	REGISTERPROPERTY(WheelBrake, false);
	REGISTERPROPERTY(WheelBrakePressure, 0);
	REGISTERPROPERTY(SteeringWheelAngle, 0);
	REGISTERPROPERTY(SteeringWheelAngleW3C, 0);
	REGISTERPROPERTY(TurnSignal, TurnSignals::Off);
	REGISTERPROPERTY(ClutchStatus, false);
	REGISTERPROPERTY(EngineOilPressure, 0);
	REGISTERPROPERTY(EngineOilTemperature, 0);
	REGISTERPROPERTY(EngineOilRemaining, 0);
	REGISTERPROPERTY(EngineOilLifeRemaining, 0);
	REGISTERPROPERTY(EngineOilChangeIndicator, false);
	REGISTERPROPERTY(EngineCoolantTemperature, 0);
	REGISTERPROPERTY(EngineCoolantLevel, 0);
	REGISTERPROPERTY(MachineGunTurretStatus, false);
	REGISTERPROPERTY(AccelerationX, 0);
	REGISTERPROPERTY(AccelerationY, 0);
	REGISTERPROPERTY(AccelerationZ, 0);
	REGISTERPROPERTY(MassAirFlow, 0);
	REGISTERPROPERTY(ButtonEvent, ButtonEvents::NoButton);
	REGISTERPROPERTY(AirIntakeTemperature, 0)
	REGISTERPROPERTY(BatteryVoltage, 0);
	REGISTERPROPERTY(BatteryCurrent, 0);
	REGISTERPROPERTY(BatteryChargeLevel, 0);
	REGISTERPROPERTY(InteriorTemperature, 0);
	REGISTERPROPERTY(ExteriorTemperature, 0);
	REGISTERPROPERTY(VIN, "");
	REGISTERPROPERTY(WMI, "");
	REGISTERPROPERTY(TirePressure, 0);
	REGISTERPROPERTY(TirePressureLow, false);
	REGISTERPROPERTY(TireTemperature, 0);
	REGISTERPROPERTY( VehiclePowerMode, Power::Off);
	registerPropertyPriv(TripMeters, [](){
		TripMetersType* t = new TripMetersType();
		BasicPropertyType<uint16_t> v(0);
		t->append(v);
		return t;
	});

	REGISTERPROPERTY(CruiseControlActive, false);
	REGISTERPROPERTY(CruiseControlSpeed, 0);
	REGISTERPROPERTY(LightHead, false);
	REGISTERPROPERTY(LightLeftTurn, false);
	REGISTERPROPERTY(LightRightTurn, false);
	REGISTERPROPERTY(LightParking, false);
	REGISTERPROPERTY(LightFog, false);
	REGISTERPROPERTY(LightHazard, false);
	REGISTERPROPERTY(LightHighBeam, false);
	REGISTERPROPERTY(LightBrake, false);
	REGISTERPROPERTY(LightAutomatic, false);
	REGISTERPROPERTY(LightDynamicHighBeam, false);
	REGISTERPROPERTY(InteriorLightDriver, false);
	REGISTERPROPERTY(InteriorLightPassenger, false);
	REGISTERPROPERTY(InteriorLightCenter, false);
	REGISTERPROPERTY(InteriorLightStatus, false);
	REGISTERPROPERTY(EngineLoad, 0);
	REGISTERPROPERTY(Horn, false);
	REGISTERPROPERTY(FuelLevel, 0);
	REGISTERPROPERTY(FuelRange, 0);
	REGISTERPROPERTY(FuelConsumption, 0);
	REGISTERPROPERTY(FuelEconomy, 0);
	REGISTERPROPERTY(FuelAverageEconomy, 0);
	REGISTERPROPERTY(FuelAverageConsumption, 0);
	REGISTERPROPERTY(FuelConsumptionSinceRestart, 0);
	REGISTERPROPERTY(FuelTimeSinceRestart, 0);
	REGISTERPROPERTY(FuelType, Fuel::Unknown);
	REGISTERPROPERTY(FuelPositionSide, Fuel::UnknownPosition);
	REGISTERPROPERTY(ExteriorBrightness, 0);
	REGISTERPROPERTY(VehicleWidth, 0);
	REGISTERPROPERTY(VehicleHeight, 0);
	REGISTERPROPERTY(VehicleLength, 0);
	REGISTERPROPERTY(Latitude, 0);
	REGISTERPROPERTY(Longitude, 0);
	REGISTERPROPERTY(Altitude, 0);
	REGISTERPROPERTY(Direction, 0);
	REGISTERPROPERTY(VehicleType, Vehicle::Unknown);
	registerPropertyPriv(DoorsPerRow, []() {
		BasicPropertyType<uint16_t> d(0);
		DoorsPerRowType* doors = new DoorsPerRowType();
		doors->append(d);
		return doors;
	});
	REGISTERPROPERTY(TransmissionGearType, Transmission::Unknown);
	REGISTERPROPERTY(FrontWheelRadius, 0);
	REGISTERPROPERTY(RearWheelRadius, 0);
	REGISTERPROPERTY(WheelTrack, 0);
	REGISTERPROPERTY(BrakePressure, 0);
	REGISTERPROPERTY(Odometer, 0);
	REGISTERPROPERTY(DistanceTotal, 0);
	REGISTERPROPERTY(DistanceSinceStart, 0);
	REGISTERPROPERTY(TransmissionFluidLevel, 0);
	REGISTERPROPERTY(BrakeFluidLevel, 0);
	REGISTERPROPERTY(WasherFluidLevel, 0);
	REGISTERPROPERTY(WasherFluidLevelLow, false);
	REGISTERPROPERTY(SecurityAlertStatus, Security::Idle);
	REGISTERPROPERTY(ParkingBrakeStatus, false);
	REGISTERPROPERTY(ParkingLightStatus, false);
	REGISTERPROPERTY(HazardLightStatus, false);
	/// TODO: deprecated in 0.14
	REGISTERPROPERTY(AirbagStatus, Airbag::Inactive);
	REGISTERPROPERTY(AirbagActivated, false);
	REGISTERPROPERTY(AirbagDeployed, false);

	REGISTERPROPERTY(AntilockBrakingSystem, false);
	REGISTERPROPERTY(AntilockBrakingSystemEnabled, false);
	REGISTERPROPERTY(TractionControlSystem, false);
	REGISTERPROPERTY(TractionControlSystemEnabled, false);
	REGISTERPROPERTY(VehicleTopSpeedLimit, 0);

	REGISTERPROPERTY(DoorStatus, Door::Closed);
	REGISTERPROPERTY(DoorStatusW3C, Door::W3C::Closed);
	REGISTERPROPERTY(DoorLockStatus, false);
	REGISTERPROPERTY(ChildLockStatus, false);
	REGISTERPROPERTY(SeatBeltStatus, false);
	/// TODO: deprecated in 0.14
	REGISTERPROPERTY(OccupantStatus, Seat::Vacant);
	REGISTERPROPERTY(OccupantStatusW3C, Seat::W3C::Vacant);
	REGISTERPROPERTY(WindowLockStatus, false);
	REGISTERPROPERTY(ObstacleDistance, 0);

	REGISTERPROPERTY(RainSensor, 0);
	REGISTERPROPERTY(WindshieldWiper, Window::Off);
	REGISTERPROPERTY(WindshieldWiperSpeed, WiperSpeedSetting::W3C::Off);
	REGISTERPROPERTY(WindshieldWiperSetting, WiperSpeedSetting::W3C::Off);
	REGISTERPROPERTY(AirflowDirection, HVAC::Front);
	REGISTERPROPERTY(AirflowDirectionW3C, HVAC::W3C::FloorPanel);
	REGISTERPROPERTY(FanSpeed, 0);
	REGISTERPROPERTY(TargetTemperature, 0);
	REGISTERPROPERTY(AirConditioning, false);
	REGISTERPROPERTY(AirRecirculation, false);
	REGISTERPROPERTY(Heater, false);

	REGISTERPROPERTY(Defrost, false);
	REGISTERPROPERTY(DefrostWindow, false);
	REGISTERPROPERTY(DefrostMirror, false);

	REGISTERPROPERTY(SteeringWheelHeater, false);
	REGISTERPROPERTY(SeatHeater, 0);
	REGISTERPROPERTY(SeatCooler, false);
	REGISTERPROPERTY(WindowStatus, 100);
	REGISTERPROPERTY(Sunroof, 0);
	REGISTERPROPERTY(SunroofTilt, 0);
	REGISTERPROPERTY(ConvertibleRoof, false);
	REGISTERPROPERTY(ConvertibleRoofStatus, "");

	REGISTERPROPERTY(NightMode, false);
	REGISTERPROPERTY(DrivingMode, Driving::None);
	REGISTERPROPERTY(DrivingModeW3C, false);
	REGISTERPROPERTY(KeyId, "");
	REGISTERPROPERTY(Language, "");
	REGISTERPROPERTY(MeasurementSystem, Measurement::Metric);
	REGISTERPROPERTY(MirrorSettingPan, 0);
	REGISTERPROPERTY(MirrorSettingTilt, 0);
	REGISTERPROPERTY(SteeringWheelPositionSlide, 0);
	REGISTERPROPERTY(SteeringWheelPositionTilt, 0);
	REGISTERPROPERTY(SeatPositionRecline, 0);
	REGISTERPROPERTY(SeatPositionSlide, 0);
	REGISTERPROPERTY(SeatPositionCushionHeight, 0);
	REGISTERPROPERTY(SeatPositionHeadrest, 0);
	REGISTERPROPERTY(SeatPositionBackCushion, 0);
	REGISTERPROPERTY(SeatPositionSideCushion, 0);
	REGISTERPROPERTY(DashboardIllumination, 0);
	REGISTERPROPERTY(GeneratedVehicleSoundMode, Vehicle::Normal);
	REGISTERPROPERTY(DriverId, "");
	REGISTERPROPERTY(PowertrainTorque, 0);
	REGISTERPROPERTY(AcceleratorPedalPosition, 0);
	REGISTERPROPERTY(Chime, false);
	REGISTERPROPERTY(WheelTick, 0);
	REGISTERPROPERTY(IgnitionTimeOff, 0);
	REGISTERPROPERTY(IgnitionTimeOn, 0);
	REGISTERPROPERTY(YawRate, 0);
	registerPropertyPriv(ButtonEventW3C, [](){
		ButtonEventW3CType* t = new ButtonEventW3CType();
		StringPropertyType k, v;
		t->append(k, v);
		return t;
	});
	REGISTERPROPERTY(TransmissionOilWear, 0);
	REGISTERPROPERTY(TransmissionOilTemperature, 0);
	REGISTERPROPERTY(TransmissionClutchWear, 0);
	REGISTERPROPERTY(BrakePadWear, 0);
	REGISTERPROPERTY(BrakeFluidLevelLow, false);
	REGISTERPROPERTY(MalfunctionIndicatorOn, false);
	REGISTERPROPERTY(AccumulatedEngineRuntime, 0);
	REGISTERPROPERTY(DistanceSinceCodeCleared, 0);
	REGISTERPROPERTY(DistanceWithMILOn, 0);
	REGISTERPROPERTY(TimeRunMILOn, 0);
	REGISTERPROPERTY(TimeTroubleCodeClear, 0);
	REGISTERPROPERTY(VehicleDriveMode, "");
	REGISTERPROPERTY(ActiveNoiseControlMode, false);
	registerPropertyPriv(AvailableSounds, [](){
		AvailableSoundsType* t = new AvailableSoundsType();
		StringPropertyType v;
		t->append(v);
		return t;
	});
	REGISTERPROPERTY(EngineSoundEnhancementMode, "");
	REGISTERPROPERTY(ElectronicStabilityControlEnabled, false);
	REGISTERPROPERTY(ElectronicStabilityControlEngaged, false);
	REGISTERPROPERTY(OccupantIdentificationType, Seat::W3C::Pin);
	REGISTERPROPERTY(OccupantName, "");
	REGISTERPROPERTY(AtmosphericPressure, 0);
}

void VehicleProperty::factory()
{
	if(!thereCanOnlyBeOne)
		thereCanOnlyBeOne = amb::make_unique(new VehicleProperty());
}

PropertyList VehicleProperty::capabilities()
{
	return mCapabilities;
}

PropertyList VehicleProperty::customProperties()
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

	DebugOut(DebugOut::Error)<<"Property not found"<<endl;

	return nullptr;
}

bool VehicleProperty::registerProperty(VehicleProperty::Property name, VehicleProperty::PropertyTypeFactoryCallback factory)
{
	if(!contains(mCustomProperties,name))
	{
		mCustomProperties.push_back(name);

		return registerPropertyPriv(name, factory);
	}

	return false;
}

bool VehicleProperty::registerPropertyPriv(VehicleProperty::Property name, VehicleProperty::PropertyTypeFactoryCallback factory)
{
	if(contains(mCapabilities,name))
	{
		DebugOut(0)<<__FUNCTION__<<" ERROR: property '"<<name<<"' already registered."<<endl;
		return false;
	}

	registeredPropertyFactoryMap[name] = factory;
	mCapabilities.push_back(name);

	return true;

}



