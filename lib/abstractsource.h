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


#ifndef ABSTRACTSOURCE_H
#define ABSTRACTSOURCE_H

#include <string>
#include <list>
#include <boost/any.hpp>

#include "abstractsink.h"
#include "vehicleproperty.h"
#include "abstractroutingengine.h"
#include "abstractpropertytype.h"
#include "propertyinfo.hpp"



class AbstractSource;

typedef std::list<AbstractSource*> SourceList;



class AbstractSource: public AbstractSink
{

public:
	/*!
	 * \brief The Operations enum is a bitmask flag used to specify which operations are supported by the source plugin
	 */
	enum Operations {
		Get = 0x01,
		Set = 0x02,
		GetRanged = 0x04
	};

	AbstractSource(AbstractRoutingEngine* engine, map<string, string> config);
	virtual ~AbstractSource();
	
	///pure virtual methods:

	/*!
	 * \brief getPropertyAsync is called when a sink requests the value for given property.
	 * This is only called if the source supports the Get operation (@see Operation)
	 * \param reply the reply variable.  @see AsyncPropertyReply
	 */
	virtual void getPropertyAsync(AsyncPropertyReply *reply) = 0;

	/*!
	 * \brief getRangePropertyAsync is called when a sink requests a series of values for a given
	 * property within a specified time or sequencial range.  This will only be called if the source
	 * support the Ranged Operation (@see Operations)
	 * \param reply is the reply variable.  @see AsyncRangePropertyReply
	 */
	virtual void getRangePropertyAsync(AsyncRangePropertyReply *reply) = 0;

	/*!
	 * \brief setProperty is called when a sink requests to set a value for a given property.
	 * This is only called if the source supports the Set Operation (@see Operation)
	 * \param request the requested property to set.
	 * \return returns a pointer to the new value for the property.  @see AsyncPropertyReply
	 */
	virtual AsyncPropertyReply * setProperty(AsyncSetPropertyRequest request) = 0;

	/*!
	 * \brief subscribeToPropertyChanges is called when a sink requests a subscription.  Source plugins
	 * can keep track of subscriptions and may wish to sleep if there are no subscriptions.
	 * \param property the property that is being subscribed.
	 * @see unsubscribeToPropertyChanges
	 */
	virtual void subscribeToPropertyChanges(VehicleProperty::Property property) = 0;

	/*!
	 * \brief unsubscribeToPropertyChanges is called when a sink requests to unsubscribe from a given property's changes.
	 * \param property the property to unsubscribe to
	 * @see subscribeToPropertyChanges
	 */
	virtual void unsubscribeToPropertyChanges(VehicleProperty::Property property) = 0;

	/*!
	 * \brief supportedOperations
	 * \return returns the supported operations.  @see Operations
	 */
	virtual int supportedOperations() = 0;

	/*!
	 * \brief getPropertyInfo used to return specific information about a property @see PropertyInfo
	 * the source should override this otherwise a PropertyInfo::invalid() will be returned for the property
	 * \param property the property to get info for.
	 * \return a PropertyInfo object.
	 */
	virtual PropertyInfo getPropertyInfo(const VehicleProperty::Property & property);
	
	/*!
	 * \brief supported
	 * \return returns the supported properties.
	 */
	virtual PropertyList supported() = 0;

protected:
	/*!
	 * \brief routingEngine the core routing engine used to send property updates to sink plugins.
	 * @see AbstractRoutingEngine
	 */
	AbstractRoutingEngine* routingEngine;
	
private:
	AbstractSource():AbstractSink(nullptr, std::map<std::string,std::string>()) { }
};

#endif // ABSTRACTSOURCE_H
