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
	std::vector<AbstractSink*> toDelete;
	for(auto itr = mSinks.begin(); itr != mSinks.end(); itr++)
	{
		AbstractSink* sink = *itr;
		toDelete.push_back(sink);
	}

	for(int i=0; i<toDelete.size(); i++)
	{
		delete toDelete[i];
	}
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
        // TODO: We do not have enough info about source in propertySinkMap so we do not know if we can/should remove property from propertySinkMap,
        // but I suppose this should be handled in AbstractSink::supportedChanged()

        const VehicleProperty::Property property(*itr);

        auto it = find_if(
            range.first,    // the first property in source
            range.second,   // one item right after the last property in source
            [&property](const std::multimap<AbstractSource*, VehicleProperty::Property>::value_type& it) { return it.second == property; }
        );

        if (it != range.second)// property was found
        {
            mMasterPropertyList.erase(it);// References and iterators to the erased elements are invalidated. Other iterators and references are not invalidated.

            // TODO: Do we need to unsubscribe here ???
            // I added it here:
            source->unsubscribeToPropertyChanges(*itr);
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

    for(SinkList::iterator itr = mSinks.begin(); itr != mSinks.end(); ++itr)
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

    for(SinkList::iterator itr = mSinks.begin(); itr != mSinks.end(); itr++)
	{
		(*itr)->supportedChanged(supported());
	}
}

PropertyList Core::supported()
{
    PropertyList supportedProperties;
    // TODO: should we care here about duplicates (check if multiple sources supports the same property)???
    transform(mMasterPropertyList.begin(), mMasterPropertyList.end(), back_inserter(supportedProperties),
        [](const std::multimap<AbstractSource*, VehicleProperty::Property>::value_type& itr) { return itr.second; }
    );

    return supportedProperties;
}

void Core::updateProperty(AbstractPropertyType *value, const string &uuid)
{
	VehicleProperty::Property property = value->name;

	SinkList list = propertySinkMap[property];

	DebugOut()<<__FUNCTION__<<"() there are "<<list.size()<<" sinks connected to property: "<<property<<endl;

	performance.propertiesPerSecond++;

	if(list.size())
	{
		performance.firedPropertiesPerSecond++;
	}

	for(SinkList::iterator itr = list.begin(); itr != list.end(); itr++)
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
	if(!contains(mSinks,self))
	{
		mSinks.push_back(self);
	}
}

void Core::unregisterSink(AbstractSink *self)
{
	removeOne(&mSinks, self);
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
        if(itSource != mSources.end())
            it = mMasterPropertyList.find(itSource->second);
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

    SinkList& list = propertySinkMap[property];

    if(!contains(list, self))
	{
		list.push_back(self);
	}

    decltype(mMasterPropertyList) temp;
    std::remove_copy_if(mMasterPropertyList.begin(), mMasterPropertyList.end(), std::inserter(temp, temp.begin()),
        [&property](const typename decltype(mMasterPropertyList)::value_type& value) { return value.second != property ? true : false; }
    );

    std::for_each(temp.begin(), temp.end(),
        [&property](const typename decltype(mMasterPropertyList)::value_type& value) { return value.first->subscribeToPropertyChanges(property); }
    );
}

void Core::subscribeToProperty(VehicleProperty::Property property, string sourceUuidFilter, AbstractSink *sink)
{
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

	AbstractSource* src = sourceForProperty(property, sourceUuidFilter);
	if(src)
	    src->subscribeToPropertyChanges(property);
}

void Core::subscribeToProperty(VehicleProperty::Property, string sourceUuidFilter, Zone::Type zoneFilter, AbstractSink *self)
{
	/// TODO: implement
	throw std::runtime_error("Not implemented");
}

void Core::unsubscribeToProperty(VehicleProperty::Property property, AbstractSink* self)
{
	if(propertySinkMap.find(property) == propertySinkMap.end())
	{
		DebugOut(1)<<__FUNCTION__<<" property not subscribed to: "<<property<<endl;
		return; 
	}
		
	removeOne(&propertySinkMap[property],self);

	if( filteredSourceSinkMap.find(self) != filteredSourceSinkMap.end())
	{
		filteredSourceSinkMap.erase(self);
	}

	/// Now we check to see if this is the last subscriber
	if(propertySinkMap.find(property) == propertySinkMap.end())
	{
	    decltype(mMasterPropertyList) temp;
        std::remove_copy_if(mMasterPropertyList.begin(), mMasterPropertyList.end(), std::inserter(temp, temp.begin()),
            [&property](const typename decltype(mMasterPropertyList)::value_type& itr) { return itr.second != property; }
        );
        std::for_each(temp.begin(), temp.end(),
            [&property](const typename decltype(mMasterPropertyList)::value_type& itr) { return itr.first->unsubscribeToPropertyChanges(property); }
        );
    }
}

PropertyInfo Core::getPropertyInfo(VehicleProperty::Property property, string sourceUuid)
{
	for(auto itr = mSources.begin(); itr != mSources.end(); itr++)
	{
		AbstractSource* src = itr->second;

		if(src->uuid() == sourceUuid)
		{
			return src->getPropertyInfo(property);
		}
	}

	return PropertyInfo::invalid();
}

std::list<string> Core::sourcesForProperty(VehicleProperty::Property property)
{
	std::list<std::string> list;
	decltype(mMasterPropertyList) temp;
	std::remove_copy_if(mMasterPropertyList.begin(), mMasterPropertyList.end(), std::inserter(temp, temp.begin()),
        [property](const typename decltype(mMasterPropertyList)::value_type& itr) { return itr.second != property; }
	);
	std::transform( temp.begin(), temp.end(), back_inserter(list),
        [](const typename decltype(mMasterPropertyList)::value_type& itr) { return itr.first->uuid(); }
	);

	std::list<std::string> list2;
	for(auto itr = mMasterPropertyList.begin(); itr != mMasterPropertyList.end(); ++itr)
	{
		if(itr->second == property) {
		    list2.push_back(itr->first->uuid());
		    itr = mMasterPropertyList.upper_bound(itr->first);
		    --itr;
		}
	}
	assert(list != list2);

	return list;
}

