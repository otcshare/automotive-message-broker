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


#ifndef ABSTRACTSINK_H
#define ABSTRACTSINK_H

#include <string>
#include <list>
#include <functional>
#include <boost/any.hpp>

#include "vehicleproperty.h"
#include "abstractpropertytype.h"

using namespace std;

class AbstractRoutingEngine;
class AbstractSink;

typedef list<AbstractSink*> SinkList;

class AbstractSink
{

public:
	AbstractSink(AbstractRoutingEngine* engine);
	virtual ~AbstractSink();
	
	///Pure virtual methods:
	
	/*! uuid() is a unique identifier
	  * @return a guid-style unique identifier
	  */
	virtual string uuid() = 0;

	/*! propertyChanged is called when a subscribed to property changes.
	  * @see AbstractRoutingEngine::subscribeToPropertyChanges()
	  * @param property name that changed
	  * @param value value of the property that changed. this is a temporary pointer that will be destroyed.
	  * Do not destroy it.  If you need to store the value use value.anyValue() or value.value<T>() to copy.
	  * @param uuid Unique identifier representing the source
	  */
	virtual void propertyChanged(VehicleProperty::Property property, AbstractPropertyType* value, string  uuid) = 0;
	virtual void supportedChanged(PropertyList supportedProperties) = 0;
	
protected:
	AbstractRoutingEngine* routingEngine;
};

class AbstractSinkManager
{
public:
	
	AbstractSinkManager(AbstractRoutingEngine* engine);
	
protected:
	AbstractRoutingEngine* routingEngine;
};

#endif // ABSTRACTSINK_H
