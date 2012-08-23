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

#ifndef _PROPERTIES_H_H_H_
#define _PROPERTIES_H_H_H_

#include "dbusplugin.h"
#include "abstractdbusinterface.h"

class VehicleSpeedProperty: public AbstractDBusInterface, public DBusSink
{
public:
	AccelerationPropertyInterface(AbstractRoutingEngine* re, GDBusConnection* connection)
		:AbstractDBusInterface("org.automotive.acceleration","/org/automotive/acceleration", connection),
		  DBusSink(re)
	{
		supportedChanged(re->supported());
	}

	void supportedChanged(PropertyList supportedProperties)
	{
		for(PropertyDBusMap itr = propertyDBusMap.begin(); itr != propertyDBusMap.end(); itr++)
		{
			if(ListPlusPlus<VehicleProperty::Property>(&supportedProperties).contains((*itr)))
			{
				routingEngine->subscribeToProperty((*itr), this);

			}
		}
	}
};

#endif
