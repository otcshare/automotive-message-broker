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


#ifndef CORE_H
#define CORE_H

#include "abstractsink.h"
#include "abstractsource.h"
#include "abstractroutingengine.h"

#include <map>

class Core: public AbstractRoutingEngine
{
	
public:
	Core();
	~Core();
	/// sources:

	void setSupported(PropertyList supported, AbstractSource* source);
	void updateSupported(PropertyList added, PropertyList removed);
	void updateProperty(AbstractPropertyType* value, const std::string &uuid);
	
	/// sinks:

	void registerSink(AbstractSink *self);
	void unregisterSink(AbstractSink *self);
	AsyncPropertyReply* getPropertyAsync(AsyncPropertyRequest request);
	AsyncRangePropertyReply* getRangePropertyAsync(AsyncRangePropertyRequest request);
	AsyncPropertyReply * setProperty(AsyncSetPropertyRequest request);
	void subscribeToProperty(VehicleProperty::Property, AbstractSink* self);
	void subscribeToProperty(VehicleProperty::Property, std::string sourceUuidFilter, AbstractSink *self);
	void subscribeToProperty(VehicleProperty::Property, std::string sourceUuidFilter, Zone::Type zoneFilter, AbstractSink *self);
	void unsubscribeToProperty(VehicleProperty::Property, AbstractSink* self);
	PropertyList supported() { return mMasterPropertyList; }

	PropertyInfo getPropertyInfo(VehicleProperty::Property, std::string sourceUuid);
	std::list<std::string> sourcesForProperty(VehicleProperty::Property property);
	
	struct Performance {
		Performance(): propertiesPerSecond(0), firedPropertiesPerSecond(0) {}
		int propertiesPerSecond;
		int firedPropertiesPerSecond;
	};

private:
	PropertyList mMasterPropertyList;
	
	SourceList mSources;
	SinkList mSinks;

	Performance performance;
	
	std::map<VehicleProperty::Property, SinkList> propertySinkMap;

	std::map<AbstractSink*, std::map<VehicleProperty::Property, std::string> > filteredSourceSinkMap;

	std::map<VehicleProperty::Property, std::string> previousValueMap;
    
};

#endif // CORE_H
