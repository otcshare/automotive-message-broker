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


#ifndef VEHICLEPROPERTY_H
#define VEHICLEPROPERTY_H

#include "nullptr.h"
#include <string>
#include <list>
#include <set>
#include <sstream>
#include <map>
#include <functional>
#include "abstractpropertytype.h"
#include "mappropertytype.hpp"


namespace ButtonEvents {
enum ButtonEventType {
	NoButton = 0,
	PlayButton = 1,
	SkipButton = 1 << 1,
	PrevButton = 1 << 2,
	PauseButton = 1 << 3,
	Preset1Button = 1 << 4,
	Preset2Button = 1 << 5,
	Preset3Button = 1 << 6,
	Preset4Button = 1 << 7,
	StopButton = 1 << 8,
	NavigateUpButton = 1 << 9,
	NavigateDownButton = 1 << 10,
	NavigateLeftButton = 1 << 11,
	NavigateRightButton = 1 << 12
};

namespace W3C
{
extern const char* Home;
extern const char* Back;
extern const char* Search;
extern const char* Call;
extern const char* EndCall;
extern const char* MediaPlay;
extern const char* MediaNext;
extern const char* MediaPrevious;
extern const char* MediaPause;
extern const char* VoiceRecognize;
extern const char* Enter;
extern const char* Left;
extern const char* Right;
extern const char* Up;
extern const char* Down;
}
}

namespace TurnSignals {
enum TurnSignalType
{
	Off=0,
	Right=1,
	Left=2,
	Hazard=3
};
}

namespace Transmission {
enum TransmissionPositions
{
	Neutral = 0,
	First,
	Second,
	Third,
	Forth,
	Fifth,
	Sixth,
	Seventh,
	Eighth,
	Ninth,
	Tenth,
	CVT = 64,
	Drive = 96,
	Reverse = 128,
	Park = 255

};
/** TODO: deprecate in 0.13.  Replaced with Drive::Mode: **/
enum Mode {
	Normal=0,
	Sports = 1,
	Economy = 2,
	OEMCustom1 = 3,
	OEMCustom2 = 4
};

enum Type {
	Unknown = -1,
	Auto = 0,
	Manual = 1,
	CV = 2
};

namespace W3C
{
extern const char* Park;
extern const char* Reverse;
extern const char* Neutral;
extern const char* Low;
extern const char* Drive;
extern const char* Overdrive;
}
}

namespace Power {
/**< Vehicle Power Modes
 * Off = Vehicle is off and key is in the "off" position.
 * Accessory1 = Vehicle is off and key is in Accessory1 position.
 * Accessory2 = Vehicle is off and key is in Accessory2 position.
 * Run = Vehichle is running.  Key is in the running position.
 */
enum PowerModes
{
	Off = 0,
	Accessory1 = 1,
	Accessory2 = 2,
	Run = 3
};
}

namespace Fuel{
enum Type
{
	Unknown = -1,
	Gasoline = 0,
	High_Octane,
	Diesel,
	Electric,
	Hydrogen
};

enum RefuelPosition
{
	UnknownPosition = -1,
	Left = 0,
	Right,
	Front,
	Rear
};
}

namespace Vehicle {
enum Type
{
	Unknown = -1,
	Sedan = 0,
	Coupe,
	Cabriole,
	Roadster,
	SUV,
	Truck
};
enum SoundMode {
	Normal = 0,
	Quiet = 1,
	Sportive = 2
};
}

namespace Security {
enum Status
{
	Idle = 0,
	Armed,
	AlarmDetected
};
}

namespace Airbag {

enum Status
{
	Inactive = 0,
	Active,
	Deployed
};

}

namespace Door
{
enum Status
{
	Closed = 0,
	Open,
	Ajar
};
namespace W3C
{
extern const char * Closed;
extern const char * Open;
extern const char * Ajar;
}
}

namespace Seat
{

enum Occupant
{
	Vacant = 0,
	Child,
	Adult
};
}

namespace Window
{
enum Location
{
	Driver = 0,
	Passenger,
	LeftRear,
	RightRear,
	Sunroof,
	Windshield,
	SideMirrorLeft,
	SideMirrorRight,
	Rear
};

enum WiperSpeed
{
	Off = 0,
	Slowest = 1,
	Fastest = 5,
	Auto = 10
};

}

namespace HVAC
{
enum AirflowDirection
{
	FrontPanel=0,
	FloorDuct = 1,
	Front = 1 << 1,
	Defroster = 1 << 2

};

namespace W3C
{
extern const char * FloorPanel;
extern const char * FloorDuct;
extern const char * Bilevel;
extern const char * DefrostFloor;
}
}

