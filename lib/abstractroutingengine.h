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
		this->sourceUuid = request.sourceUuid;
		this->timeout = request.timeout;
	}

	AsyncPropertyRequest & operator = (const AsyncPropertyRequest & other)
	{
		this->property = other.property;
		this->completed = other.completed;
		this->sourceUuid = other.sourceUuid;
		this->timeout = other.timeout;

		return *this;
	}

	VehicleProperty::Property property;
	std::string sourceUuid;
	GetPropertyCompletedSignal completed;
	uint timeout;
};

class AsyncPropertyReply: public AsyncPropertyRequest
{
public:
	AsyncPropertyReply(const AsyncPropertyRequest &request);
/*		:AsyncPropertyRequest(request), value(NULL), success(false), timeoutSource(nullptr)
	{
		if(timeout)
		{

			auto timeoutfunc = [](gpointer userData) {
                        	AsyncPropertyReply* thisReply = static_cast<AsyncPropertyReply*>(userData);
	                        if(thisReply->success == false)
        	                {
                	                thisReply->error = Timeout;
                        	        thisReply->completed(thisReply);
	                        }
        	                return 0;
                	};
			timeoutSource = g_timeout_source_new(timeout);
			g_source_set_callback(timeoutSource, [](gpointer userData) {
                                AsyncPropertyReply* thisReply = static_cast<AsyncPropertyReply*>(userData);
                                if(thisReply->success == false)
                                {
                                        thisReply->error = Timeout;
                                        thisReply->completed(thisReply);
                                }
                                return 0;
                        }, this, nullptr);
			g_source_attach(timeoutSource, nullptr);
		}
	}*/

	~AsyncPropertyReply()
	{
		if(timeoutSource)
		{
			g_source_destroy(timeoutSource);
			g_source_unref(timeoutSource);
		}
	}

	enum Error {
		NoError,
		Timeout,
		InvalidOperation,
		PermissionDenied
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
		this->property = request.property;
		this->completed = request.completed;
		this->timeBegin = request.timeBegin;
		this->timeEnd = request.timeEnd;
		this->sequenceBegin = request.sequenceBegin;
		this->sequenceEnd = request.sequenceEnd;
		this->sourceUuid = request.sourceUuid;
	}

	VehicleProperty::Property property;
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

	std::list<AbstractPropertyType*> values;
	bool success;
};

class AbstractRoutingEngine
{
public:
	virtual ~AbstractRoutingEngine();

	virtual void setSupported(PropertyList supported, AbstractSource* source) = 0;
	virtual void updateSupported(PropertyList added, PropertyList removed) = 0;
	virtual void updateProperty(VehicleProperty::Property property, AbstractPropertyType* value, std::string uuid) = 0;

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
