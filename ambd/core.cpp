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
		auto supportedChangedCb = std::bind(&Core::supportedChanged,this, _1, _2);
		(*itr)->setSupportedChangedCb(supportedChangedCb);
		
		auto propChangedDb = std::bind(&Core::propertyChanged, this, _1, _2);
		(*itr)->setPropertyChangedCb(propChangedDb);
	}
}


void Core::supportedChanged(PropertyList added, PropertyList removed)
{
	
	for(PropertyList::iterator itr = added.begin(); itr != added.end(); itr++)
	{
		if(ListPlusPlus<VehicleProperty::Property>(added).contains(*itr))
		{
			mMasterPropertyList.push_back(*itr);
		}
	}
	
	for(PropertyList::iterator itr = removed.begin(); itr != removed.end(); itr++)
	{
		ListPlusPlus<VehicleProperty::Property>(removed).removeOne(*itr);
	}
	
}

void Core::propertyChanged(VehicleProperty::Property property, boost::any value)
{

}

