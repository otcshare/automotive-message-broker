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

class VehicleProperty
{

public:
	

	VehicleProperty();

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

	/**< Interior Air Temperature in degrees celcius */
	static const Property InteriorTemperature;
	typedef BasicPropertyType<int> InteriorTemperatureType;

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
	typedef BasicPropertyType<float> TirePressureType;

	/**< Tire temperature in degrees C */
	static const Property TireTemperatureLeftFront;
	static const Property TireTemperatureRightFront;
	static const Property TireTemperatureLeftRear;
	static const Property TireTemperatureRightRear;
	typedef BasicPropertyType<float> TireTemperatureType;
	
	/**< Vehicle Power Mode.
	 *@see Power::PowerModes
	 */
	static const Property VehiclePowerMode;
	typedef BasicPropertyType<Power::PowerModes> VehiclePowerModeType;

	static const Property TripMeterA;
	static const Property TripMeterB;
	static const Property TripMeterC;
	typedef BasicPropertyType<uint16_t> TripMeterType;

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

	static std::list<VehicleProperty::Property> capabilities();

	/*! getPropertyTypeForPropertyNameValue returns an AbstractPropertyType* for the property name
	  * with the value specified by 'value'.  Ownership of the returned AbstractPropertyType* is
	  * transfered to the caller.
	  */
	static AbstractPropertyType* getPropertyTypeForPropertyNameValue(Property name, std::string value);

	/*! registerProperty registers properties with the Vehicle Property system.  Returns true if property
	 *  has been registered successfully.
	 *  @param name - name of property.  Name cannot match any existing property or it will be rejected and
	 *  this method will return false.
	 *  @param factor - callback function that returns an AbstractPropertyType representation of the value.
	 *  custom properties will need to return an AbstractPropertyType based object.
	 *  @example :
	 *  //In the constructor of a source plugin:
	 *  ...
	 *  Property VehicleJetEngineStatus = "VehicleJetEngineStatus";
	 *  VehicleProperty::registerProperty(VehicleJetEngineStatus, [](){return new BasicPropertyType<bool>(false);});
	 *  ...
	 *  //other initialization
	 */
	static bool registerProperty(Property name, PropertyTypeFactoryCallback factory);

private:

	static std::map<Property, PropertyTypeFactoryCallback> registeredPropertyFactoryMap;
	static std::list<VehicleProperty::Property> mCapabilities;
};

typedef std::list<VehicleProperty::Property> PropertyList;
typedef std::set<VehicleProperty::Property> PropertySet;

#endif // VEHICLEPROPERTY_H