namespace Driving
{
enum Mode
{
	None = 0,
	Driving
};
}

namespace Drive
{
enum Mode {
	Normal=0,
	Sports = 1,
	Economy = 2,
	OEMCustom1 = 3,
	OEMCustom2 = 4
};
}

namespace Measurement
{
enum Type
{
	Metric = 0,
	ImperialUS = 1,
	ImperialUK = 2
};
}

#include <boost/preprocessor/comma.hpp>

#define PROPERTYTYPE(property, propertyType, baseClass, valueType) \
	class propertyType : public baseClass { \
	public: propertyType(): baseClass(property) {} \
	propertyType(valueType val) : baseClass(property, val) {} \
	using baseClass::operator=; \
	};

#define PROPERTYTYPE1(property, propertyType, baseClass, valueType) \
	class propertyType : public baseClass { \
	public: propertyType(): baseClass(#property) {} \
	propertyType(valueType val) : baseClass(#property, val) {} \
	};

#define PROPERTYTYPEBASIC(property, valueType) \
	class property ## Type : public BasicPropertyType<valueType> { \
	public: property ## Type(): BasicPropertyType(property) {} \
	property ## Type(valueType val) : BasicPropertyType(property, val) {} \
	using BasicPropertyType<valueType>::operator=; \
	};

#define PROPERTYTYPEBASIC1(property, valueType) \
	class property ## Type : public BasicPropertyType<valueType> { \
	public: property ## Type(): BasicPropertyType( #property) {} \
	property ## Type(valueType val) : BasicPropertyType(#property, val) {} \
	};

#define PROPERTYTYPENOVAL(property, propertyType, baseClass) \
	class propertyType : public baseClass { \
	public: propertyType(): baseClass(property) {} \
	};

class VehicleProperty;


typedef std::vector<std::string> PropertyList;
typedef std::set<std::string> PropertySet;

class VehicleProperty
{

public:


	/*!
	 * \brief factory constructs a static instance of VehicleProperty.  This should be called once before VehicleProperty is used in the app
	 */
	static void factory();

	typedef std::string Property;

	/*!
	 * \brief PropertyTypeFactoryCallback callback used to construct a AbstractPropertyType for a property
	 * \see registerProperty
	 */
	typedef std::function<AbstractPropertyType* (void)> PropertyTypeFactoryCallback;

	/// Various property types:

	static const Property NoValue;

	/**< Vehicle Velocity in km/h */
	static const Property VehicleSpeed;
	PROPERTYTYPE(VehicleSpeed, VehicleSpeedType, BasicPropertyType<uint16_t>, uint16_t)
	//typedef BasicPropertyType<uint16_t> VehicleSpeedType;

	/**< Engine Speed in rotations per minute */
	static const Property EngineSpeed;
	PROPERTYTYPE(EngineSpeed, EngineSpeedType, BasicPropertyType<uint16_t>, uint16_t)
	//typedef BasicPropertyType<uint16_t> EngineSpeedType;

	 /**< Transmission Shift Position
	 * 0 = Neutral
	 * 1 = 1st
	 * 2 = 2nd
	 * ...
	 * 96 = Drive
	 * 128 = Reverse
	 * 255 = Park
	 */
	static const Property TransmissionShiftPosition;
	PROPERTYTYPE(TransmissionShiftPosition, TransmissionShiftPositionType,
				 BasicPropertyType<Transmission::TransmissionPositions>, Transmission::TransmissionPositions )
	//typedef BasicPropertyType<Transmission::TransmissionPositions> TransmissionShiftPositionType;

	/**< Transmission Gear Position
	* 0 = Neutral
	* 1 = 1st
	* 2 = 2nd
	* ...
	* 64 = CVT
	* 128 = Reverse
	*/
	static const Property TransmissionGearPosition;
	PROPERTYTYPE(TransmissionGearPosition, TransmissionGearPositionType,
				 BasicPropertyType<Transmission::TransmissionPositions>, Transmission::TransmissionPositions)
	//typedef BasicPropertyType<Transmission::TransmissionPositions> TransmissionGearPositionType;

	/// TODO: deprecate in 0.13.  Replaced by Drive::Mode
	static const Property TransmissionMode;
	PROPERTYTYPE(TransmissionMode, TransmissionModeType,
				 BasicPropertyType<Transmission::Mode>, Transmission::Mode)
	//typedef BasicPropertyType<Transmission::Mode> TransmissionModeType;

