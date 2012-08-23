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
#include "accelerationproperty.h"
#include "abstractroutingengine.h"
#include "listplusplus.h"

AccelerationPropertyInterface::AccelerationPropertyInterface(AbstractRoutingEngine *re, GDBusConnection* connection)
	:DBusSink("org.automotive.acceleration","/org/automotive/acceleration", re, connection),
	  accelerationX(nullptr), accelerationY(nullptr), accelerationZ(nullptr)
{
	supportedChanged(re->supported());
}

void AccelerationPropertyInterface::supportedChanged(PropertyList supportedProperties)
{
	bool supported = false;
	if(ListPlusPlus<VehicleProperty::Property>(&supportedProperties).contains(VehicleProperty::AccelerationX))
	{
		if(!accelerationX)
		{
			routingEngine->subscribeToProperty(VehicleProperty::AccelerationX, this);
			accelerationX = new BasicProperty<int>("X", "i", AbstractProperty::Read, this);
			propertyDBusMap[VehicleProperty::AccelerationX] = accelerationX;
			supported = true;
		}
	}

	if(ListPlusPlus<VehicleProperty::Property>(&supportedProperties).contains(VehicleProperty::AccelerationY))
	{
		if(!accelerationY)
		{
			routingEngine->subscribeToProperty(VehicleProperty::AccelerationY, this);
			accelerationY = new BasicProperty<int>("Y", "i", AbstractProperty::Read, this);
			propertyDBusMap[VehicleProperty::AccelerationY] = accelerationY;
			supported = true;
		}
	}

	if(ListPlusPlus<VehicleProperty::Property>(&supportedProperties).contains(VehicleProperty::AccelerationZ))
	{
		if(!accelerationZ)
		{
			routingEngine->subscribeToProperty(VehicleProperty::AccelerationZ, this);
			accelerationZ = new BasicProperty<int>("Z", "i", AbstractProperty::Read, this);
			propertyDBusMap[VehicleProperty::AccelerationZ] = accelerationZ;
			supported = true;
		}
	}

	if(supported)
	{
		registerObject();
	}
}

/*void AccelerationPropertyInterface::propertyChanged(VehicleProperty::Property property, boost::any value, string)
{

	if(property == VehicleProperty::AccelerationX)
	{
		if(accelerationX)
		{
			accelerationX->setValue(boost::any_cast<int>(value));
		}
	}

	else if(property == VehicleProperty::AccelerationY)
	{
		if(accelerationY)
		{
			accelerationY->setValue(boost::any_cast<int>(value));
		}
	}

	else if(property == VehicleProperty::AccelerationZ)
	{
		if(accelerationZ)
		{
			accelerationZ->setValue(boost::any_cast<int>(value));
		}
	}
}*/




