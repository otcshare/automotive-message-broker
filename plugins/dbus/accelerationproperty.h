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

#ifndef __ACCELERATIONPROPERTY_H_
#define __ACCELERATIONPROPERTY_H_

#include "abstractdbusinterface.h"
#include "dbusplugin.h"
#include "basicproperty.h"

class AccelerationPropertyInterface: public AbstractDBusInterface, public DBusSink
{
public:
	AccelerationPropertyInterface(AbstractRoutingEngine* re, GDBusConnection* connection);

	virtual void supportedChanged(PropertyList supportedProperties);
	virtual void propertyChanged(VehicleProperty::Property property, boost::any value, std::string uuid);

private:

	BasicProperty<int> * accelerationX;
	BasicProperty<int> * accelerationY;
	BasicProperty<int> * accelerationZ;
	bool supported;
};

#endif