	/**< TransmissionModeW3C
	 * see Transmission::W3C::Park, Transmission::W3C::Drive, etc
	 */
	static const Property TransmissionModeW3C;
	PROPERTYTYPE(TransmissionModeW3C, TransmissionModeW3CType, StringPropertyType, std::string)

	/**< Throttle position 0-100% */
	static const Property ThrottlePosition;
	PROPERTYTYPE(ThrottlePosition, ThrottlePositionType, BasicPropertyType<uint16_t> , uint16_t)
	//typedef BasicPropertyType<uint16_t> ThrottlePositionType;

	/**< Wheel brake position.  Engaged = true, Idle = false */
	static const Property WheelBrake;
	PROPERTYTYPE(WheelBrake, WheelBrakeType, BasicPropertyType<bool>, bool)
	//typedef BasicPropertyType<bool> WheelBrakeType;

	static const Property WheelBrakePressure;
	PROPERTYTYPE(WheelBrakePressure, WheelBrakePressureType, BasicPropertyType<uint16_t>, uint16_t)
	//typedef BasicPropertyType<uint16_t> WheelBrakePressureType;

	/**< Steering wheel angle (0-359)
	 * TODO: Deprecate in 0.13.  Replace with SteeringWheelAngleW3C
	*/
	static const Property SteeringWheelAngle;
	PROPERTYTYPE(SteeringWheelAngle, SteeringWheelAngleType, BasicPropertyType<uint16_t>, uint16_t)

	/// TODO: Rename to "SteeringWheel" in 0.13
	static const Property SteeringWheelAngleW3C;
	PROPERTYTYPEBASIC(SteeringWheelAngleW3C, int16_t)

	/**< 0=off, 1=right, 2=left, 3=hazard */
	static const Property TurnSignal;
	PROPERTYTYPEBASIC(TurnSignal, TurnSignals::TurnSignalType)

	/**< Clutch pedal status 0=off, 1=on */
	static const Property ClutchStatus;
	PROPERTYTYPE(ClutchStatus, ClutchStatusType,BasicPropertyType<bool>,bool)

	 /**< Oil pressure TODO: units */
	static const Property EngineOilPressure;
	PROPERTYTYPE(EngineOilPressure, EngineOilPressureType, BasicPropertyType<uint16_t>, uint16_t)

	/**< Engine coolant temperature in degrees celcius **/
	static const Property EngineCoolantTemperature;
	PROPERTYTYPEBASIC(EngineCoolantTemperature, int16_t)

	static const Property EngineCoolantLevel;
	PROPERTYTYPE(EngineCoolantLevel, EngineCoolantLevelType, BasicPropertyType<uint>, uint)

	/**< 0=off, 1=on */
	static const Property MachineGunTurretStatus;
	PROPERTYTYPEBASIC(MachineGunTurretStatus, bool)

	static const Property AccelerationX;
	PROPERTYTYPEBASIC(AccelerationX, int16_t)

	static const Property AccelerationY;
	PROPERTYTYPEBASIC(AccelerationY, int16_t)

	static const Property AccelerationZ;
	PROPERTYTYPEBASIC(AccelerationZ, int16_t)

	/**< Mass Air Flow.  grams/sec */
	static const Property MassAirFlow;
	PROPERTYTYPE(MassAirFlow, MassAirFlowType, BasicPropertyType<uint16_t>, uint16_t)
	//typedef BasicPropertyType<uint16_t> MassAirFlowType;

	/**< Button Event @see ButtonEvents::ButtonEventType */
	///TODO: deprecated.  Use ButtonEventW3C. Remove in 0.14
	static const Property ButtonEvent;
	PROPERTYTYPE(ButtonEvent, ButtonEventType, BasicPropertyType<ButtonEvents::ButtonEventType>, ButtonEvents::ButtonEventType)

	static const Property ButtonEventW3C;
	PROPERTYTYPE(ButtonEventW3C, ButtonEventW3CType, StringPropertyType, std::string)

	/**< Air intake temperature in degrees celcius */
	static const Property AirIntakeTemperature;
	PROPERTYTYPE(AirIntakeTemperature, AirIntakeTemperatureType, BasicPropertyType<int>, int)
	//typedef BasicPropertyType<int> AirIntakeTemperatureType;

	/**< Battery voltage in volts */
	static const Property BatteryVoltage;
	PROPERTYTYPE(BatteryVoltage, BatteryVoltageType, BasicPropertyType<double>, double)
	//typedef BasicPropertyType<double> BatteryVoltageType;

