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


#ifndef INTERIORLIGHTSTATUSPROPERTY_H
#define INTERIORLIGHTSTATUSPROPERTY_H

#include <abstractproperty.h>
#include <map>

class InteriorLightStatusProperty : public AbstractProperty
{

public:
	enum InteriorLight {
		Driver = 0,
		Passenger = 1,
		Center = 2
	};
	
	typedef std::map<InteriorLight, bool> InteriorLightStatus;
	
	void setValue(InteriorLightStatusProperty::InteriorLightStatus val);
	InteriorLightStatusProperty::InteriorLightStatus value();
	
	virtual void fromGVariant(GVariant* value);
	virtual GVariant* toGVariant();
	InteriorLightStatusProperty();
};

#endif // INTERIORLIGHTSTATUSPROPERTY_H
