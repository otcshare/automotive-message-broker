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

#ifndef ENGINECOOLANTPROPERTY_H
#define ENGINECOOLANTPROPERTY_H

#include <abstractproperty.h>

class EngineCoolant
{
public:
	EngineCoolant(): temperature(0), level(0) {}
	EngineCoolant(EngineCoolant const & other)
	{
		temperature = other.temperature;
		level = other.level;
	}

	EngineCoolant& operator = (EngineCoolant const &other)
	{
		temperature = other.temperature;
		level = other.level;
		return *this;
	}

	bool operator == (const EngineCoolant& other) const
	{
		return temperature == other.temperature && level == other.level;
	}



	uint8_t temperature;
	uint8_t level;
};

class EngineCoolantProperty : public AbstractProperty
{

public:
	virtual void fromGVariant(GVariant* value);
	virtual GVariant* toGVariant();
	EngineCoolantProperty();

	EngineCoolantProperty& operator=(EngineCoolant const & other)
	{
		setValue<EngineCoolant>(other);
		return *this;
	}

	operator EngineCoolant()
	{
		return value<EngineCoolant>();
	}

};

#endif // ENGINECOOLANTPROPERTY_H
