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
#include <abstractpropertytype.h>
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
enum Location
{
	Driver = 0,
	Passenger,
	LeftSide,
	RightSide
};

enum Status
{
	Inactive = 0,
	Active,
	Deployed
};

}

namespace Door {
enum Location
{
	Driver=0,
	Passenger,
	LeftRear,
	RightRear,
	Trunk,
	FuelCap,
	Hood
};

enum Status
{
	Closed = 0,
	Open,
	Ajar
};

}

namespace Seat
{
enum Location
{
	Driver = 0,
	FrontMiddle = 1,
	Passenger,
	LeftRear,
	MiddleRear,
	RightRear
};

enum Status
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
	SideMirrorRight
};

enum WiperSpeed
{
	Off = 0,
	Slowest = 1,
	Fastest = 5,
	Auto = 10
};

}

namespace DistanceSensor
{
enum Location
{
	LeftFront = 0,
	RightFront = 1,
	LeftRear = 2,
	RightRear = 3,
	LeftBlindSpot = 4,
	RightBlindSPot = 5

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
}

class VehicleProperty
{

public:
	

	static void factory();

	typedef std::string Property;
	typedef std::function<AbstractPropertyType* (void)> PropertyTypeFactoryCallback;

	/// Various property types:

	static const Property NoValue;

	/**< Vehicle Velocity in km/h */
	static const Property VehicleSpeed;
	typedef BasicPropertyType<uint16_t> VehicleSpeedType;

	/**< Engine Speed in rotations per minute */
	static const Property EngineSpeed;
	typedef BasicPropertyType<uint16_t> EngineSpeedType;

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
	typedef BasicPropertyType<Transmission::TransmissionPositions> TransmissionShiftPositionType;

	/**< Transmission Gear Position
	* 0 = Neutral
	* 1 = 1st
	* 2 = 2nd
	* ...
	* 64 = CVT
	* 128 = Reverse
	*/
	static const Property TransmissionGearPosition;
	typedef BasicPropertyType<Transmission::TransmissionPositions> TransmissionGearPositionType;

	static const Property TransmissionMode;
	typedef BasicPropertyType<Transmission::Mode> TransmissionModeType;

	/**< Throttle position 0-100% */
	static const Property ThrottlePosition;
	typedef BasicPropertyType<uint16_t> ThrottlePositionType;

	/**< Wheel brake position.  Engaged = true, Idle = false */
	static const Property WheelBrake;
	typedef BasicPropertyType<bool> WheelBrakeType;

	static const Property WheelBrakePressure;
	typedef BasicPropertyType<uint16_t> WheelBrakePressureType;

	/**< Steering wheel angle (0-359) */
	static const Property SteeringWheelAngle;
	typedef BasicPropertyType<uint16_t> SteeringWheelAngleType;

	/**< 0=off, 1=right, 2=left, 3=hazard */
	static const Property TurnSignal;
	typedef BasicPropertyType<TurnSignals::TurnSignalType> TurnSignalType;

	/**< Clutch pedal status 0=off, 1=on */
	static const Property ClutchStatus;
	typedef BasicPropertyType<bool> ClutchStatusType;

	 /**< Oil pressure TODO: units */
	static const Property EngineOilPressure;
	typedef BasicPropertyType<uint16_t> EngineOilPressureType;

	/**< Engine coolant temperature in degrees celcius **/
	static const Property EngineCoolantTemperature;
	typedef BasicPropertyType<int> EngineCoolantTemperatureType;

	/**< 0=off, 1=on */
	static const Property MachineGunTurretStatus;
	typedef BasicPropertyType<bool> MachineGunTurretStatusType;

	/**< Acceleration on the 'x' axis in 1/1000 gravitational acceleration "g-force" */
	static const Property AccelerationX;
	typedef BasicPropertyType<uint16_t> AccelerationType;

	/**< Acceleration on the 'y' axis in 1/1000 gravitational acceleration "g-force" */
	static const Property AccelerationY;

	/**< Acceleration on the 'z' axis in 1/1000 gravitational acceleration "g-force" */
	static const Property AccelerationZ;

