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


#ifndef ENGINEOILPROPERTY_H
#define ENGINEOILPROPERTY_H

#include <abstractproperty.h>

class EngineOil 
{
public:
	EngineOil():remaining(0), temperature(0), pressure(0) { }
	
	uint8_t remaining;
	uint8_t temperature;
	uint16_t pressure;
};

class EngineOilProperty : public AbstractProperty
{

public:


	virtual void fromGVariant(GVariant* value);
	virtual GVariant* toGVariant();
    
    
};

#endif // ENGINEOILPROPERTY_H
