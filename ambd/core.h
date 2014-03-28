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

#include <unordered_map>
#include <map>

class Core: public AbstractRoutingEngine
{
	
public:
	Core();
	~Core();
	/// sources:

	void registerSource(AbstractSource *src);
	void updateSupported(PropertyList added, PropertyList removed, AbstractSource* source);
	void updateProperty(AbstractPropertyType* value, const std::string &uuid);
	
	/// sinks:

	void registerSink(AbstractSink *self);
	void unregisterSink(AbstractSink *self);
	AsyncPropertyReply* getPropertyAsync(AsyncPropertyRequest request);
	AsyncRangePropertyReply* getRangePropertyAsync(AsyncRangePropertyRequest request);
	AsyncPropertyReply * setProperty(AsyncSetPropertyRequest request);
	bool subscribeToProperty(VehicleProperty::Property, AbstractSink* self);
	bool subscribeToProperty(VehicleProperty::Property, std::string sourceUuidFilter, AbstractSink *self);
	bool subscribeToProperty(VehicleProperty::Property, std::string sourceUuidFilter, Zone::Type zoneFilter, AbstractSink *self);
	bool unsubscribeToProperty(VehicleProperty::Property, AbstractSink* self);
	PropertyList supported();

	PropertyInfo getPropertyInfo(VehicleProperty::Property, std::string sourceUuid);
	std::list<std::string> sourcesForProperty(VehicleProperty::Property property);
	
	struct Performance {
		Performance(): propertiesPerSecond(0), firedPropertiesPerSecond(0) {}
		int propertiesPerSecond;
		int firedPropertiesPerSecond;
	};

	void inspectSupported();

private:

	void handleAddSupported(const PropertyList& added, AbstractSource* source);
	void handleRemoveSupported(const PropertyList& removed, AbstractSource* source);
	AbstractSource* sourceForProperty(const VehicleProperty::Property& property, const std::string& sourceUuidFilter = "") const;

private:
	
	//typedef std::map< Zone::Type, AbstractPropertyType> ZonePropertyType;

	// to support zone filtering replace VehicleProperty::Property with ZonePropertyType
	std::multimap<AbstractSource*, VehicleProperty::Property> mMasterPropertyList;

	// K = AbstractSource::uuid(), T = AbstractSource*
	std::set<AbstractSource*> mSources;
	std::set<AbstractSink*> mSinks;

	Performance performance;
	
	// std::string here is AbstractSource::uuid()
	typedef std::map<AbstractSink*, std::string> FilteredSourceSinkMap;

	// to support zone filtering replace VehicleProperty::Property with ZonePropertyType
	std::unordered_map<VehicleProperty::Property, FilteredSourceSinkMap > propertySinkMap;
};

#endif // CORE_H