	static const Property BatteryCurrent;
	PROPERTYTYPE(BatteryCurrent, BatteryCurrentType, BasicPropertyType<double>, double)
	//typedef BasicPropertyType<double> BatteryCurrentType;

	static const Property BatteryChargeLevel;
	PROPERTYTYPEBASIC(BatteryChargeLevel, uint16_t)

	/**< Interior Air Temperature in degrees celcius */
	static const Property InteriorTemperature;
	PROPERTYTYPE(InteriorTemperature, InteriorTemperatureType, BasicPropertyType<int>, int)
	//typedef BasicPropertyType<int> InteriorTemperatureType;

	static const Property ExteriorTemperature;
	PROPERTYTYPE(ExteriorTemperature, ExteriorTemperatureType, BasicPropertyType<int>, int)
	//typedef BasicPropertyType<int> ExteriorTemperatureType;

	static const Property EngineOilTemperature;
	PROPERTYTYPEBASIC(EngineOilTemperature, int)

	static const Property EngineOilRemaining;
	PROPERTYTYPEBASIC(EngineOilRemaining, uint16_t)

	static const Property EngineOilLifeRemaining;
	PROPERTYTYPEBASIC(EngineOilLifeRemaining, uint8_t)

	static const Property EngineOilChangeIndicator;
	PROPERTYTYPEBASIC(EngineOilChangeIndicator, bool)

	/**< Vehicle Identification Number (ISO 3779) 17 chars**/
	static const Property VIN;
	PROPERTYTYPE(VIN, VINType, StringPropertyType, std::string)
	//class VINType: public StringPropertyType { public: VINType(std::string val):StringPropertyType(VIN,val){} };

	/**< World Manufacturer Identifier (SAE) 3 characters. */
	static const Property WMI;
	PROPERTYTYPE(WMI, WMIType, StringPropertyType, std::string)
	//class WMIType: public StringPropertyType { public: WMIType(std::string val):StringPropertyType(WMI,val){} };

	/**< Tire pressure in kPa */
	static const Property TirePressure;
	PROPERTYTYPEBASIC(TirePressure, uint16_t)

	/**< Tire pressure low */
	static const Property TirePressureLow;
	PROPERTYTYPEBASIC(TirePressureLow, bool)

	/**< Tire temperature in degrees C */
	static const Property TireTemperature;
	PROPERTYTYPEBASIC(TireTemperature, int16_t)

	/**< Vehicle Power Mode.
	 *@see Power::PowerModes
	 */
	static const Property VehiclePowerMode;
	PROPERTYTYPE(VehiclePowerMode, VehiclePowerModeType, BasicPropertyType<Power::PowerModes> ,Power::PowerModes)
	//typedef BasicPropertyType<Power::PowerModes> VehiclePowerModeType;

	static const Property TripMeters;
	PROPERTYTYPE(TripMeters, TripMetersType, ListPropertyType<BasicPropertyType<uint16_t> >, AbstractPropertyType*)
	//typedef ListPropertyType<BasicPropertyType<uint16_t> > TripMetersType;

	static const Property CruiseControlActive;
	PROPERTYTYPE(CruiseControlActive, CruiseControlActiveType, BasicPropertyType<bool>, bool)
	//typedef BasicPropertyType<bool> CruiseControlActiveType;

	static const Property CruiseControlSpeed;
	PROPERTYTYPE(CruiseControlSpeed, CruiseControlSpeedType, BasicPropertyType<uint16_t>, uint16_t)
	//typedef BasicPropertyType<uint16_t> CruiseControlSpeedType;

	static const Property LightHead;
	PROPERTYTYPE(LightHead, LightHeadType, BasicPropertyType<bool>, bool)
	static const Property LightRightTurn;
	PROPERTYTYPE(LightRightTurn, LightRightTurnType, BasicPropertyType<bool>, bool)
	static const Property LightLeftTurn;
	PROPERTYTYPE(LightLeftTurn, LightLeftTurnType, BasicPropertyType<bool>, bool)
	static const Property LightBrake;
	PROPERTYTYPE(LightBrake, LightBrakeType, BasicPropertyType<bool>, bool)
	static const Property LightFog;
	PROPERTYTYPE(LightFog, LightFogType, BasicPropertyType<bool>, bool)
	static const Property LightHazard;
	PROPERTYTYPE(LightHazard, LightHazardType, BasicPropertyType<bool>, bool)
	static const Property LightParking;
	PROPERTYTYPE(LightParking, LightParkingType, BasicPropertyType<bool>, bool)
	static const Property LightHighBeam;
	PROPERTYTYPE(LightHighBeam, LightHighBeamType, BasicPropertyType<bool>, bool)
	static const Property LightAutomatic;
	PROPERTYTYPEBASIC(LightAutomatic, bool)
	static const Property LightDynamicHighBeam;
	PROPERTYTYPEBASIC(LightDynamicHighBeam, bool)

