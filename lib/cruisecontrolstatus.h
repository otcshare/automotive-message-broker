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


#ifndef CRUISECONTROLSTATUS_H
#define CRUISECONTROLSTATUS_H

#include <abstractproperty.h>

class CruiseControlStatus
{
public:
	CruiseControlStatus() : activated(false), speed(0) { }
	
	CruiseControlStatus(bool active, uint16_t spd)
	: activated(active), speed(spd)
	{ }
	
	CruiseControlStatus(const CruiseControlStatus& other)
	{
		activated = other.activated;
		speed = other.speed;
	}
	
	CruiseControlStatus operator = (CruiseControlStatus other)
	{
		activated = other.activated;
		speed = other.speed;
		
		return *this;
	}
	
	bool operator == (CruiseControlStatus &other)
	{
		return activated == other.activated && speed == other.speed;
	}
	
	bool activated;
	uint16_t speed;
	
};

class CruiseControlStatusProperty : public AbstractProperty
{
public:
	void setValue(CruiseControlStatus status);
	CruiseControlStatus value();
	
	virtual void fromGVariant(GVariant* value);
	virtual GVariant* toGVariant();
	CruiseControlStatusProperty();
};

#endif // CRUISECONTROLSTATUS_H