	/**< Mass Air Flow.  TODO: units */
	static const Property MassAirFlow;
	typedef BasicPropertyType<uint16_t> MassAirFlowType;

	/**< Button Event @see ButtonEvents::ButtonEventType */
	static const Property ButtonEvent;
	typedef BasicPropertyType<ButtonEvents::ButtonEventType> ButtonEventType;

	/**< Air intake temperature in degrees celcius */
	static const Property AirIntakeTemperature;
	typedef BasicPropertyType<int> AirIntakeTemperatureType;

	/**< Battery voltage in volts */
	static const Property BatteryVoltage;
	typedef BasicPropertyType<double> BatteryVoltageType;

	static const Property BatteryCurrent;
	typedef BasicPropertyType<double> BatteryCurrentType;

	/**< Interior Air Temperature in degrees celcius */
	static const Property InteriorTemperature;
	typedef BasicPropertyType<int> InteriorTemperatureType;

	static const Property ExteriorTemperature;
	typedef BasicPropertyType<int> ExteriorTemperatureType;

	/**< Engine Oil Temperature in degrees celcius */
	static const Property EngineOilTemperature;
	typedef BasicPropertyType<int> EngineOilTemperatureType;

	static const Property EngineOilRemaining;
	typedef BasicPropertyType<uint16_t> EngineOilRemainingType;

	/**< Vehicle Identification Number (ISO 3779) 17 chars**/
	static const Property VIN;
	typedef StringPropertyType VINType;

	/**< World Manufacturer Identifier (SAE) 3 characters. */
	static const Property WMI;
	typedef StringPropertyType WMIType;

	/**< Tire pressure in kPa */
	static const Property TirePressureLeftFront;
	static const Property TirePressureRightFront;
	static const Property TirePressureLeftRear;
	static const Property TirePressureRightRear;
	typedef BasicPropertyType<double> TirePressureType;

	/**< Tire temperature in degrees C */
	static const Property TireTemperatureLeftFront;
	static const Property TireTemperatureRightFront;
	static const Property TireTemperatureLeftRear;
	static const Property TireTemperatureRightRear;
	typedef BasicPropertyType<double> TireTemperatureType;
	
	/**< Vehicle Power Mode.
	 *@see Power::PowerModes
	 */
	static const Property VehiclePowerMode;
	typedef BasicPropertyType<Power::PowerModes> VehiclePowerModeType;

	static const Property TripMeters;
	typedef ListPropertyType<BasicPropertyType<uint16_t> > TripMetersType;

	static const Property CruiseControlActive;
	typedef BasicPropertyType<bool> CruiseControlActiveType;

	static const Property CruiseControlSpeed;
	typedef BasicPropertyType<uint16_t> CruiseControlSpeedType;

	static const Property LightHead;
	static const Property LightRightTurn;
	static const Property LightLeftTurn;
	static const Property LightBrake;
	static const Property LightFog;
	static const Property LightHazard;
	static const Property LightParking;
	static const Property LightHighBeam;
	typedef BasicPropertyType<bool> LightStatusType;
	static const Property InteriorLightDriver;
	static const Property InteriorLightCenter;
	static const Property InteriorLightPassenger;

	static const Property EngineLoad;
	typedef BasicPropertyType<uint16_t> EngineLoadType;

	static const Property Horn;
	typedef BasicPropertyType<bool> HornType;

	static const Property FuelLevel;
	typedef BasicPropertyType<uint16_t> FuelLevelType;

	static const Property FuelRange;
	typedef BasicPropertyType<uint16_t> FuelRangeType;

	static const Property FuelConsumption;
	typedef BasicPropertyType<uint16_t> FuelConsumptionType;

	static const Property FuelEconomy;
	typedef BasicPropertyType<uint16_t> FuelEconomyType;

	static const Property FuelAverageEconomy;
	typedef BasicPropertyType<uint16_t> FuelAverageEconomyType;

	static const Property FuelType;
	typedef BasicPropertyType<Fuel::Type> FuelTypeType;

	static const Property FuelPositionSide;
	typedef BasicPropertyType<Fuel::RefuelPosition> FuelPositionSideType;

