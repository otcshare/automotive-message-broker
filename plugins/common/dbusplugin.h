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
#include "abstractdbusinterface.h"
#include "varianttype.h"

#include <map>

class DBusSink : public AbstractSink, public AbstractDBusInterface
{

public:
	DBusSink(std::string objectName, AbstractRoutingEngine* engine, GDBusConnection* connection, map<string, string> config);
	virtual ~DBusSink() {
		for(auto i : propertyDBusMap)
		{
			delete i;
		}
	}
	virtual void supportedChanged(const PropertyList & supportedProperties);
	virtual void propertyChanged(AbstractPropertyType *value);
	virtual const std::string uuid();

	PropertyList wantsProperties()
	{
		PropertyList l;
		for(auto i : propertyDBusMap)
		{
			l.push_back(i->ambPropertyName());
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

	void wantPropertyVariant(VehicleProperty::Property ambProperty, std::string propertyName, VariantType::Access access)
	{
		propertyDBusMap.push_back(new VariantType(routingEngine, ambProperty, propertyName, access));
	}

};

#endif