	///TODO: deprecated.  Use InteriorLightStatus which is zoned.  Remove in 0.14
	static const Property InteriorLightDriver;
	PROPERTYTYPE(InteriorLightDriver, InteriorLightDriverType, BasicPropertyType<bool>, bool)
	///TODO: deprecated.  Use InteriorLightStatus which is zoned.  Remove in 0.14
	static const Property InteriorLightCenter;
	PROPERTYTYPE(InteriorLightCenter, InteriorLightCenterType, BasicPropertyType<bool>, bool)
	///TODO: deprecated.  Use InteriorLightStatus which is zoned.  Remove in 0.14
	static const Property InteriorLightPassenger;
	PROPERTYTYPE(InteriorLightPassenger, InteriorLightPassengerType, BasicPropertyType<bool>, bool)

	static const Property InteriorLightStatus;
	PROPERTYTYPEBASIC(InteriorLightStatus, bool)

	static const Property EngineLoad;
	PROPERTYTYPE(EngineLoad, EngineLoadType, BasicPropertyType<uint16_t>, uint16_t)

	static const Property Horn;
	PROPERTYTYPE(Horn, HornType, BasicPropertyType<bool>, bool)

	static const Property FuelLevel;
	PROPERTYTYPE(FuelLevel, FuelLevelType, BasicPropertyType<uint16_t>, uint16_t)

	static const Property FuelRange;
	PROPERTYTYPE(FuelRange, FuelRangeType, BasicPropertyType<uint16_t> , uint16_t)

	static const Property FuelConsumption;
	PROPERTYTYPE(FuelConsumption, FuelConsumptionType, BasicPropertyType<uint>, uint)

	static const Property FuelEconomy;
	PROPERTYTYPE(FuelEconomy, FuelEconomyType, BasicPropertyType<uint16_t>, uint16_t)

	static const Property FuelAverageEconomy;
	PROPERTYTYPE(FuelAverageEconomy, FuelAverageEconomyType, BasicPropertyType<uint16_t> , uint16_t)

	static const Property FuelAverageConsumption;
	PROPERTYTYPEBASIC(FuelAverageConsumption, uint)

	static const Property FuelTimeSinceRestart;
	PROPERTYTYPEBASIC(FuelTimeSinceRestart, uint)

	static const Property FuelConsumptionSinceRestart;
	PROPERTYTYPEBASIC(FuelConsumptionSinceRestart, uint)

	static const Property FuelType;
	PROPERTYTYPE(FuelType, FuelTypeType, BasicPropertyType<Fuel::Type>, Fuel::Type)

	static const Property FuelPositionSide;
	PROPERTYTYPE(FuelPositionSide, FuelPositionSideType, BasicPropertyType<Fuel::RefuelPosition>, Fuel::RefuelPosition)

	static const Property ExteriorBrightness;
	PROPERTYTYPE(ExteriorBrightness, ExteriorBrightnessType, BasicPropertyType<uint16_t>, uint16_t)

	static const Property Latitude;
	PROPERTYTYPE(Latitude, LatitudeType, BasicPropertyType<double>, double)

	static const Property Longitude;
	PROPERTYTYPE(Longitude, LongitudeType, BasicPropertyType<double>, double)

	static const Property Altitude;
	PROPERTYTYPE(Altitude, AltitudeType, BasicPropertyType<double> , double)

	static const Property Direction;
	PROPERTYTYPE(Direction, DirectionType, BasicPropertyType<uint16_t>, uint16_t)

	static const Property VehicleWidth;
	PROPERTYTYPE(VehicleWidth, VehicleWidthType, BasicPropertyType<uint>, uint)
	static const Property VehicleHeight;
	PROPERTYTYPE(VehicleHeight, VehicleHeightType, BasicPropertyType<uint>, uint)
	static const Property VehicleLength;
	PROPERTYTYPE(VehicleLength, VehicleLengthType, BasicPropertyType<uint>, uint)
	//typedef BasicPropertyType<uint> VehicleSizeType;

	static const Property VehicleType;
	PROPERTYTYPE(VehicleType, VehicleTypeType, BasicPropertyType<Vehicle::Type>, Vehicle::Type)
	//typedef BasicPropertyType<Vehicle::Type> VehicleTypeType;

