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

static int PPSUpdate(void* data)
{
	int* pps = (int*)data;

	int temp = *pps;

	if(temp > 0 && temp != lastpps)
	{
		lastpps = temp;
		DebugOut(1)<<"Property updates per second: "<<temp<<endl;
	}

	*pps = 0;
}

Core::Core()
	:propertiesPerSecond(0)
{
	g_timeout_add(1000,PPSUpdate,&propertiesPerSecond);
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


void Core::setSupported(PropertyList supported, AbstractSource* source)
{

	if(!ListPlusPlus<AbstractSource*>(&mSources).contains(source))
		mSources.push_back(source);
		
	for(PropertyList::iterator itr = supported.begin(); itr != supported.end(); itr++)
	{
		if(!ListPlusPlus<VehicleProperty::Property>(&mMasterPropertyList).contains((*itr)))
		{
			DebugOut()<<__FUNCTION__<<"() adding support for property "<<(*itr)<<endl;
			mMasterPropertyList.push_back((*itr));
		}
	}

	/// tell all new sinks about the newly supported properties.

	for(SinkList::iterator itr = mSinks.begin(); itr != mSinks.end(); itr++)
	{
		AbstractSink* s = (*itr);
		s->supportedChanged(mMasterPropertyList);
	}

	/// iterate through subscribed properties and resubscribe.  This catches newly supported properties in the process.

	for(map<VehicleProperty::Property, SinkList>::iterator itr = propertySinkMap.begin(); itr != propertySinkMap.end(); itr++)
	{
		VehicleProperty::Property  property = (*itr).first;

		for(SourceList::iterator source = mSources.begin(); source != mSources.end(); source++)
		{
			PropertyList properties = (*source)->supported();

			if(ListPlusPlus<VehicleProperty::Property>(&properties).contains(property))
			{
				(*source)->subscribeToPropertyChanges(property);
			}
		}
	}
}


void Core::updateSupported(PropertyList added, PropertyList removed)
{
	
	/// add the newly supported to master list
	
	for(PropertyList::iterator itr = added.begin(); itr != added.end(); itr++)
	{
		if(ListPlusPlus<VehicleProperty::Property>(&added).contains(*itr))
		{
			mMasterPropertyList.push_back(*itr);
		}
	}
	
	/// removed no longer supported properties from master list.
	
	for(PropertyList::iterator itr = removed.begin(); itr != removed.end(); itr++)
	{
		ListPlusPlus<VehicleProperty::Property>(&mMasterPropertyList).removeOne(*itr);
	}
	
	/// tell all new sinks about the newly supported properties.
	
	for(SinkList::iterator itr = mSinks.begin(); itr != mSinks.end(); itr++)
	{
		(*itr)->supportedChanged(mMasterPropertyList);
	}
	
	/// iterate through subscribed properties and resubscribe.  This catches newly supported properties in the process.
	
	for(map<VehicleProperty::Property, SinkList>::iterator itr = propertySinkMap.begin(); itr != propertySinkMap.end(); itr++)
	{
		VehicleProperty::Property  property = (*itr).first;
		
		for(SourceList::iterator source = mSources.begin(); source != mSources.end(); source++)
		{
			PropertyList properties = (*source)->supported();
			
			if(ListPlusPlus<VehicleProperty::Property>(&properties).contains(property))
			{
				(*source)->subscribeToPropertyChanges(property);
			}
		}
	}
}

void Core::updateProperty(VehicleProperty::Property property, AbstractPropertyType *value, std::string uuid)
{
	SinkList list = propertySinkMap[property];
	
	DebugOut()<<__FUNCTION__<<"() there are "<<list.size()<<" sinks connected to property: "<<property<<endl;

	propertiesPerSecond++;


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

			sink->propertyChanged(property, value, uuid);
		}
	}
}

std::list<std::string> Core::sourcesForProperty(VehicleProperty::Property property)
{
	std::list<std::string> l;

	for(auto itr = mSources.begin(); itr != mSources.end(); itr++)
	{
		AbstractSource* src = *itr;

		PropertyList s = src->supported();

		if(ListPlusPlus<VehicleProperty::Property>(&s).contains(property))
		{
			l.push_back(src->uuid());
		}
	}

	return l;
}

void Core::registerSink(AbstractSink *self)
{
	if(!ListPlusPlus<AbstractSink*>(&mSinks).contains(self))
	{
		mSinks.push_back(self);
	}
}

void Core::unregisterSink(AbstractSink *self)
{
	if(ListPlusPlus<AbstractSink*>(&mSinks).contains(self))
	{
		ListPlusPlus<AbstractSink*>(&mSinks).removeOne(self);
	}
}


