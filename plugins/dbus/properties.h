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
#include "abstractroutingengine.h"

class VehicleSpeedProperty: public DBusSink
{
public:
	VehicleSpeedProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.vehicleSpeed","/org/automotive/runningstatus/vehicleSpeed", re, connection, map<string, string>())
	{
		wantProperty<uint16_t>(VehicleProperty::VehicleSpeed,"VehicleSpeed", "i", AbstractProperty::Read);
		supportedChanged(re->supported());
	}


};

class TirePressureProperty: public DBusSink
{
public:
	TirePressureProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.tirePressure","/org/automotive/maintainance/tirePressure", re, connection, map<string, string>())
	{
		wantProperty<uint16_t>(VehicleProperty::TirePressureLeftFront,"LeftFront", "i", AbstractProperty::Read);
		wantProperty<uint16_t>(VehicleProperty::TirePressureRightFront,"RightFront", "i", AbstractProperty::Read);
		wantProperty<uint16_t>(VehicleProperty::TirePressureLeftRear,"LeftRear", "i", AbstractProperty::Read);
		wantProperty<uint16_t>(VehicleProperty::TirePressureRightRear,"RightRear", "i", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

#endif
