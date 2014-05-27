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

#ifndef DBUSSINK_H_
#define DBUSSINK_H_

#include "abstractsink.h"
#include "abstractproperty.h"
#include "abstractdbusinterface.h"
#include "varianttype.h"

#include <map>
#include <type_traits>

typedef std::unordered_map<VehicleProperty::Property, VariantType*> PropertyDBusMap;

class DBusSink : public AbstractSink, public AbstractDBusInterface
{

public:
	DBusSink(std::string objectName, AbstractRoutingEngine* engine, GDBusConnection* connection, map<string, string> config);
	virtual ~DBusSink() {
		for(auto itr = propertyDBusMap.begin(); itr != propertyDBusMap.end(); ++itr)
		{
			delete itr->second;
		}
	}
	virtual void supportedChanged(const PropertyList & supportedProperties);
	virtual void propertyChanged(AbstractPropertyType *value);
	virtual const std::string uuid();

	std::list<VehicleProperty::Property> wantsProperties()
	{
		std::list<VehicleProperty::Property> l;
		for(auto itr = propertyDBusMap.begin(); itr != propertyDBusMap.end(); itr++)
		{
			l.push_back((*itr).first);
		}

		return l;
	}

	void setSourceFilter(std::string sourceFilter)
	{
		mSourceFilter = sourceFilter;
	}

	void setZoneFilter(Zone::Type zone)
	{
		zoneFilter = zone;
	}

protected:
	template <typename T>
	void wantProperty(VehicleProperty::Property property, std::string propertyName, std::string signature, AbstractProperty::Access access)
	{
		propertyDBusMap[property] = new VariantType(routingEngine, signature, property, propertyName, access);
	}


	void wantPropertyString(VehicleProperty::Property property, std::string propertyName, std::string signature, AbstractProperty::Access access)
	{
		propertyDBusMap[property] = new VariantType(routingEngine, signature, property, propertyName, access);
	}

	void wantPropertyVariant(VehicleProperty::Property ambProperty, std::string propertyName, std::string signature, AbstractProperty::Access access)
	{
		propertyDBusMap[ambProperty] = new VariantType(routingEngine, signature, ambProperty, propertyName, access);
	}

	PropertyDBusMap propertyDBusMap;



};

class DBusInterfaceManager;
class DBusSinkManager: public AbstractSinkManager
{
	DBusInterfaceManager* manager;
public:
	DBusSinkManager(AbstractRoutingEngine* engine, map<string, string> config);
	~DBusSinkManager();
};

#endif
