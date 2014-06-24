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
class AsyncSetPropertyRequest;


typedef std::function<void (AsyncPropertyReply*)> GetPropertyCompletedSignal;
typedef std::function<void (AsyncRangePropertyReply*)> GetRangedPropertyCompletedSignal;

/*!
 * \brief The AsyncPropertyRequest class is used by sinks to request property values.
 * \see AbstractRoutingEngine::getPropertyAsync
 * \see AsyncPropertyReply
 */
class AsyncPropertyRequest
{
public:
	AsyncPropertyRequest()
		:property(VehicleProperty::NoValue),zoneFilter(Zone::None), timeout(10000)
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

	/*!
	 * \brief property property to request.
	 */
	VehicleProperty::Property property;

	/*!
	 * \brief sourceUuidFilter the requesting sink should use this to filter on a specific source or leave blank to use any source
	 */
	std::string sourceUuidFilter;

	/*!
	 * \brief zoneFilter the requesting sink should use this if he wants to filter on a specific zone
	 */
	Zone::Type zoneFilter;

	/*!
	 * \brief completed the callback when the request has been completed.
	 */
	GetPropertyCompletedSignal completed;

	/*!
	 * \brief use to specify a timeout in ms for the request.  When a timeout occurs, the 'completed' callback
	 * will be called with an error.  @see AsyncPropertyReply
	 * default value is: 10000 ms
	 */
	uint timeout;

	/*!
	 * \brief pid requesting process id
	 */
	std::string pid;
};

/*!
 * \brief The AsyncPropertyReply class is used by sources to reply to Get and Set operations.  The source should
 * set success to true if the call is successful or 'false' if the request was not successful and set 'error'
 * to the appropriate error.
 * \see AbstractRoutingEngine::getPropertyAsync
 * \see AsyncPropertyReply
 * \see AbstractSource::Operations
 * \see AbstractSource::getPropertyAsync
 */
class AsyncPropertyReply: public AsyncPropertyRequest
{
public:
	AsyncPropertyReply();

	AsyncPropertyReply(const AsyncPropertyRequest &request);

	AsyncPropertyReply(const AsyncSetPropertyRequest &request);

	virtual ~AsyncPropertyReply()
	{
		if(timeoutSource)
		{
			g_source_destroy(timeoutSource);
			g_source_unref(timeoutSource);
		}
	}

	/*!
	 * \brief The Error enum
	 */
	enum Error {
		NoError = 0,
		Timeout,
		InvalidOperation,
		PermissionDenied,
		ZoneNotSupported
	};

	/*!
	 * \brief value of the reply.  This may be null if success = false.  This is owned by the source.
	 */
	AbstractPropertyType* value;

	/*!
	 * \brief success indicates if the request was successfull or not.  True means success.  False means fail and the 'error'
	 * member should be set.
	 */
	bool success;

	/*!
	 * \brief error contains the error if the request was not successful.\
	 * \see Error
	 */
	Error error;

private:
	void setTimeout();
	GSource* timeoutSource;
};

/*!
 * \brief The AsyncSetPropertyRequest class is used by sinks to set a property to the 'value'.  The source will reply
 * with a AsyncPropertyReply containing the new value or an error.
 * \see AbstractRoutingEngine::setProperty
 * \see AsyncPropertyReply
 */
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

	virtual ~AsyncSetPropertyRequest()
	{

	}

	/*!
	 * \brief value the new value to set the property to.
	 */
	AbstractPropertyType* value;
};

/*!
 * \brief The AsyncRangePropertyRequest class is used by sinks to request values within a time or sequence range
 * \see AbstractRoutingEngine::getRangePropertyAsync
 */