	static const Property DoorsPerRow;
	PROPERTYTYPE(DoorsPerRow, DoorsPerRowType, ListPropertyType<BasicPropertyType<uint16_t> >, AbstractPropertyType*)
	//typedef ListPropertyType<BasicPropertyType<uint16_t> > DoorsPerRowType;

	static const Property TransmissionGearType;
	PROPERTYTYPE(TransmissionGearType, TransmissionGearTypeType, BasicPropertyType<Transmission::Type>, Transmission::Type)
	//typedef BasicPropertyType<Transmission::Type> TransmissionGearTypeType;

	static const Property FrontWheelRadius;
	PROPERTYTYPE(FrontWheelRadius, FrontWheelRadiusType, BasicPropertyType<uint16_t>, uint16_t)
	static const Property RearWheelRadius;
	PROPERTYTYPE(RearWheelRadius, RearWheelRadiusType, BasicPropertyType<uint16_t>, uint16_t)
//	/typedef BasicPropertyType<uint16_t> WheelRadiusType;

	static const Property WheelTrack;
	PROPERTYTYPE(WheelTrack, WheelTrackType, BasicPropertyType<uint>, uint)
//	typedef BasicPropertyType<uint> WheelTrackType;

	static const Property BrakePressure;
	PROPERTYTYPEBASIC(BrakePressure, uint16_t)
	//typedef BasicPropertyType<uint16_t> BrakePressureType;

	/// TODO: deprecated.  remove in 0.13.  Use DistanceTotal
	static const Property Odometer;
	PROPERTYTYPEBASIC(Odometer, uint)

	static const Property DistanceTotal;
	PROPERTYTYPEBASIC(DistanceTotal, uint)

	static const Property DistanceSinceStart;
	PROPERTYTYPEBASIC(DistanceSinceStart, uint)

	/**< Transmission Fluid Level 0-100%.
	 **/
	static const Property TransmissionFluidLevel;
	PROPERTYTYPEBASIC(TransmissionFluidLevel, uint16_t)
	//typedef BasicPropertyType<uint16_t> TransmissionFluidLevelType;

	/**< Brake Fluid Level 0-100%.
	 **/
	static const Property BrakeFluidLevel;
	PROPERTYTYPEBASIC(BrakeFluidLevel, uint16_t)
	//typedef BasicPropertyType<uint16_t> BrakeFluidLevelType;

	/**< Washer Fluid Level 0-100%.
	 **/
	static const Property WasherFluidLevel;
	PROPERTYTYPEBASIC(WasherFluidLevel, uint16_t)
	//typedef BasicPropertyType<uint16_t> WasherFluidLevelType;

	/**< Securty Alert Status
	 * status of security alert
	 * @see Security::Status
	 */
	static const Property SecurityAlertStatus;
	PROPERTYTYPEBASIC(SecurityAlertStatus, Security::Status)
	//typedef BasicPropertyType<Security::Status> SecurityAlertStatusType;

	/**< Parking Brake Status
	 * status of parking break active (true) or inactive (false)
	 */
	static const Property ParkingBrakeStatus;
	PROPERTYTYPEBASIC(ParkingBrakeStatus, bool)
	//typedef BasicPropertyType<bool> ParkingBrakeStatusType;

	/**< Parking Light Status
	 * status of parking lights active (true) or inactive (false)
	 */
	static const Property ParkingLightStatus;
	PROPERTYTYPEBASIC(ParkingLightStatus,bool)
	//typedef BasicPropertyType<bool> ParkingLightStatusType;

	/**< Hazard Lights Status
	 * status of parking lights active (true) or inactive (false)
	 */
	static const Property HazardLightStatus;
	PROPERTYTYPEBASIC(HazardLightStatus, bool)
	//typedef BasicPropertyType<bool> HazardLightStatusType;

	static const Property AntilockBrakingSystem;
	PROPERTYTYPEBASIC(AntilockBrakingSystem, bool)
	//typedef BasicPropertyType<bool> AntilockBrakingSystemType;

	static const Property TractionControlSystem;
	PROPERTYTYPEBASIC(TractionControlSystem, bool)
	//typedef BasicPropertyType<bool> TractionControlSystemType;

	static const Property VehicleTopSpeedLimit;
	PROPERTYTYPEBASIC(VehicleTopSpeedLimit,uint16_t)
	//typedef BasicPropertyType<uint16_t> VehicleTopSpeedLimitType;

	static const Property AirbagStatus;
	PROPERTYTYPEBASIC(AirbagStatus, Airbag::Status)