AsyncPropertyReply *Core::getPropertyAsync(AsyncPropertyRequest request)
{
	AsyncPropertyReply * reply = new AsyncPropertyReply(request);

	for(SourceList::iterator itr = mSources.begin(); itr != mSources.end(); itr++)
	{
		AbstractSource* src = (*itr);
		PropertyList properties = src->supported();
		int supportedOps = src->supportedOperations();

		bool supportsGet = supportedOps & AbstractSource::Get;

		if(ListPlusPlus<VehicleProperty::Property>(&properties).contains(request.property) && supportsGet && (request.sourceUuidFilter == "" || request.sourceUuidFilter == src->uuid()))
		{

			src->getPropertyAsync(reply);

			/** right now the owner of the reply becomes the requestor that called this method.
			 *  reply will become invalid after the first reply. */
			return reply;
		}
	}

	return reply;
}

AsyncRangePropertyReply *Core::getRangePropertyAsync(AsyncRangePropertyRequest request)
{
	AsyncRangePropertyReply * reply = new AsyncRangePropertyReply(request);

	for(SourceList::iterator itr = mSources.begin(); itr != mSources.end(); itr++)
	{
		AbstractSource* src = (*itr);
		if((src->supportedOperations() & AbstractSource::GetRanged)
				&& (request.sourceUuid == "" || request.sourceUuid == src->uuid()))
		{
			src->getRangePropertyAsync(reply);
		}
	}

	return reply;
}

AsyncPropertyReply * Core::setProperty(AsyncSetPropertyRequest request)
{
	for(SourceList::iterator itr = mSources.begin(); itr != mSources.end(); itr++)
	{
		AbstractSource* src = (*itr);
		PropertyList properties = src->supported();
		if(ListPlusPlus<VehicleProperty::Property>(&properties).contains(request.property) && src->supportedOperations() & AbstractSource::Set)
		{
			return src->setProperty(request);
		}
	}

	DebugOut(0)<<"Error: setProperty opration failed"<<endl;
	return NULL;
}

void Core::subscribeToProperty(VehicleProperty::Property property, AbstractSink* self)
{
	DebugOut(1)<<"Subscribing to: "<<property<<endl;

	if(propertySinkMap.find(property) == propertySinkMap.end())
	{
		propertySinkMap[property] = SinkList();
	}

	SinkList list = propertySinkMap[property];

	if(!ListPlusPlus<AbstractSink*>(&list).contains(self))
	{
		propertySinkMap[property].push_back(self);
	}

	for(SourceList::iterator itr = mSources.begin(); itr != mSources.end(); itr++)
	{
		AbstractSource* src = (*itr);
		PropertyList properties = src->supported();
		if(ListPlusPlus<VehicleProperty::Property>(&properties).contains(property))
		{
			src->subscribeToPropertyChanges(property);
		}
	}

}

void Core::subscribeToProperty(VehicleProperty::Property property, string sourceUuidFilter, AbstractSink *sink)
{
	if(filteredSourceSinkMap.find(sink) == filteredSourceSinkMap.end() && sourceUuidFilter != "")
	{
		std::map<VehicleProperty::Property, std::string> propertyFilter;
		propertyFilter[property] = sourceUuidFilter;
		filteredSourceSinkMap[sink] = propertyFilter;
	}

	subscribeToProperty(property,sink);
}

void Core::subscribeToProperty(VehicleProperty::Property, string sourceUuidFilter, Zone::Type zoneFilter, AbstractSink *self)
{

}

void Core::unsubscribeToProperty(VehicleProperty::Property property, AbstractSink* self)
{
	if(propertySinkMap.find(property) == propertySinkMap.end())
	{
		DebugOut(1)<<__FUNCTION__<<" property not subscribed to: "<<property<<endl;
		return; 
	}
		
	ListPlusPlus<AbstractSink*>(&propertySinkMap[property]).removeOne(self);

	if( filteredSourceSinkMap.find(self) != filteredSourceSinkMap.end())
	{
		filteredSourceSinkMap.erase(self);
	}

	/// Now we check to see if this is the last subscriber
	if(propertySinkMap.find(property) == propertySinkMap.end())
	{
		for(SourceList::iterator itr = mSources.begin(); itr != mSources.end(); itr++)
		{
			AbstractSource* src = (*itr);
			PropertyList properties = src->supported();

			if(ListPlusPlus<VehicleProperty::Property>(&properties).contains(property))
			{
				src->unsubscribeToPropertyChanges(property);
			}
		}
	}
}

PropertyInfo Core::getPropertyInfo(VehicleProperty::Property property, string sourceUuid)
{
	for(auto itr = mSources.begin(); itr != mSources.end(); itr++)
	{
		AbstractSource* src = *itr;

		if(src->uuid() == sourceUuid)
		{
			return src->getPropertyInfo(property);
		}
	}

	return PropertyInfo::invalid();
}

std::list<string> Core::getSourcesForProperty(VehicleProperty::Property property)
{
	std::list<std::string> list;

	for(auto itr = mSources.begin(); itr != mSources.end(); itr++)
	{
		AbstractSource* src = *itr;

		PropertyList supportedProperties = src->supported();

		if(ListPlusPlus<VehicleProperty::Property>(&supportedProperties).contains(property))
		{
			list.push_back(src->uuid());
		}
	}

	return list;
}

