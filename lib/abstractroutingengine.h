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

#include "sys/types.h"
#include <stdlib.h>

#include <boost/any.hpp>
#include <functional>
#include <time.h>
#include "vehicleproperty.h"
#include "abstractpropertytype.h"

class AbstractSink;
class AbstractSource;
class AsyncPropertyReply;


typedef std::function<void (AsyncPropertyReply*)> GetPropertyCompletedSignal;

class PropertyValueTime {
public:
	AbstractPropertyType* value;
	time_t timestamp;
};

class AsyncPropertyRequest
{
public:
	AsyncPropertyRequest()
		:property(VehicleProperty::NoValue)
	{

	}

	AsyncPropertyRequest(const AsyncPropertyRequest &request)
	{
		this->property = request.property;
		this->completed = request.completed;
	}

	AsyncPropertyRequest & operator = (const AsyncPropertyRequest & other)
	{
		this->property = other.property;
		this->completed = other.completed;

		return *this;
	}

	VehicleProperty::Property property;
	GetPropertyCompletedSignal completed;
};

class AsyncPropertyReply: public AsyncPropertyRequest
{
public:
	AsyncPropertyReply(const AsyncPropertyRequest &request)
		:AsyncPropertyRequest(request), value(NULL), success(false)
	{

	}

	AbstractPropertyType* value;
	bool success;
};

class AsyncSetPropertyRequest: public AsyncPropertyRequest
{
public:
	AsyncSetPropertyRequest()
		:value(NULL)
	{

	}

	AsyncSetPropertyRequest(const AsyncPropertyRequest &request)
		:AsyncPropertyRequest(request), value(NULL)
	{

	}

	AbstractPropertyType* value;
};

class AsyncRangePropertyRequest: public AsyncPropertyRequest
{
public:
	AsyncRangePropertyRequest()
		:begin(0), end(0)
	{

	}

	AsyncRangePropertyRequest(const AsyncPropertyRequest &request)
		:AsyncPropertyRequest(request), begin(0), end(0)
	{
		this->property = request.property;
		this->completed = request.completed;
	}

	AsyncRangePropertyRequest(const AsyncRangePropertyRequest &request)
		:AsyncPropertyRequest(request)
	{
		this->property = request.property;
		this->completed = request.completed;
		this->begin = request.begin;
		this->end = request.end;
	}

	time_t begin;
	time_t end;
};

class AsyncRangePropertyReply: public AsyncRangePropertyRequest
{
public:
	AsyncRangePropertyReply(AsyncRangePropertyRequest request)
		:AsyncRangePropertyRequest(request)
	{

	}

	std::list<PropertyValueTime*> values;
};

class AbstractRoutingEngine
{
public:
	virtual void setSupported(PropertyList supported, AbstractSource* source) = 0;
	virtual void updateSupported(PropertyList added, PropertyList removed) = 0;
	virtual void updateProperty(VehicleProperty::Property property, AbstractPropertyType* value) = 0;

	/// sinks:
	virtual void registerSink(AbstractSink* self) = 0;
	virtual void  unregisterSink(AbstractSink* self) = 0;
	virtual AsyncPropertyReply * getPropertyAsync(AsyncPropertyRequest request) = 0;
	virtual AsyncRangePropertyReply * getRangePropertyAsync(AsyncRangePropertyRequest request) = 0;
	virtual AsyncPropertyReply * setProperty(AsyncSetPropertyRequest request) = 0;
	virtual void subscribeToProperty(VehicleProperty::Property, AbstractSink* self) = 0;
	virtual void unsubscribeToProperty(VehicleProperty::Property, AbstractSink* self) = 0;
	virtual PropertyList supported() = 0;
};

#endif // ABSTRACTROUTINGENGINE_H