	/// TODO: deprecate in 0.13.  Use DoorStatusW3C
	static const Property DoorStatus;
	PROPERTYTYPEBASIC(DoorStatus, Door::Status)

	/** use with Door::W3C::*
	 */
	static const Property DoorStatusW3C;
	PROPERTYTYPE(DoorStatusW3C, DoorStatusW3CType, StringPropertyType, std::string)

	static const Property DoorLockStatus;
	PROPERTYTYPEBASIC(DoorLockStatus, bool)

	static const Property ChildLockStatus;
	PROPERTYTYPEBASIC(ChildLockStatus, bool)

	static const Property SeatBeltStatus;
	PROPERTYTYPEBASIC(SeatBeltStatus, bool)

	static const Property WindowLockStatus;
	PROPERTYTYPEBASIC(WindowLockStatus, bool)

	static const Property OccupantStatus;
	PROPERTYTYPEBASIC(OccupantStatus, Seat::Occupant)

	static const Property ObstacleDistance;
	PROPERTYTYPEBASIC(ObstacleDistance, double)

	static const Property RainSensor;
	PROPERTYTYPEBASIC(RainSensor, uint16_t)
	//typedef BasicPropertyType<uint16_t> RainSensorType;

	static const Property WindshieldWiper;
	PROPERTYTYPEBASIC(WindshieldWiper,Window::WiperSpeed)
	//typedef BasicPropertyType<Window::WiperSpeed> WindshieldWiperType;

	/// TODO: Deprecated.  Remove in 0.13
	static const Property AirflowDirection;
	PROPERTYTYPEBASIC(AirflowDirection,HVAC::AirflowDirection)

	static const Property AirflowDirectionW3C;
	PROPERTYTYPE(AirflowDirectionW3C, AirflowDirectionW3CType, StringPropertyType, std::string)

	static const Property FanSpeed;
	PROPERTYTYPEBASIC(FanSpeed, uint16_t)

	static const Property TargetTemperature;
	PROPERTYTYPEBASIC(TargetTemperature, int)

	static const Property AirConditioning;
	PROPERTYTYPEBASIC(AirConditioning,bool)

	static const Property AirRecirculation;
	PROPERTYTYPEBASIC(AirRecirculation,bool)

	static const Property Heater;
	PROPERTYTYPEBASIC(Heater, bool)

	/// TODO: deprecated. remove in 0.13
	static const Property Defrost;
	PROPERTYTYPEBASIC(Defrost, bool )

	static const Property DefrostWindow;
	PROPERTYTYPEBASIC(DefrostWindow, bool )

	static const Property DefrostMirror;
	PROPERTYTYPEBASIC(DefrostMirror, bool )

	static const Property SteeringWheelHeater;
	PROPERTYTYPEBASIC(SteeringWheelHeater,bool)
	//typedef BasicPropertyType<bool> SteeringWheelHeaterType;

	static const Property SeatHeater;
	PROPERTYTYPEBASIC(SeatHeater,int)
	//typedef BasicPropertyType<bool> SeatHeaterType;

	static const Property SeatCooler;
	PROPERTYTYPEBASIC(SeatCooler,bool)
//	/typedef BasicPropertyType<bool> SeatCoolerType;

	static const Property WindowStatus;
	PROPERTYTYPEBASIC(WindowStatus, uint16_t)

	static const Property Sunroof;
	PROPERTYTYPEBASIC(Sunroof, uint16_t)
	//typedef BasicPropertyType<uint16_t> SunroofType;

	static const Property SunroofTilt;
	PROPERTYTYPEBASIC(SunroofTilt,uint16_t)
	//typedef BasicPropertyType<uint16_t> SunroofTiltType;

	static const Property ConvertibleRoof;
	PROPERTYTYPEBASIC(ConvertibleRoof, bool)
	//typedef BasicPropertyType<bool> ConvertibleRoofType;

	static const Property NightMode;
	PROPERTYTYPEBASIC(NightMode, bool)

	static const Property DrivingMode;
	PROPERTYTYPEBASIC(DrivingMode, Driving::Mode)

	static const Property DrivingModeW3C;
	PROPERTYTYPEBASIC(DrivingModeW3C, bool)

	static const Property KeyId;
	PROPERTYTYPE(KeyId, KeyIdType, StringPropertyType, std::string)

	static const Property Language;
	PROPERTYTYPE(Language, LanguageType, StringPropertyType, std::string)

	static const Property MeasurementSystem;
	PROPERTYTYPEBASIC(MeasurementSystem, Measurement::Type)

	static const Property MirrorSettingPan;
	static const Property MirrorSettingTilt;

