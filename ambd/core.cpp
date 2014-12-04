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


#include "core.h"
#include <functional>
#include <glib.h>
#include "listplusplus.h"
#include "debugout.h"

using namespace std::placeholders;

int lastpps=0;
int lastfpps=0;

static int PPSUpdate(void* data)
{
	Core::Performance *performance = (Core::Performance*)data;

	if(performance->propertiesPerSecond > 0 && performance->propertiesPerSecond != lastpps)
	{
		lastpps = performance->propertiesPerSecond;
		DebugOut(1)<<"Property updates per second: "<<performance->propertiesPerSecond<<endl;
	}

	performance->propertiesPerSecond = 0;

	if(performance->firedPropertiesPerSecond > 0 && performance->firedPropertiesPerSecond != lastfpps)
	{
		lastfpps = performance->firedPropertiesPerSecond;
		DebugOut(1)<<"Fired property updates per second: "<<performance->firedPropertiesPerSecond<<endl;
	}

	performance->firedPropertiesPerSecond = 0;

	return 1;
}

Core::Core(std::map<string, string> config): AbstractRoutingEngine(config), handleCount(0)
{
	g_timeout_add(1000,PPSUpdate,&performance);

	auto simpleCb = [this](amb::Queue<AbstractPropertyType*, amb::PropertyCompare>* q)
	{
		while(q->count())
		{
			AbstractPropertyType* value = q->pop();
			updateProperty(value);
		}
	};

	int hpqs = 0;
	int lpqs = 0;
	int npqs = 0;

	if(config.find("highPriorityQueueSize") != config.end())
	{
		hpqs = boost::lexical_cast<int, std::string>(config["highPriorityQueueSize"]);
	}

	if(config.find("normalPriorityQueueSize") != config.end())
	{
		npqs = boost::lexical_cast<int, std::string>(config["normalPriorityQueueSize"]);
	}

	if(config.find("lowPriorityQueueSize") != config.end())
	{
		lpqs = boost::lexical_cast<int, std::string>(config["lowPriorityQueueSize"]);
	}

	watcherPtr = new amb::AsyncQueueWatcher<AbstractPropertyType*, amb::PropertyCompare>(&updatePropertyQueue, simpleCb, npqs);
	watcherPtrLow = new amb::AsyncQueueWatcher<AbstractPropertyType*, amb::PropertyCompare>(&updatePropertyQueueLow,
																	  simpleCb, lpqs,
																	  AbstractPropertyType::Low);
	watcherPtrHigh = new amb::AsyncQueueWatcher<AbstractPropertyType*, amb::PropertyCompare>(&updatePropertyQueueHigh,
																	   simpleCb, hpqs,
																	   AbstractPropertyType::High);

}

Core::~Core()
{
	delete watcherPtr;

	for(auto itr = mSinks.begin(); itr != mSinks.end(); ++itr)
	{
		delete *itr;
		itr = mSinks.begin();
	}
	mSinks.clear();
}

void Core::registerSource(AbstractSource *source)
{
	mSources.insert(source);
}

void Core::updateSupported(PropertyList added, PropertyList removed, AbstractSource* source)
{
	if(!source || mSources.find(source) == mSources.end())
		return;

	/// add the newly supported to master list

	if(added.size())
		handleAddSupported(added, source);

	/// removed no longer supported properties from master list.
	if(removed.size())
		handleRemoveSupported(removed, source);

	/// tell all sinks about the newly supported properties.

	PropertyList s = supported();

	if(!s.size()) return;

	for(auto itr = mSinks.begin(); itr != mSinks.end(); ++itr)
	{
		AbstractSink* sink = *itr;
		sink->supportedChanged(s);
	}
}

PropertyList Core::supported()
{
	PropertyList supportedProperties;

	std::transform(mMasterPropertyList.begin(), mMasterPropertyList.end(), std::back_inserter(supportedProperties),
			  [](const std::multimap<AbstractSource*, VehicleProperty::Property>::value_type& itr)
	{
		return itr.second;
	});

	// remove duplicates:
	std::sort(supportedProperties.begin(), supportedProperties.end());
	auto itr = std::unique(supportedProperties.begin(), supportedProperties.end());

	supportedProperties.erase(itr,supportedProperties.end());

	return supportedProperties;
}

