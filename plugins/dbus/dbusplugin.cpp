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

#include <boost/lexical_cast.hpp>

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
	AbstractDBusInterface::re = engine;

	int timeout = 60;

	if(config.find("frequency") != config.end())
	{
		int t = boost::lexical_cast<int>(config["frequency"]);

		timeout = 1000 / t;
	}

	setTimeout(timeout);
}

void DBusSink::supportedChanged(const PropertyList &supportedProperties)
{
	startRegistration();

	for(PropertyDBusMap::iterator itr = propertyDBusMap.begin(); itr != propertyDBusMap.end(); itr++)
	{
		if(contains(supportedProperties, (*itr).first))
		{
			VariantType* prop = (*itr).second;
			prop->setSourceFilter(mSourceFilter);
			prop->setZoneFilter(zoneFilter);
			prop->initialize();
			VehicleProperty::Property p = (*itr).first;
			routingEngine->subscribeToProperty(p, mSourceFilter, this);
			addProperty(prop);
			supported = true;
		}
	}
}

void DBusSink::propertyChanged(AbstractPropertyType *value)
{
	VehicleProperty::Property property = value->name;

	if(propertyDBusMap.find(property) == propertyDBusMap.end() || value->zone != zoneFilter)
		return;

	AbstractProperty* prop = propertyDBusMap[property];
	mTime = value->timestamp;
	prop->updateValue(value->copy());
	updateValue(prop);
}

const string DBusSink::uuid()
{
	return "c2e6cafa-eef5-4b8a-99a0-0f2c9be1057d";
}

DBusSinkManager::DBusSinkManager(AbstractRoutingEngine *engine, map<string, string> config) :
	AbstractSinkManager(engine, config),
	manager(nullptr)
{
	manager = new DBusInterfaceManager(engine, config);
}

DBusSinkManager::~DBusSinkManager()
{
	if(manager){
		// delete manager; <-- currently AbstractSink* instances are deleted in Core::~Core()
		manager = nullptr;
	}
}
