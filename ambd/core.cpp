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

Core::Core()
{
	g_timeout_add(1000,PPSUpdate,&performance);
}

Core::~Core()
{
	for(auto itr = mSinks.begin(); itr != mSinks.end(); ++itr)
	{
		delete *itr;
	}
	mSinks.clear();
}

void Core::handleAddSupported(const PropertyList& added, AbstractSource* source)
{
	if(!source)
		return;

	for(auto itr = added.begin(); itr != added.end(); ++itr)
	{
		VehicleProperty::Property property = *itr;
		mMasterPropertyList.emplace(source, property);// TODO: no check for duplicated properties from the same source - is it needed ?

		// Subscribe to property in a new source if such property was subscribed. This catches newly supported properties in the process.
		//
		// TODO: is this sufficient to:
		/// Iterate through subscribed properties and resubscribe.  This catches newly supported properties in the process.
		// which was originally located at the end of the setSupported and updateSupported functions? I think it is sufficient.
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
		// TODO: We do not have info about sources in
		// std::unordered_map<VehicleProperty::Property, std::set<AbstractSink*> > propertySinkMap
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
			// I added it here:
			source->unsubscribeToPropertyChanges(property);
		}
	}
}

void Core::setSupported(PropertyList supported, AbstractSource* source)
{
	if(!source)
		return;

	mSources[source->uuid()] = source;
		
	handleAddSupported(supported, source);

	/// tell all new sinks about the newly supported properties.

	for(auto itr = mSinks.begin(); itr != mSinks.end(); ++itr)
	{
		AbstractSink* s = (*itr);
		s->supportedChanged(this->supported());
	}

}

void Core::updateSupported(PropertyList added, PropertyList removed, AbstractSource* source)
{
	if(!source || mSources.find(source->uuid()) == mSources.end())
		return;

	/// add the newly supported to master list

	handleAddSupported(added, source);
	
	/// removed no longer supported properties from master list.

	handleRemoveSupported(removed, source);
	
	/// tell all new sinks about the newly supported properties.

	for(auto itr = mSinks.begin(); itr != mSinks.end(); ++itr)
	{
		(*itr)->supportedChanged(supported());
	}
}

PropertyList Core::supported()
{
	PropertyList supportedProperties;
	// TODO: should we care here about duplicates ?
	// (if multiple sources supports the same property ==> Property will be more than once in PropertyList)
	transform(mMasterPropertyList.begin(), mMasterPropertyList.end(), back_inserter(supportedProperties),
		[](const std::multimap<AbstractSource*, VehicleProperty::Property>::value_type& itr) { return itr.second; }
	);
	// remove duplicates:
	supportedProperties.sort();
	supportedProperties.unique();

	//DebugOut(1)<<__FUNCTION__<<"supported list: " << endl;
	//for_each(supportedProperties.begin(), supportedProperties.end(), [](const std::string& str){ DebugOut(1)<<__FUNCTION__<< str << endl; });

	return supportedProperties;
}

