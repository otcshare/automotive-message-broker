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


#ifndef TRIPMETERPROPERTY_H
#define TRIPMETERPROPERTY_H

#include <abstractproperty.h>
#include <list>


typedef std::list<uint32_t> TripMeters;

class TripMeterProperty : public AbstractProperty
{

public:
	void setValue(TripMeters trips);
	TripMeters value(); 
	virtual void fromGVariant(GVariant* value);
	virtual GVariant* toGVariant();
	TripMeterProperty();
	
private:
	TripMeters mValue;
};

#endif // TRIPMETERPROPERTY_H