void Core::updateProperty(AbstractPropertyType *value, const string & uuid)
{
	if(value->sourceUuid != uuid)
	{
		value->sourceUuid = uuid;
	}

	if(value->priority == AbstractPropertyType::Instant)
		updateProperty(value);
	else if(value->priority == AbstractPropertyType::High)
	{
		value->destroyed.push_back([this](AbstractPropertyType* v)
		{
			updatePropertyQueueHigh.remove(v);
		});
		updatePropertyQueueHigh.append(value);
	}
	else if(value->priority == AbstractPropertyType::Normal)
	{
		value->destroyed.push_back([this](AbstractPropertyType* v)
		{
			updatePropertyQueue.remove(v);
		});
		updatePropertyQueue.append(value);
	}
	else if(value->priority == AbstractPropertyType::Low)
	{
		value->destroyed.push_back([this](AbstractPropertyType* v)
		{
			updatePropertyQueueLow.remove(v);
		});
		updatePropertyQueueLow.append(value);
	}
}

void Core::updateProperty(AbstractPropertyType * value)
{
	VehicleProperty::Property & property = value->name;
	const string & uuid = value->sourceUuid;

	performance.propertiesPerSecond++;

	//auto sinks = propertySinkMap[property]; !!! this will insert empty std::set<AbstractSink*> into propertySinkMap !!!
	auto filteredSourceSinkMapIt =  propertySinkMap.find(property);
	auto cbMapItr = propertyCbMap.find(property);

	if(filteredSourceSinkMapIt != propertySinkMap.end())
	{
		const FilteredSourceSinkMap & filteredSourceSinks = filteredSourceSinkMapIt->second;

		DebugOut()<<__FUNCTION__<<"() there are "<<filteredSourceSinks.size()<<" sinks connected to property: "<<property<<endl;

		performance.firedPropertiesPerSecond++;

		for(auto itr = filteredSourceSinks.begin(); itr != filteredSourceSinks.end(); ++itr)
		{
			AbstractSink* sink = itr->first;
			const std::string & sourceUuid = itr->second;

			bool isFiltered = !sourceUuid.empty();

			if(isFiltered)
			{
				DebugOut()<<"Property ("<<property<<") for sink is filtered for source: "<<sourceUuid<<endl;
			}

			if( !isFiltered || sourceUuid == uuid)
			{
				sink->propertyChanged(value);
			}
		}
	}
	else
	{
		DebugOut()<<__FUNCTION__<<"() there are no sinks connected to property: "<<property<<endl;
	}

	if(cbMapItr != propertyCbMap.end())
	{
		FilteredSourceCbMap cbs = (*cbMapItr).second;

		for(auto itr : cbs)
		{
			uint handle = itr.first;
			const std::string& sourceUuid = itr.second;

			bool isFiltered = !sourceUuid.empty();

			if(isFiltered)
			{
				DebugOut()<<"Property ("<<property<<") for cb is filtered for source: "<<sourceUuid<<endl;
			}

			if( !isFiltered || sourceUuid == uuid)
			{
				if(handleCbMap.count(handle))
				{
					auto cb = handleCbMap[handle];
					try
					{
						cb(value);
					}
					catch(...)
					{
						DebugOut(DebugOut::Warning)<<"Failed to call callback subscribed to property: "<<property<<endl;
					}
				}
			}
		}
	}
	else
	{
		DebugOut()<<__FUNCTION__<<"() there are no cb connected to property: "<<property<<endl;
		return;
	}
}

void Core::registerSink(AbstractSink *self)
{
	mSinks.insert(self);
}

void Core::unregisterSink(AbstractSink *self)
{
	mSinks.erase(self);
}

AsyncPropertyReply *Core::getPropertyAsync(AsyncPropertyRequest request)
{
	AbstractSource* source = sourceForProperty(request.property, request.sourceUuidFilter);

	AsyncPropertyReply* reply = new AsyncPropertyReply(request);

	if(!source || ((source->supportedOperations() & AbstractSource::Get) != AbstractSource::Get)) { // not found or doesn't support AbstractSource::Get
		// Don't wait until timer expire, complete with error here.
		reply->error = AsyncPropertyReply::InvalidOperation;
		if(request.completed)
			request.completed(reply);
	}
	else{
		source->getPropertyAsync(reply);

	}

	/** right now the owner of the reply becomes the requestor that called this method.
   *  reply will become invalid after the first reply. */
	return reply;
}

