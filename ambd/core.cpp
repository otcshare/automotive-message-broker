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
#include "listplusplus.h"
#include "debugout.h"

using namespace std::placeholders;

Core::Core()
{
	
}

Core::~Core()
{

}


void Core::setSupported(PropertyList supported, AbstractSource* source)
{
	mSources.push_back(source);
		
	for(PropertyList::iterator itr = supported.begin(); itr != supported.end(); itr++)
	{
		if(!ListPlusPlus<VehicleProperty::Property>(&mMasterPropertyList).contains((*itr)))
		{
			DebugOut()<<__FUNCTION__<<"() adding suport for property "<<(*itr)<<endl;
			mMasterPropertyList.push_back((*itr));
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

void Core::updateProperty(VehicleProperty::Property property, boost::any value)
{
	SinkList list = propertySinkMap[property];
	
	DebugOut()<<__FUNCTION__<<"() there are "<<list.size()<<" sinks connected to property: "<<property<<endl;
	
	for(SinkList::iterator itr = list.begin(); itr != list.end(); itr++)
	{
		(*itr)->propertyChanged(property, value,(*itr)->uuid());
	}
}

boost::any Core::getProperty(VehicleProperty::Property property)
{
	for(SourceList::iterator itr = mSources.begin(); itr != mSources.end(); itr++)
	{
		AbstractSource* src = (*itr);
		PropertyList properties = src->supported();
		if(ListPlusPlus<VehicleProperty::Property>(&properties).contains(property))
		{
			return src->getProperty(property);
		}
	}
}

AsyncPropertyReply *Core::getPropertyAsync(AsyncPropertyRequest request)
{
	AsyncPropertyReply * reply = new AsyncPropertyReply(request);

	for(SourceList::iterator itr = mSources.begin(); itr != mSources.end(); itr++)
	{
		AbstractSource* src = (*itr);
		PropertyList properties = src->supported();
		if(ListPlusPlus<VehicleProperty::Property>(&properties).contains(request.property))
		{
			src->getPropertyAsync(reply);
		}
	}

	return reply;
}

void Core::setProperty(VehicleProperty::Property property, boost::any value)
{
	for(SourceList::iterator itr = mSources.begin(); itr != mSources.end(); itr++)
	{
		AbstractSource* src = (*itr);
		PropertyList properties = src->supported();
		if(ListPlusPlus<VehicleProperty::Property>(&properties).contains(property))
		{
			src->setProperty(property, value);
		}
	}
}

void Core::subscribeToProperty(VehicleProperty::Property property, AbstractSink* self)
{
	if(!ListPlusPlus<VehicleProperty::Property>(&mMasterPropertyList).contains((property)))
	{
		DebugOut()<<__FUNCTION__<<"(): property not supported: "<<property<<endl;
		return; 
	}
	
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

void Core::unsubscribeToProperty(VehicleProperty::Property property, AbstractSink* self)
{
	if(propertySinkMap.find(property) == propertySinkMap.end())
	{
		DebugOut()<<__FUNCTION__<<"property not supported: "<<property;
		return; 
	}
		
	ListPlusPlus<AbstractSink*>(&propertySinkMap[property]).removeOne(self);
	
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

