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
#include "asyncqueue.hpp"

#include <unordered_map>
#include <unordered_set>
#include <map>

class Core: public AbstractRoutingEngine
{

public:
	Core(std::map<std::string, std::string> config);
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
	uint subscribeToProperty(const VehicleProperty::Property &, AbstractRoutingEngine::PropertyChangedType, std::string pid="");
	bool subscribeToProperty(const VehicleProperty::Property &, AbstractSink* self);
	bool subscribeToProperty(const VehicleProperty::Property &, const std::string & sourceUuidFilter, AbstractSink *self);
	bool subscribeToProperty(const VehicleProperty::Property &, const std::string & sourceUuidFilter, Zone::Type zoneFilter, AbstractSink *self);
	bool unsubscribeToProperty(const VehicleProperty::Property &, AbstractSink* self);
	void unsubscribeToProperty(uint handle);
	PropertyList supported();

	PropertyInfo getPropertyInfo(const VehicleProperty::Property &,const std::string &sourceUuid);
	std::vector<std::string> sourcesForProperty(const VehicleProperty::Property & property);

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
	void updateProperty(AbstractPropertyType* value);

private:

	//typedef std::map< Zone::Type, AbstractPropertyType> ZonePropertyType;

	// to support zone filtering replace VehicleProperty::Property with ZonePropertyType
	std::multimap<AbstractSource*, VehicleProperty::Property> mMasterPropertyList;

	std::unordered_set<AbstractSource*> mSources;
	std::unordered_set<AbstractSink*> mSinks;

	Performance performance;

	// std::string here is AbstractSource::uuid()
	typedef std::map<AbstractSink*, std::string> FilteredSourceSinkMap;

	///uint cbHandle, std::string uuid
	typedef std::unordered_map<uint, std::string> FilteredSourceCbMap;

	// to support zone filtering replace VehicleProperty::Property with ZonePropertyType
	std::unordered_map<VehicleProperty::Property, FilteredSourceSinkMap > propertySinkMap;
	std::unordered_map<VehicleProperty::Property, FilteredSourceCbMap> propertyCbMap;
	std::unordered_map<uint, AbstractRoutingEngine::PropertyChangedType> handleCbMap;

	amb::Queue<AbstractPropertyType*, amb::PropertyCompare> updatePropertyQueue;
	amb::Queue<AbstractPropertyType*, amb::PropertyCompare> updatePropertyQueueHigh;
	amb::Queue<AbstractPropertyType*, amb::PropertyCompare> updatePropertyQueueLow;
	amb::AsyncQueueWatcher<AbstractPropertyType*, amb::PropertyCompare>* watcherPtr;
	amb::AsyncQueueWatcher<AbstractPropertyType*, amb::PropertyCompare>* watcherPtrHigh;
	amb::AsyncQueueWatcher<AbstractPropertyType*, amb::PropertyCompare>* watcherPtrLow;

	uint handleCount;
};

#endif // CORE_H
