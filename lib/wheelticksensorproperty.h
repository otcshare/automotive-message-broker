/*
    <one line to give the library's name and an idea of what it does.>
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

#ifndef WHEELTICKSENSORPROPERTY_H
#define WHEELTICKSENSORPROPERTY_H

#include <abstractproperty.h>

class WheelTickSensor
{
public:
	WheelTickSensor(): right(0), left(0) { }

	WheelTickSensor(WheelTickSensor const & other)
	{
		right = other.right;
		left = other.left;
	}
	WheelTickSensor & operator = (WheelTickSensor const & other)
	{
		right = other.right;
		left = other.left;
		return *this;
	}

	uint16_t right;
	uint16_t left;
};

class WheelTickSensorProperty : public AbstractProperty
{

public:
	operator WheelTickSensor()
	{
		return value<WheelTickSensor>();
	}

	WheelTickSensorProperty & operator = (WheelTickSensor const & v)
	{
		setValue<WheelTickSensor>(v);
		return *this;
	}

	virtual void fromGVariant(GVariant* value);
	virtual GVariant* toGVariant();
	WheelTickSensorProperty();
};

#endif // WHEELTICKSENSORPROPERTY_H
