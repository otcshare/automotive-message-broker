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


#ifndef TRANSMISSIONGEARSTATUS_H
#define TRANSMISSIONGEARSTATUS_H

#include <abstractproperty.h>


class TransmissionGearStatusProperty : public AbstractProperty
{

public:
	enum TransmissionGearStatus 
	{
		Neutral = 0,
		First = 1,
		Second = 2,
		Third = 3,
		Fourth = 4,
		Fifth = 5,
		Sixth,
		Seventh,
		Eighth, 
		Nineth,
		Tenth,
		Drive = 64,
		Reverse = 128,
		Park = 255
	};
	
	void setValue(TransmissionGearStatus status);
	TransmissionGearStatus value();

	virtual void fromGVariant(GVariant* value);
	virtual GVariant* toGVariant();
	TransmissionGearStatusProperty();
};

#endif // TRANSMISSIONGEARSTATUS_H