void Core::updateProperty(AbstractPropertyType *value, const string &uuid)
{
	VehicleProperty::Property property = value->name;

	performance.propertiesPerSecond++;

	//auto sinks = propertySinkMap[property]; !!! this will insert empty std::set<AbstractSink*> into propertySinkMap !!!
	auto sinksIt =  propertySinkMap.find(property);
	if(sinksIt == propertySinkMap.end()){
		DebugOut()<<__FUNCTION__<<"() there are no sinks connected to property: "<<property<<endl;
		return;
	}

	const std::set<AbstractSink*>& sinks = sinksIt->second;

	DebugOut()<<__FUNCTION__<<"() there are "<<sinks.size()<<" sinks connected to property: "<<property<<endl;

	if(sinks.size())// is this check redundant ?
	{
		performance.firedPropertiesPerSecond++;
	}

	for(auto itr = sinks.begin(); itr != sinks.end(); ++itr)
	{
		AbstractSink* sink = *itr;

		auto isFiltered = filteredSourceSinkMap.find(sink);

		if(isFiltered != filteredSourceSinkMap.end() )
		{

			std::string u = filteredSourceSinkMap[sink][property];
			DebugOut()<<"Property ("<<property<<") for sink is filtered for source: "<<u<<endl;
		}

		if( (isFiltered != filteredSourceSinkMap.end() && filteredSourceSinkMap[sink][property] == uuid) || isFiltered == filteredSourceSinkMap.end())
		{
			/// FIXME: Set this here just in case a source neglects to:

			if(value->sourceUuid != uuid)
			{
				//DebugOut(DebugOut::Warning)<<"Source not setting uuid for property "<<value->name<<endl;
				value->sourceUuid = uuid;
			}

			sink->propertyChanged(value);
		}
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

AbstractSource* Core::sourceForProperty(const VehicleProperty::Property& property, const std::string& sourceUuidFilter) const
{
	auto it = mMasterPropertyList.end();
	if(sourceUuidFilter.empty()){
		it = std::find_if(mMasterPropertyList.begin(), mMasterPropertyList.end(),
			[&property](const std::multimap<AbstractSource*, VehicleProperty::Property>::value_type& it) { return it.second == property; }
		);
	}
	else{
		auto itSource = mSources.find(sourceUuidFilter);
		if(itSource != mSources.end()){
			auto range = mMasterPropertyList.equal_range(itSource->second);
			auto temp = find_if(
				range.first,	// the first property in source
				range.second,   // one item right after the last property in source
				[&property](const std::multimap<AbstractSource*, VehicleProperty::Property>::value_type& it) { return it.second == property; }
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
		AbstractSource* src = itr->second;
		if(((src->supportedOperations() & AbstractSource::GetRanged) == AbstractSource::GetRanged)
				&& (request.sourceUuid == "" || request.sourceUuid == src->uuid()))
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

	DebugOut(0)<<"Error: setProperty opration failed"<<endl;
	return NULL;
}

void Core::subscribeToProperty(VehicleProperty::Property property, AbstractSink* self)
{
	DebugOut(1)<<"Subscribing to: "<<property<<endl;

	bool subscribed(false);
	auto itr = mMasterPropertyList.begin();
	while(itr != mMasterPropertyList.end())
	{
		if(itr->second == property) {
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

	if(subscribed)
		propertySinkMap[property].insert(self);
}

void Core::subscribeToProperty(VehicleProperty::Property property, string sourceUuidFilter, AbstractSink *sink)
{
	DebugOut(1)<<"Subscribing to: "<<property<<endl;

	AbstractSource* src = sourceForProperty(property, sourceUuidFilter);
	if(!src)
		return;

	src->subscribeToPropertyChanges(property);

	propertySinkMap[property].insert(sink);

	if(filteredSourceSinkMap.find(sink) == filteredSourceSinkMap.end() && sourceUuidFilter != "")
	{
		std::map<VehicleProperty::Property, std::string> propertyFilter;
		propertyFilter[property] = sourceUuidFilter;
		filteredSourceSinkMap[sink] = propertyFilter;
	}
	else if(sourceUuidFilter != "")
	{
		filteredSourceSinkMap[sink][property] = sourceUuidFilter;
	}
}

void Core::subscribeToProperty(VehicleProperty::Property, string sourceUuidFilter, Zone::Type zoneFilter, AbstractSink *self)
{
	/// TODO: implement
	throw std::runtime_error("Not implemented");
}

void Core::unsubscribeToProperty(VehicleProperty::Property property, AbstractSink* self)
{
	auto sinksIt = propertySinkMap.find(property);
	if(sinksIt == propertySinkMap.end())
	{
		DebugOut(1)<<__FUNCTION__<<" property not subscribed to: "<<property<<endl;
		return; 
	}

	sinksIt->second.erase(self);

	filteredSourceSinkMap.erase(self);

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
}

PropertyInfo Core::getPropertyInfo(VehicleProperty::Property property, string sourceUuid)
{
	auto srcIt = mSources.find(sourceUuid);
	if(srcIt == mSources.end())
		return PropertyInfo::invalid();

	return srcIt->second->getPropertyInfo(property);
}

std::list<string> Core::sourcesForProperty(VehicleProperty::Property property)
{
	std::list<std::string> list;

	auto itr = mMasterPropertyList.begin();
	while(itr != mMasterPropertyList.end())
	{
		if(itr->second == property) {
			AbstractSource* src = itr->first;
			list.push_back(src->uuid());
			itr = mMasterPropertyList.upper_bound(src);
		}
		else{
			++itr;
		}
	}

	//DebugOut(1)<<__FUNCTION__<<"sources list: " << endl;
	//for_each(list.begin(), list.end(), [](const std::string& str){ DebugOut(1)<<__FUNCTION__<< str << endl; });

	return list;
}