AsyncRangePropertyReply * Core::getRangePropertyAsync(AsyncRangePropertyRequest request)
{
	AsyncRangePropertyReply * reply = new AsyncRangePropertyReply(request);

	for(auto itr = mSources.begin(); itr != mSources.end(); ++itr)
	{
		AbstractSource* src = *itr;
		if(((src->supportedOperations() & AbstractSource::GetRanged) == AbstractSource::GetRanged))
		{
			src->getRangePropertyAsync(reply);
		}
	}

	return reply;
}

AsyncPropertyReply * Core::setProperty(AsyncSetPropertyRequest request)
{
	AbstractSource* src = sourceForProperty(request.property, request.sourceUuidFilter);

	if(src && ((src->supportedOperations() & AbstractSource::Set) == AbstractSource::Set))
		return src->setProperty(request);

	DebugOut(DebugOut::Warning)<<"Error: setProperty opration failed.  Property may not be supported: "<<request.property<<endl;
	return NULL;
}

bool Core::subscribeToProperty(const VehicleProperty::Property &property, AbstractSink* sink)
{
	auto sinksIt = propertySinkMap.find(property);
	if(sinksIt != propertySinkMap.end() && sinksIt->second.find(sink) != sinksIt->second.end())
	{
		DebugOut(1)<<__FUNCTION__<<" property " << property << " has already been subscribed." << endl;
		return false;
	}

	DebugOut(1)<<"Subscribing to: "<<property<<endl;

	bool subscribed(false);
	auto itr = mMasterPropertyList.begin();
	while(itr != mMasterPropertyList.end())
	{
		VehicleProperty::Property prop = itr->second;
		if(prop == property) {
			AbstractSource* src = itr->first;
			src->subscribeToPropertyChanges(property);
			// Move to next source. It will skip all the remaining properties in this source.
			itr = mMasterPropertyList.upper_bound(src);
			subscribed = true;
		}
		else{
			++itr;
		}
	}

	//if(subscribed)
	propertySinkMap[property].emplace(sink, std::string(""));

	return subscribed;
}

bool Core::subscribeToProperty(const VehicleProperty::Property &property, const string &sourceUuidFilter, AbstractSink *sink)
{
	auto sinksIt = propertySinkMap.find(property);
	if(sinksIt != propertySinkMap.end() && sinksIt->second.find(sink) != sinksIt->second.end())
	{
		DebugOut(1)<<__FUNCTION__<<" property " << property << " has already been subscribed." << endl;
		return false;
	}

	DebugOut(1)<<"Subscribing to: "<<property<<endl;

	AbstractSource* src = sourceForProperty(property, sourceUuidFilter);
	if(!src)
		return false;

	propertySinkMap[property].emplace(sink, sourceUuidFilter);

	src->subscribeToPropertyChanges(property);

	return true;
}

bool Core::subscribeToProperty(const VehicleProperty::Property &, const string & sourceUuidFilter, Zone::Type zoneFilter, AbstractSink *sink)
{
	/// TODO: implement
	throw std::runtime_error("Not implemented");
}

uint Core::subscribeToProperty(const VehicleProperty::Property &property, AbstractRoutingEngine::PropertyChangedType cb, std::string pid)
{
	DebugOut(1)<<"Subscribing to: "<<property<<endl;

	auto itr = mMasterPropertyList.begin();
	while(itr != mMasterPropertyList.end())
	{
		VehicleProperty::Property prop = itr->second;
		if(prop == property) {
			AbstractSource* src = itr->first;
			src->subscribeToPropertyChanges(property);
			// Move to next source. It will skip all the remaining properties in this source.
			itr = mMasterPropertyList.upper_bound(src);
		}
		else{
			++itr;
		}
	}

	handleCbMap[++handleCount] = cb;
	propertyCbMap[property].emplace(handleCount, std::string(""));
	return handleCount;
}

bool Core::unsubscribeToProperty(const VehicleProperty::Property & property, AbstractSink* sink)
{
	auto sinksIt = propertySinkMap.find(property);
	if(sinksIt == propertySinkMap.end())
	{
		DebugOut(1)<<__FUNCTION__<<" property not subscribed to: "<<property<<endl;
		return false;
	}

	sinksIt->second.erase(sink);

	/// Now we check to see if this is the last subscriber
	if(sinksIt->second.empty())
	{
		propertySinkMap.erase(sinksIt);
		auto itr = mMasterPropertyList.begin();
		while(itr != mMasterPropertyList.end())
		{
			if(itr->second == property) {
				AbstractSource* src = itr->first;
				src->unsubscribeToPropertyChanges(property);
				// Move to next source. It will skip all the remaining properties in this source.
				itr = mMasterPropertyList.upper_bound(src);
			}
			else{
				++itr;
			}
		}
	}

	return true;
}