	static const Property ExteriorBrightness;
	typedef BasicPropertyType<uint16_t> ExteriorBrightnessType;
	
	static const Property Latitude;
	typedef BasicPropertyType<double> LatitudeType;

	static const Property Longitude;
	typedef BasicPropertyType<double> LongitudeType;

	static const Property Altitude;
	typedef BasicPropertyType<double> AltitudeType;

	static const Property Direction;
	typedef BasicPropertyType<uint16_t> DirectionType;

	static const Property VehicleWidth;
	static const Property VehicleHeight;
	static const Property VehicleLength;
	typedef BasicPropertyType<uint> VehicleSizeType;


	static const Property VehicleType;
	typedef BasicPropertyType<Vehicle::Type> VehicleTypeType;

	static const Property DoorsPerRow;
	typedef ListPropertyType<BasicPropertyType<uint16_t> > DoorsPerRowType;

	static const Property TransmissionGearType;
	typedef BasicPropertyType<Transmission::Type> TransmissionGearTypeType;

	static const Property FrontWheelRadius;
	static const Property RearWheelRadius;
	typedef BasicPropertyType<uint16_t> WheelRadiusType;

	static const Property WheelTrack;
	typedef BasicPropertyType<uint> WheelTrackType;

	static const Property BrakePressure;
	typedef BasicPropertyType<uint16_t> BrakePressureType;

	static const Property Odometer;
	typedef BasicPropertyType<uint> OdometerType;

	/**< Transmission Fluid Level 0-100%.
	 **/
	static const Property TransmissionFluidLevel;
	typedef BasicPropertyType<uint16_t> TransmissionFluidLevelType;

	/**< Brake Fluid Level 0-100%.
	 **/
	static const Property BrakeFluidLevel;
	typedef BasicPropertyType<uint16_t> BrakeFluidLevelType;

	/**< Washer Fluid Level 0-100%.
	 **/
	static const Property WasherFluidLevel;
	typedef BasicPropertyType<uint16_t> WasherFluidLevelType;

	/**< Securty Alert Status
	 * status of security alert
	 * @see Security::Status
	 */
	static const Property SecurityAlertStatus;
	typedef BasicPropertyType<Security::Status> SecurityAlertStatusType;

	/**< Parking Brake Status
	 * status of parking break active (true) or inactive (false)
	 */
	static const Property ParkingBrakeStatus;
	typedef BasicPropertyType<bool> ParkingBrakeStatusType;

	/**< Parking Light Status
	 * status of parking lights active (true) or inactive (false)
	 */
	static const Property ParkingLightStatus;
	typedef BasicPropertyType<bool> ParkingLightStatusType;

	/**< Hazard Lights Status
	 * status of parking lights active (true) or inactive (false)
	 */
	static const Property HazardLightStatus;
	typedef BasicPropertyType<bool> HazardLightStatusType;

	static const Property AntilockBrakingSystem;
	typedef BasicPropertyType<bool> AntilockBrakingSystemType;

	static const Property TractionControlSystem;
	typedef BasicPropertyType<bool> TractionControlSystemType;

	static const Property VehicleTopSpeedLimit;
	typedef BasicPropertyType<uint16_t> VehicleTopSpeedLimitType;

	static const Property AirbagStatus;
	typedef MapPropertyType<BasicPropertyType<Airbag::Location>, BasicPropertyType<Airbag::Status> > AirbagStatusType;

	static const Property DoorStatus;
	typedef MapPropertyType<BasicPropertyType<Door::Location>, BasicPropertyType<Door::Status> > DoorStatusType;

	static const Property DoorLockStatus;
	typedef MapPropertyType<BasicPropertyType<Door::Location>, BasicPropertyType<bool> > DoorLockStatusType;

	static const Property SeatBeltStatus;
	typedef MapPropertyType<BasicPropertyType<Seat::Location>, BasicPropertyType<bool> > SeatBeltStatusType;

	static const Property WindowLockStatus;
	typedef MapPropertyType<BasicPropertyType<Window::Location>, BasicPropertyType<bool> > WindowLockStatusType;

