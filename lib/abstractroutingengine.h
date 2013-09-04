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

#include <sys/types.h>
#include <stdlib.h>
#include <boost/any.hpp>
#include <functional>
#include <string>
#include <time.h>

#include "vehicleproperty.h"
#include "abstractpropertytype.h"
#include "propertyinfo.hpp"

class AbstractSink;
class AbstractSource;
class AsyncPropertyReply;
class AsyncRangePropertyReply;


typedef std::function<void (AsyncPropertyReply*)> GetPropertyCompletedSignal;
typedef std::function<void (AsyncRangePropertyReply*)> GetRangedPropertyCompletedSignal;

class AsyncPropertyRequest
{
public:
	AsyncPropertyRequest()
		:property(VehicleProperty::NoValue),timeout(10000)
	{

	}

	AsyncPropertyRequest(const AsyncPropertyRequest &request)
	{
		this->property = request.property;
		this->completed = request.completed;
		this->sourceUuidFilter = request.sourceUuidFilter;
		this->zoneFilter = request.zoneFilter;
		this->timeout = request.timeout;
	}

	AsyncPropertyRequest & operator = (const AsyncPropertyRequest & other)
	{
		this->property = other.property;
		this->completed = other.completed;
		this->sourceUuidFilter = other.sourceUuidFilter;
		this->zoneFilter = other.zoneFilter;
		this->timeout = other.timeout;

		return *this;
	}

	virtual ~AsyncPropertyRequest() { }

	VehicleProperty::Property property;
	std::string sourceUuidFilter;
	Zone::Type zoneFilter;
	GetPropertyCompletedSignal completed;
	uint timeout;
};

class AsyncPropertyReply: public AsyncPropertyRequest
{
public:
	AsyncPropertyReply(const AsyncPropertyRequest &request);

	virtual ~AsyncPropertyReply()
	{
		if(timeoutSource)
		{
			g_source_destroy(timeoutSource);
			g_source_unref(timeoutSource);
		}
	}

	enum Error {
		NoError = 0,
		Timeout,
		InvalidOperation,
		PermissionDenied,
		ZoneNotSupported
	};

	/**
	 * @brief value of the reply.  This may be null if success = false.  This is owned by the source.
	 */
	AbstractPropertyType* value;
	bool success;
	Error error;

private:
	GSource* timeoutSource;
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

	virtual ~AsyncSetPropertyRequest() { }

	AbstractPropertyType* value;
};

class AsyncRangePropertyRequest
{
public:
	AsyncRangePropertyRequest()
		:timeBegin(0), timeEnd(0), sequenceBegin(-1), sequenceEnd(-1)
	{

	}

	AsyncRangePropertyRequest(const AsyncRangePropertyRequest &request)

	{
		this->properties = request.properties;
		this->completed = request.completed;
		this->timeBegin = request.timeBegin;
		this->timeEnd = request.timeEnd;
		this->sequenceBegin = request.sequenceBegin;
		this->sequenceEnd = request.sequenceEnd;
		this->sourceUuid = request.sourceUuid;
	}

	virtual ~AsyncRangePropertyRequest() {}

	PropertyList properties;
	std::string sourceUuid;
	GetRangedPropertyCompletedSignal completed;
	double timeBegin;
	double timeEnd;
	int32_t sequenceBegin;
	int32_t sequenceEnd;
};

class AsyncRangePropertyReply: public AsyncRangePropertyRequest
{
public:
	AsyncRangePropertyReply(AsyncRangePropertyRequest request)
		:AsyncRangePropertyRequest(request), success(false)
	{

	}

	~AsyncRangePropertyReply()
	{
		for(auto itr = values.begin(); itr != values.end(); itr++)
		{
			delete (*itr);
		}

		values.clear();
	}

	AsyncPropertyReply::Error error;

	std::list<AbstractPropertyType*> values;
	bool success;
};

class AbstractRoutingEngine
{
public:
	virtual ~AbstractRoutingEngine();

	virtual void setSupported(PropertyList supported, AbstractSource* source) = 0;
	virtual void updateSupported(PropertyList added, PropertyList removed) = 0;
	/// Deprecated:
	void updateProperty(VehicleProperty::Property property, AbstractPropertyType* value, std::string uuid)
	{
		DebugOut(DebugOut::Warning)<<"updateProperty(VehicleProperty::Property,AbstractPropertyType*,std::string) is deprecated.  use new updateProperty(AbstractPropertyType*, const std::string &)"<<endl;
		updateProperty(value,uuid);
	}

	virtual void updateProperty(AbstractPropertyType* value, const std::string &uuid) = 0;
	virtual PropertyList supported() = 0;

	/// sinks:
	virtual void registerSink(AbstractSink* self) = 0;
	virtual void  unregisterSink(AbstractSink* self) = 0;

	/**
	 * @brief sourcesForProperty
	 * @param property
	 * @return list of source uuid's that support the "property"
	 */
	virtual std::list<std::string> sourcesForProperty(VehicleProperty::Property property) = 0;

	/**
	 * @brief getPropertyAsync requests a property value from a source.  This call has a timeout and will always return.
	 * @see AsyncPropertyRequest
	 * @see AsyncPropertyReply.
	 * @param request requested property.
	 * @return AsyncPropertyReply. The returned AsyncPropertyReply is owned by the caller of getPropertyAsync.
	 * @example AsyncPropertyRequest request;
	 * request.property = VehicleProperty::VehicleSpeed
	 * request.completed = [](AsyncPropertyReply* reply) { delete reply; };
	 * routingEngine->getPropertyAsync(request);
	 */
	virtual AsyncPropertyReply * getPropertyAsync(AsyncPropertyRequest request) = 0;
	virtual AsyncRangePropertyReply * getRangePropertyAsync(AsyncRangePropertyRequest request) = 0;
	virtual AsyncPropertyReply * setProperty(AsyncSetPropertyRequest request) = 0;
	virtual void subscribeToProperty(VehicleProperty::Property, AbstractSink* self) = 0;
	virtual void subscribeToProperty(VehicleProperty::Property, std::string sourceUuidFilter, AbstractSink *self) = 0;
	virtual void subscribeToProperty(VehicleProperty::Property, std::string sourceUuidFilter, Zone::Type zoneFilter, AbstractSink *self) = 0;
	virtual void unsubscribeToProperty(VehicleProperty::Property, AbstractSink* self) = 0;

	virtual PropertyInfo getPropertyInfo(VehicleProperty::Property, std::string sourceUuid) = 0;
	virtual std::list<std::string> getSourcesForProperty(VehicleProperty::Property) = 0;
};

#endif // ABSTRACTROUTINGENGINE_H