void Core::unsubscribeToProperty(uint handle)
{
	handleCbMap.erase(handle);
	/// TODO: unsubscribe from source
}

PropertyInfo Core::getPropertyInfo(const VehicleProperty::Property &property, const string &sourceUuid)
{
	if(sourceUuid == "")
		return PropertyInfo::invalid();

	auto srcs = sourcesForProperty(property);

	if(!contains(srcs, sourceUuid))
		return PropertyInfo::invalid();

	auto theSource = find_if(mSources.begin(), mSources.end(),[&sourceUuid](const std::set<AbstractSource*>::value_type & itr)
	{
		return (itr)->uuid() == sourceUuid;
	});

	return (*theSource)->getPropertyInfo(property);
}

std::vector<string> Core::sourcesForProperty(const VehicleProperty::Property & property)
{
	std::vector<std::string> list;

	for(auto src : mSources)
	{
		if(contains(src->supported(), property))
			list.push_back(src->uuid());
	}

	return list;
}

void Core::inspectSupported()
{
	for(AbstractSource* src : mSources)
	{
		updateSupported(src->supported(), PropertyList(), src);
	}
}

void Core::handleAddSupported(const PropertyList& added, AbstractSource* source)
{
	if(!source)
		throw std::runtime_error("Core::handleAddSupported passed a null source");

	for(auto property : added)
	{
		if(!sourceForProperty(property, source->uuid()))
			mMasterPropertyList.emplace(source, property);

		// Subscribe to property in a new source if such property was subscribed. This catches newly supported properties in the process.
		if( propertySinkMap.find(property) != propertySinkMap.end()){
			source->subscribeToPropertyChanges(property);
		}
	}
}

void Core::handleRemoveSupported(const PropertyList& removed, AbstractSource* source)
{
	if(!source)
		return;

	auto range = mMasterPropertyList.equal_range(source);
	for(auto itr = removed.begin(); itr != removed.end(); ++itr)
	{
		//
		// TODO: We do not have info about all subscribed sources in
		// std::unordered_map<VehicleProperty::Property, std::map<AbstractSink*, std::string> > propertySinkMap
		// so we do not know if we can/should remove property from propertySinkMap,
		// but I suppose this should be handled by each AbstractSink implementation in a callback AbstractSink::supportedChanged().

		const VehicleProperty::Property property(*itr);

		auto it = find_if(
					range.first,	// the first property in source
					range.second,   // one item right after the last property in source
					[&property](const std::multimap<AbstractSource*, VehicleProperty::Property>::value_type& it) { return it.second == property; }
		);

		if (it != range.second)// property was found
		{
			mMasterPropertyList.erase(it);// References and iterators to the erased elements are invalidated. Other iterators and references are not invalidated.

			// TODO: Do we need to unsubscribe here ???
		}
	}
}

AbstractSource* Core::sourceForProperty(const VehicleProperty::Property& property, const std::string& sourceUuidFilter) const
{
	auto it = mMasterPropertyList.end();
	if(sourceUuidFilter.empty()){
		it = std::find_if(mMasterPropertyList.begin(), mMasterPropertyList.end(),
						  [&property](const std::multimap<AbstractSource*, VehicleProperty::Property>::value_type& it) { return it.second == property; }
		);
	}
	else{
		auto itSource = find_if(mSources.begin(),mSources.end(),[&sourceUuidFilter](const std::set<AbstractSource*>::value_type & it)
		{
			return (it)->uuid() == sourceUuidFilter;
		});
		if(itSource != mSources.end()){
			auto range = mMasterPropertyList.equal_range(*itSource);
			auto temp = find_if(
						range.first,	// the first property in source
						range.second,   // one item right after the last property in source
						[&property](const std::multimap<AbstractSource*, VehicleProperty::Property>::value_type& it)
			{
				return it.second == property; }
			);

			if (temp != range.second)// property was found
				it = temp;
		}
	}

	if(it == mMasterPropertyList.end())
		return nullptr;
	else
		return it->first;
}