	PROPERTYTYPEBASIC(MirrorSettingPan, uint16_t)
	PROPERTYTYPEBASIC(MirrorSettingTilt, uint16_t)

	static const Property SteeringWheelPositionSlide;
	static const Property SteeringWheelPositionTilt;

	PROPERTYTYPEBASIC(SteeringWheelPositionSlide, uint16_t)
	PROPERTYTYPEBASIC(SteeringWheelPositionTilt, uint16_t)

	static const Property SeatPositionRecline;
	static const Property SeatPositionSlide;
	static const Property SeatPositionCushionHeight;
	static const Property SeatPositionHeadrest;
	static const Property SeatPositionBackCushion;
	static const Property SeatPositionSideCushion;

	PROPERTYTYPEBASIC(SeatPositionRecline, uint16_t)
	PROPERTYTYPEBASIC(SeatPositionSlide, uint16_t)
	PROPERTYTYPEBASIC(SeatPositionCushionHeight, uint16_t)
	PROPERTYTYPEBASIC(SeatPositionHeadrest, uint16_t)
	PROPERTYTYPEBASIC(SeatPositionBackCushion, uint16_t)
	PROPERTYTYPEBASIC(SeatPositionSideCushion, uint16_t)

	static const Property DashboardIllumination;
	PROPERTYTYPEBASIC(DashboardIllumination, uint16_t)

	static const Property GeneratedVehicleSoundMode;
	PROPERTYTYPEBASIC(GeneratedVehicleSoundMode, Vehicle::SoundMode)

	static const Property DriverId;
	PROPERTYTYPE(DriverId, DriverIdType, StringPropertyType, std::string)

	static const Property PowertrainTorque;
	PROPERTYTYPEBASIC(PowertrainTorque, uint16_t)

	static const Property AcceleratorPedalPosition;
	PROPERTYTYPEBASIC(AcceleratorPedalPosition, uint8_t)

	static const Property Chime;
	PROPERTYTYPEBASIC(Chime, bool)

	static const Property WheelTick;
	PROPERTYTYPEBASIC(WheelTick, uint)

	static const Property IgnitionTimeOn;
	PROPERTYTYPEBASIC(IgnitionTimeOn, uint64_t)

	static const Property IgnitionTimeOff;
	PROPERTYTYPEBASIC(IgnitionTimeOff, uint64_t)

	static const Property YawRate;
	PROPERTYTYPEBASIC(YawRate, int16_t)

	/** END PROPERTIES **/


	/*!
	 * \brief capabilities
	 * \return returns list of all registered properties
	 * \see VehicleProperty::registerProperty
	 */
	static PropertyList capabilities();

	/*!
	 * \brief customProperties
	 * \return returns list of custom properties defined by plugins using VehicleProperty::registerProperty
	 */
	static PropertyList customProperties();

	/*! \brief getPropertyTypeForPropertyNameValue returns an AbstractPropertyType* for the property name
	  * with the value specified by 'value'.  Ownership of the returned AbstractPropertyType* is
	  * transfered to the caller.
	  */
	static AbstractPropertyType* getPropertyTypeForPropertyNameValue(Property name, std::string value="");

	/*! \brief registerProperty registers properties with the Vehicle Property system.  Returns true if property
	 *  has been registered successfully.
	 *  \param name - name of property.  Name cannot match any existing property or it will be rejected and
	 *  this method will return false.
	 *  \param factor - callback function that returns an AbstractPropertyType representation of the value.
	 *  custom properties will need to return a custom AbstractPropertyType based object.
	 *  \example :
	 *
	 *  #include <vehicleproperty.h>
	 *  #include <abstractpropertytype.h>
	 *
	 *  //Somewhere in a source plugin:
	 *  ...
	 *  Property VehicleJetEngineStatus = "VehicleJetEngineStatus";
	 *  VehicleProperty::registerProperty(VehicleJetEngineStatus, [](){return new BasicPropertyType<bool>(VehicleJetEngineStatus, false);});
	 *  ...
	 *  //other initialization
	 */
	static bool registerProperty(Property name, PropertyTypeFactoryCallback factory);



private:

	VehicleProperty();

	static std::unique_ptr<VehicleProperty> thereCanOnlyBeOne;

	static bool registerPropertyPriv(Property name, PropertyTypeFactoryCallback factory);

	static std::map<Property, PropertyTypeFactoryCallback> registeredPropertyFactoryMap;
	static PropertyList mCapabilities;
	static PropertyList mCustomProperties;
};

#endif // VEHICLEPROPERTY_H