class AsyncRangePropertyRequest
{
public:
	AsyncRangePropertyRequest()
		:zone(Zone::None), timeBegin(0), timeEnd(0), sequenceBegin(-1), sequenceEnd(-1)
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
		this->zone = request.zone;
	}

	virtual ~AsyncRangePropertyRequest() {}

	/*!
	 * \brief properties list of properties to request
	 */
	PropertyList properties;

	/*!
	 * \brief sourceUuid if the sink wishes to request a specific source, this should be set to the uuid of the source.
	 */
	std::string sourceUuid;

	/*!
	 * \brief zone if the sink wishes to request a specific zone, this should be set to the desired zone .
	 */
	Zone::Type zone;

	/*!
	 * \brief completed callback that is called when the ranged request is complete. The reply from this request is passed
	 * into the completed call.  The completed callback must free the reply before it returns or there will be a leak.
	 */
	GetRangedPropertyCompletedSignal completed;

	/*!
	 * \brief timeBegin set this to request values for the specified property beggining at this time.  Time is seconds\
	 * since the unix epoc.  Set this to '0' if you do not want values within a time range.
	 */
	double timeBegin;

	/*!
	 * \brief timeEnd set this to request values for the specified property beggining at this time.  Time is seconds\
	 * since the unix epoc.  Set this to '0' if you do not want values within a time range.
	 */
	double timeEnd;

	/*!
	 * \brief sequenceBegin set this to request values with a sequence >= to the sequenceBegin value.  Set to -1 if
	 * you don't want values within a sequence ranges.
	 */
	int32_t sequenceBegin;

	/*!
	 * \brief sequenceEnd set this to request values with a sequence <= to the sequenceEnd value.  Set to -1 if
	 * you don't want values within a sequence ranges.
	 */
	int32_t sequenceEnd;

	/*!
	 * \brief pid requesting process id
	 */
	std::string pid;
};

/*!
 * \brief The AsyncRangePropertyReply class is used by a source to reply to an AsyncRangePropertyRequest.
 * the source should set success to 'true' and populate the 'values' member if the request was successful.
 * If the request is not successful, 'success' should be set to 'false' and the 'error' member should be set.
 */
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

	/*!
	 * \brief error this is set if there was an error in the request.  "success" will also be set to false.
	 */
	AsyncPropertyReply::Error error;

	/*!
	 * \brief values if the request was successful, this will contain a list of values meeting the criteria of the request.
	 */
	std::list<AbstractPropertyType*> values;

	/*!
	 * \brief success this will be true if the request was successful.  If not, this is false and error is set.
	 */
	bool success;
};

class AbstractRoutingEngine
{
public:
	typedef std::function<void (AbstractPropertyType* value)> PropertyChangedType;
	virtual ~AbstractRoutingEngine();

	virtual void registerSource(AbstractSource* src) = 0;
	virtual void updateSupported(PropertyList added, PropertyList removed, AbstractSource* source) = 0;


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
	 * /brief sourcesForProperty
	 * /param property
	 * /return list of source uuid's that support the "property"
	 */
	virtual std::list<std::string> sourcesForProperty(VehicleProperty::Property property) = 0;

	/**
	 * /brief getPropertyAsync requests a property value from a source.  This call has a timeout and the callback specified in the request will always be called.
	 * /see AsyncPropertyRequest
	 * /see AsyncPropertyReply.
	 * /param request requested property.
	 * /return AsyncPropertyReply. The returned AsyncPropertyReply is owned by the caller of getPropertyAsync.
	 * /example AsyncPropertyRequest request;
	 * request.property = VehicleProperty::VehicleSpeed
	 * request.completed = [](AsyncPropertyReply* reply)
	 * {
	 *   //you own the reply
	 *   delete reply;
	 * };
	 * routingEngine->getPropertyAsync(request);
	 */
	virtual AsyncPropertyReply * getPropertyAsync(AsyncPropertyRequest request) = 0;

