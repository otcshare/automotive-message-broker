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


#include "dbusplugin.h"
#include "abstractroutingengine.h"
#include "dbusinterfacemanager.h"
#include "debugout.h"
#include "listplusplus.h"

extern "C" AbstractSinkManager * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new DBusSinkManager(routingengine, config);
}

DBusSink::DBusSink(string propertyName, AbstractRoutingEngine* engine, GDBusConnection* connection, map<string, string> config = map<string, string>())
	:AbstractDBusInterface("org.automotive."+propertyName, propertyName, connection),
	  AbstractSink(engine, config)
{

}

void DBusSink::supportedChanged(PropertyList supportedProperties)
{
	startRegistration();

	for(PropertyDBusMap::iterator itr = propertyDBusMap.begin(); itr != propertyDBusMap.end(); itr++)
	{
		if(ListPlusPlus<VehicleProperty::Property>(&supportedProperties).contains((*itr).first))
		{
			VariantType* prop = (*itr).second;
			prop->setSourceFilter(mSourceFilter);
			prop->setZoneFilter(zoneFilter);
			prop->initialize();
			routingEngine->subscribeToProperty((*itr).first, mSourceFilter, this);
			addProperty(prop);
			supported = true;
		}
	}


	if(supported)
		registerObject();
	else
		unregisterObject();
}

void DBusSink::propertyChanged(VehicleProperty::Property property, AbstractPropertyType *value, string uuid)
{
	if(propertyDBusMap.find(property) == propertyDBusMap.end() || value->zone != zoneFilter)
		return;

	AbstractProperty* prop = propertyDBusMap[property];
	prop->setValue(value);
	mTime = value->timestamp;
}

std::string DBusSink::uuid()
{
	return "c2e6cafa-eef5-4b8a-99a0-0f2c9be1057d";
}

DBusSinkManager::DBusSinkManager(AbstractRoutingEngine *engine, map<string, string> config)
	:AbstractSinkManager(engine, config)
{
	DBusInterfaceManager* manager = new DBusInterfaceManager(engine, config);
}
