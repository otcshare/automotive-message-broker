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
#include "basicproperty.h"
#include <map>

typedef std::map<VehicleProperty::Property, AbstractProperty*> PropertyDBusMap;

class DBusSink : public AbstractSink, public AbstractDBusInterface
{

public:
	DBusSink(std::string interface, std::string path, AbstractRoutingEngine* engine, GDBusConnection* connection, map<string, string> config);
	virtual void supportedChanged(PropertyList supportedProperties);
	virtual void propertyChanged(VehicleProperty::Property property, AbstractPropertyType *value, std::string uuid);
	virtual std::string uuid();

protected:
	template <typename T>
	void wantProperty(VehicleProperty::Property property, std::string propertyName, std::string signature, AbstractProperty::Access access)
	{
		propertyDBusMap[property] = new BasicProperty<T>(routingEngine, property, propertyName, signature, access, this);
	}

	/*virtual void setProperty(VehicleProperty::Property name, AbstractPropertyType* value)
	{
		AsyncSetPropertyRequest request;
		request.property = name;
		request.value = value;
		request.completed = [](AsyncPropertyReply* reply) { delete reply; };
		routingEngine->setProperty(request);
	}*/

	PropertyDBusMap propertyDBusMap;
private:

	bool supported;
};

class DBusSinkManager: public AbstractSinkManager
{
public:
	DBusSinkManager(AbstractRoutingEngine* engine, map<string, string> config);
};

#endif
