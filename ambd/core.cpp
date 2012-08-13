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

using namespace std::placeholders;

Core::Core(SourceList sources, SinkList sinks)
: mSources(sources), mSinks(sinks)
{
	///Hook up signals for each source
	
	for(SourceList::iterator itr = mSources.begin(); itr!=mSources.end(); itr++)
	{
		auto supportedChangedCb = std::bind(&Core::supportedChanged, this, _1, _2);
		(*itr)->setSupportedChangedCb(supportedChangedCb);
		
		auto propChangedDb = std::bind(&Core::propertyChanged, this, _1, _2);
		(*itr)->setPropertyChangedCb(propChangedDb);
	}
	
	for(SinkList::iterator itr = mSinks.begin(); itr != mSinks.end(); itr++)
	{
		auto setPropertyCb = std::bind(&Core::setProperty, this, _1, _2);
		(*itr)->setSetPropertyCb(setPropertyCb);
		
		auto subscribeToPropertyCb = std::bind(&Core::subscribeToProperty, this, _1, _2);
		(*itr)->setSubcribeToPropertyCb(subscribeToPropertyCb);
		
		auto unsubscribeToPropertyCb = std::bind(&Core::unsubscribeToProperty, this, _1, _2);
		(*itr)->setUnsubscribeToPropertyCb(unsubscribeToPropertyCb);
	}
}


void Core::supportedChanged(PropertyList added, PropertyList removed)
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
		(*itr)->setSupported(mMasterPropertyList);
	}
	
	/// iterate through subscribed properties and resubscribe.  This catches newly supported properties in the process.
	
	for(unordered_map<VehicleProperty::Property, SinkList>::iterator itr = propertySinkMap.begin(); itr != propertySinkMap.end(); itr++)
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

void Core::propertyChanged(VehicleProperty::Property property, boost::any value)
{

}

void Core::setProperty(VehicleProperty::Property , boost::any )
{

}

void Core::subscribeToProperty(VehicleProperty::Property property, AbstractSink* self)
{
	
}

void Core::unsubscribeToProperty(VehicleProperty::Property , AbstractSink* self)
{

}


