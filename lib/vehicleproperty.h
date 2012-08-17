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



class VehicleProperty
{

public:
	

	VehicleProperty();

	enum Property
	{
		NoValue=0,
		VehicleSpeed, /**< Vehicle Velocity in km/h */
		EngineSpeed, /**< Engine Speed in rotations per minute */
		TransmissionShiftPosition, /**< Transmission Shift Position
									* 0 = Neutral
									* 1 = 1st
									* 2 = 2nd
									* ...
									* 64 = Drive
									* 128 = Reverse
									* 255 = Park
									*/

		ThrottlePosition, /**< Throttle position 0-100% */
		WheelBrake, /**< Wheel brake position.  Engaged = true, Idle = false */
		SteeringWheelAngle, /**< Steering wheel angle (0-360) */
		TurnSignal, /**< 0=off, 1=right, 2=left, 3=hazard */
		MachineGunTurretStatus /**< 0=off, 1=on */
	};

	static std::string name(Property prop);
	static Property value(std::string name);
    
};

typedef std::list<VehicleProperty::Property> PropertyList;

#endif // VEHICLEPROPERTY_H
