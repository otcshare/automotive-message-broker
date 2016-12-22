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

#include <boost/lexical_cast.hpp>
#include <map>
#include <string>

#include "abstractroutingengine.h"
#include "debugout.h"
#include "listplusplus.h"

std::map<std::string, std::string> DBusSink::dbusConfig;

DBusSink::DBusSink(std::string propertyName, AbstractRoutingEngine* engine, GDBusConnection* connection, std::map<std::string, std::string> config)
	: AbstractSink(engine, dbusConfig),
	AbstractDBusInterface("org.automotive."+propertyName, propertyName, connection)
{
	AbstractDBusInterface::re = engine;

	int timeout = 60;

	if(configuration.find("frequency") != configuration.end())
	{
		int t = boost::lexical_cast<int>(configuration["frequency"]);

		timeout = 1000 / t;
	}

	setTimeout(timeout);
}

void DBusSink::supportedChanged(const PropertyList &supportedProperties)
{
	startRegistration();

	for(auto itr : propertyDBusMap)
	{
		if(contains(supportedProperties, itr->ambPropertyName()))
		{
			PropertyInfo info = re->getPropertyInfo(itr->ambPropertyName(), mSourceFilter);

			if (!info.isValid() || !contains(info.zones(), zoneFilter))
			{
				continue;
			}

			VariantType* prop = itr;
			prop->setSourceFilter(mSourceFilter);
			prop->setZoneFilter(zoneFilter);
			prop->initialize();
			VehicleProperty::Property p = itr->ambPropertyName();
			routingEngine->subscribeToProperty(p, mSourceFilter, this);
			addProperty(prop);
			supported = true;
			mTime = amb::currentTime();
		}
	}
}

void DBusSink::propertyChanged(AbstractPropertyType *value)
{
	VehicleProperty::Property property = value->name;

	if( value->zone != zoneFilter)
		return;

	for(auto i : propertyDBusMap)
	{
		if(i->ambPropertyName() == property)
		{
			VariantType* prop = i;
			mTime = value->timestamp;
			prop->updateValue(value);
			updateValue(prop);
		}
	}
}

const string DBusSink::uuid()
{
	return "c2e6cafa-eef5-4b8a-99a0-0f2c9be1057d";
}
