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

#include <string>
#include <list>
#include <set>

#include <abstractpropertytype.h>

class VehicleProperty
{

public:
	

	VehicleProperty();

	typedef std::string Property;

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
	 * 64 = Drive
	 * 128 = Reverse
	 * 255 = Park
	 */
	static const Property TransmissionShiftPosition;
	typedef BasicPropertyType<unsigned char> TransmissionShiftPositionType;

	/**< Transmission Gear Position
	* 0 = Neutral
	* 1 = 1st
	* 2 = 2nd
	* ...
	* 64 = CVT
	*/
	static const Property TransmissionGearPosition;
	typedef BasicPropertyType<unsigned char> TransmissionGearPositionType;

	/**< Throttle position 0-100% */
	static const Property ThrottlePosition;
	typedef BasicPropertyType<unsigned char> ThrottlePositionType;

	/**< Wheel brake position.  Engaged = true, Idle = false */
	static const Property WheelBrake;
	typedef BasicPropertyType<bool> WheelBrakeType;

	/**< Steering wheel angle (0-359) */
	static const Property SteeringWheelAngle;
	typedef BasicPropertyType<uint16_t> SteeringWheelAngleType;

	/**< 0=off, 1=right, 2=left, 3=hazard */
	static const Property TurnSignal;
	typedef BasicPropertyType<unsigned char> TurnSignalType;

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

	static std::list<VehicleProperty::Property> capabilities();

	static AbstractPropertyType getPropertyTypeForPropertyNameValue(Property name, std::string value);
    
};

typedef std::list<VehicleProperty::Property> PropertyList;
typedef std::set<VehicleProperty::Property> PropertySet;

#endif // VEHICLEPROPERTY_H
