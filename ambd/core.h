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

#include <unordered_map>

class Core
{

public:
    Core(SourceList sources, SinkList sinks);
    
    
private: ///methods

	void supportedChanged(PropertyList added, PropertyList removed);
	void propertyChanged(VehicleProperty::Property property, boost::any value);
	
	///sinks:
	
	void setProperty(VehicleProperty::Property, boost::any);
	void subscribeToProperty(VehicleProperty::Property, AbstractSink* self);
	void unsubscribeToProperty(VehicleProperty::Property, AbstractSink* self);
    
private:
	PropertyList mMasterPropertyList;
	
	SourceList mSources;
	SinkList mSinks;
	
	unordered_map<VehicleProperty::Property, SinkList> propertySinkMap;
    
};

#endif // CORE_H
