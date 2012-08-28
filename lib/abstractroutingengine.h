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


#ifndef ABSTRACTROUTINGENGINE_H
#define ABSTRACTROUTINGENGINE_H

#include <boost/any.hpp>
#include <functional>
#include "vehicleproperty.h"
#include "abstractpropertytype.h"

class AbstractSink;
class AbstractSource;
class AsyncPropertyReply;


typedef std::function<void (AsyncPropertyReply*)> CompletedSignal;

class AsyncPropertyRequest
{
public:
	VehicleProperty::Property property;
	CompletedSignal completed;
};

class AsyncPropertyReply: public AsyncPropertyRequest
{
public:
	AsyncPropertyReply(AsyncPropertyRequest request)
	{
		this->property = request.property;
		this->completed = request.completed;
	}

	AbstractPropertyType value;
};

class AbstractRoutingEngine
{
public:
	virtual void setSupported(PropertyList supported, AbstractSource* source) = 0;
	virtual void updateSupported(PropertyList added, PropertyList removed) = 0;
	virtual void updateProperty(VehicleProperty::Property property, AbstractPropertyType value) = 0;
	
	/// sinks:
	virtual void registerSink(AbstractSink* self) = 0;
	virtual void  unregisterSink(AbstractSink* self) = 0;
	virtual AsyncPropertyReply *getPropertyAsync(AsyncPropertyRequest request) = 0;
	virtual void setProperty(VehicleProperty::Property, AbstractPropertyType) = 0;
	virtual void subscribeToProperty(VehicleProperty::Property, AbstractSink* self) = 0;
	virtual void unsubscribeToProperty(VehicleProperty::Property, AbstractSink* self) = 0;
	virtual PropertyList supported() = 0;
};

#endif // ABSTRACTROUTINGENGINE_H