	static const Property OccupantStatus;
	typedef MapPropertyType<BasicPropertyType<Seat::Location>, BasicPropertyType<bool> > OccupantStatusType;

	static const Property ObstacleDistance;
	typedef MapPropertyType<BasicPropertyType<DistanceSensor::Location>, BasicPropertyType<double> > ObstacleDistanceType;

	static const Property RainSensor;
	typedef BasicPropertyType<uint16_t> RainSensorType;

	static const Property WindshieldWiper;
	typedef BasicPropertyType<Window::WiperSpeed> WindshieldWiperType;

	static const Property AirflowDirection;
	typedef BasicPropertyType<HVAC::AirflowDirection> AirflowDirectionType;

	static const Property FanSpeed;
	typedef BasicPropertyType<uint16_t> FanSpeedType;

	static const Property TargetTemperature;
	typedef BasicPropertyType<int> TargetTemperatureType;

	static const Property AirConditioning;
	typedef BasicPropertyType<bool> AirConditioningType;

	static const Property AirRecirculation;
	typedef BasicPropertyType<bool> AirRecirculationType;

	static const Property Heater;
	typedef BasicPropertyType<bool> HeaterType;

	static const Property Defrost;
	typedef MapPropertyType<BasicPropertyType<Window::Location>, BasicPropertyType<bool> > DefrostType;

	static const Property SteeringWheelHeater;
	typedef BasicPropertyType<bool> SteeringWheelHeaterType;

	static const Property SeatHeater;
	typedef BasicPropertyType<bool> SeatHeaterType;

	static const Property SeatCooler;
	typedef BasicPropertyType<bool> SeatCoolerType;

	static const Property WindowStatus;
	typedef MapPropertyType<BasicPropertyType<Window::Location>, BasicPropertyType<uint16_t> > WindowStatusType;

	static const Property Sunroof;
	typedef BasicPropertyType<uint16_t> SunroofType;

	static const Property SunroofTilt;
	typedef BasicPropertyType<uint16_t> SunroofTiltType;

	static const Property ConvertibleRoof;
	typedef BasicPropertyType<bool> ConvertibleRoofType;

	/** END PROPERTIES **/


	static std::list<VehicleProperty::Property> capabilities();
	static std::list<VehicleProperty::Property> customProperties();

	/*! getPropertyTypeForPropertyNameValue returns an AbstractPropertyType* for the property name
	  * with the value specified by 'value'.  Ownership of the returned AbstractPropertyType* is
	  * transfered to the caller.
	  */
	static AbstractPropertyType* getPropertyTypeForPropertyNameValue(Property name, std::string value="");

	/*! registerProperty registers properties with the Vehicle Property system.  Returns true if property
	 *  has been registered successfully.
	 *  @param name - name of property.  Name cannot match any existing property or it will be rejected and
	 *  this method will return false.
	 *  @param factor - callback function that returns an AbstractPropertyType representation of the value.
	 *  custom properties will need to return a custom AbstractPropertyType based object.
	 *  @example :
	 *
	 *  #include <vehicleproperty.h>
	 *  #include <abstractpropertytype.h>
	 *
	 *  //Somewhere in a source plugin:
	 *  ...
	 *  Property VehicleJetEngineStatus = "VehicleJetEngineStatus";
	 *  VehicleProperty::registerProperty(VehicleJetEngineStatus, [](){return new BasicPropertyType<bool>(false);});
	 *  ...
	 *  //other initialization
	 */
	static bool registerProperty(Property name, PropertyTypeFactoryCallback factory);



private:

	VehicleProperty();

	static VehicleProperty* thereCanOnlyBeOne;

	static bool registerPropertyPriv(Property name, PropertyTypeFactoryCallback factory);

	static std::map<Property, PropertyTypeFactoryCallback> registeredPropertyFactoryMap;
	static std::list<VehicleProperty::Property> mCapabilities;
	static std::list<VehicleProperty::Property> mCustomProperties;
};

typedef std::list<VehicleProperty::Property> PropertyList;
typedef std::set<VehicleProperty::Property> PropertySet;

#endif // VEHICLEPROPERTY_H