	/*!
	 * \brief getRangePropertyAsync is used for getting a range of properties that are within the specified time or sequence parameters.
	 * \param request the request containing the property and other information required by the query
	 * \return a pointer to the reply.
	 * \example AsyncRangePropertyRequest vehicleSpeedFromLastWeek;
	 *
	 *	vehicleSpeedFromLastWeek.timeBegin = amb::currentTime() - 10;
	 *	vehicleSpeedFromLastWeek.timeEnd = amb::currentTime();
	 *
	 *	PropertyList requestList;
	 *	requestList.push_back(VehicleProperty::VehicleSpeed);
	 *	requestList.push_back(VehicleProperty::EngineSpeed);
	 *
	 *	vehicleSpeedFromLastWeek.properties = requestList;
	 *	vehicleSpeedFromLastWeek.completed = [](AsyncRangePropertyReply* reply)
	 *	{
	 *		std::list<AbstractPropertyType*> values = reply->values;
	 *		for(auto itr = values.begin(); itr != values.end(); itr++)
	 *		{
	 *			auto val = *itr;
	 *			DebugOut(1)<<"Value from past: ("<<val->name<<"): "<<val->toString()<<" time: "<<val->timestamp<<endl;
	 *		}
	 *
	 *		delete reply;
	 *	};
	 *
	 *	routingEngine->getRangePropertyAsync(vehicleSpeedFromLastWeek);
	 *
	 */
	virtual AsyncRangePropertyReply * getRangePropertyAsync(AsyncRangePropertyRequest request) = 0;

	/*!
	 * \brief setProperty sets a property to a value.
	 * \see AsyncSetPropertyRequest
	 * \see AsyncPropertyReply
	 * \param request the request containing the property and the value to set
	 * \return a pointer to the reply which is owned by the caller of this method
	 * \example 
	 */
	virtual AsyncPropertyReply * setProperty(AsyncSetPropertyRequest request) = 0;

	/*!
	 * \brief subscribeToProperty subscribes to propertyName.  Value changes will be passed to callback.
	 * \param propertyName
	 * \param callback
	 * \param pid process id of the requesting application
	 * \return subscription handle
	 */
	virtual uint subscribeToProperty(const VehicleProperty::Property & propertyName, PropertyChangedType callback, std::string pid="") = 0;

	/*!
	 * \brief unsubscribeToProperty
	 * \param handle
	 */
	virtual void unsubscribeToProperty(uint handle) = 0;

	/*!
	 * \brief subscribeToProperty subscribe to changes made to a property value.
	 * \param propertyName name of the property to request a subscription for.
	 * \param self pointer to the sink who is subscribing.
	 * \example
	 * //somewhere in the sink:
	 * routingEngine->subscribeToProperty(VehicleProperty::EngineSpeed, this);
	 *
	 * //... elsewhere in the sink, this will be called when a property changes:
	 * void MySink::propertyChanged(const AbstractPropertyType* property)
	 * {
	 *   if(property->name == VehicleProperty::EngineSpeed)
	 *   {
	 *      ...
	 *   }
	 * }
	 */
	virtual bool subscribeToProperty(const VehicleProperty::Property & propertyName, AbstractSink* self) = 0;

	/*!
	 * \brief subscribeToProperty subscribe to changes made to a property value.
	 * \param propertyName name of the property to request a subscription for.
	 * \param sourceUuidFilter source UUID to filter.  Only property updates from this source will be sent to the sink.
	 * \param self pointer to the sink who is subscribing.
	 */
	virtual bool subscribeToProperty(const VehicleProperty::Property & propertyName, const std::string & sourceUuidFilter, AbstractSink *self) = 0;

	/*!
	 * \brief subscribeToProperty subscribe to changes made to a property value.
	 * \param propertyName name of the property to request a subscription for.
	 * \param sourceUuidFilter source UUID to filter.  Only property updates from this source will be sent to the sink.
	 * \param zoneFilter zone to filter.  Only updates from this zone will be passed to the sink.
	 * \param self pointer to the sink who is subscribing.
	 */
	virtual bool subscribeToProperty(const VehicleProperty::Property & propertyName, const std::string & sourceUuidFilter, Zone::Type zoneFilter, AbstractSink *self) = 0;

	virtual bool unsubscribeToProperty(const VehicleProperty::Property &, AbstractSink* self) = 0;

	virtual PropertyInfo getPropertyInfo(const VehicleProperty::Property &, const std::string & sourceUuid) = 0;
};

#endif // ABSTRACTROUTINGENGINE_H
