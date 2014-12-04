/*
Copyright (C) 2012 Intel Corporation

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

#ifndef _AMBPLUGINIMPL_H_
#define _AMBPLUGINIMPL_H_

#include "abstractsource.h"

/*! \addtogroup ivipocbase
 *  @{
 */

/*!
 * \brief AmbPlugin private class implementation - base class for all plugin implementations.
 *
 * Contains common code used in all IviPoC II plugins for Automotive message broker(AMB).
 * For the AMB library API please visit <a href="https://github.com/otcshare/automotive-message-broker">Automotive message broker web page</a>.
 *
 * \class AmbPluginImpl
 */
class AmbPluginImpl
{

public:
	/*!
	 * \param re AbstractRoutingEngine
	 * \param config Map of the configuration string values loaded on startup from AMB configuration file
	 * \param parent AmbPlugin instance
	 */
	AmbPluginImpl(AbstractRoutingEngine* re, const map<string, string>& config, AbstractSource &parent);
	virtual ~AmbPluginImpl() { } /*LCOV_EXCL_LINE*/

	//  aka AbstractSource:
public:

	/*!
	 * \brief getPropertyAsync is called when a sink requests the value for given property.
	 * This is only called if the source supports the Get operation.
	 * \param reply the reply variable.
	 */
	virtual void getPropertyAsync(AsyncPropertyReply *reply);

	/*!
	 * \brief getRangePropertyAsync is called when a sink requests a series of values for a given
	 * property within a specified time or sequencial range.  This will only be called if the source
	 * support the Ranged Operation.
	 * \param reply is the reply variable.
	 */
	virtual void getRangePropertyAsync(AsyncRangePropertyReply *reply);

	/*!
	 * \brief setProperty is called when a sink requests to set a value for a given property.
	 * This is only called if the source supports the Set Operation.
	 * \param request the requested property to set.
	 * \return returns a pointer to the new value for the property.
	 */
	virtual AsyncPropertyReply *setProperty(const AsyncSetPropertyRequest& request );

	/*!
	 * \brief subscribeToPropertyChanges is called when a sink requests a subscription.  Source plugins
	 * can keep track of subscriptions and may wish to sleep if there are no subscriptions.
	 * \param property the property that is being subscribed.
	 */
	virtual void subscribeToPropertyChanges(const VehicleProperty::Property& property);

	/*!
	 * \brief unsubscribeToPropertyChanges is called when a sink requests to unsubscribe from a given property's changes.
	 * \param property the property to unsubscribe to
	 */
	virtual void unsubscribeToPropertyChanges(const VehicleProperty::Property& property);

	/*!
	 * \brief supported is called by the routingEngine to understand what properties this source supports.
	 * \return returns a list of supported properties.  If the the supported properties changed, the source should call AbstractRoutingEngine::setSupported.
	 */
	virtual PropertyList supported() const;

	/*!
	 * \brief supportedOperations
	 * \return returns the supported operations.
	 */
	virtual int supportedOperations() const;

	/*!
	 * \brief getPropertyInfo used to return specific information about a property.
	 * The source should override this otherwise a PropertyInfo::invalid() will be returned for the property
	 * \param property the property to get info for.
	 * \return a PropertyInfo object.
	 */
	virtual PropertyInfo getPropertyInfo(const VehicleProperty::Property & property);

	// aka AbstractSink:
public:

	/*! uuid() is a unique identifier
	  * @return a guid-style unique identifier
	  */
	virtual const std::string uuid() const = 0;

	/*! propertyChanged is called when a subscribed to property changes.
	  * @param value value of the property that changed. this is a temporary pointer that will be destroyed.
	  * Do not destroy it.  If you need to store the value use value.anyValue(), value.value<T>() or
	  * value->copy() to copy.
	  */
	virtual void propertyChanged(AbstractPropertyType* value);

	/*! supportedChanged() is called when the supported properties changes
	 * \fn supportedChanged
	 * \param supportedProperties the new list of supported properties.
	 */
	virtual void supportedChanged(const PropertyList& supportedProperties);

	/*!
	 * Second phase of the plugin initialization.
	 * \fn init
	 */
	virtual void init();

protected:

	/*! Finds property type in #properties
	 * \param propertyName Name of the property to be found.
	 * \param zone Zone of the property to be found.
	 * \return AbstractPropertyType* if signal exits otherwise nullptr(in this case we do not know its datatype)
	 */
	virtual AbstractPropertyType* findPropertyType(const VehicleProperty::Property& propertyName, const Zone::Type& zone);

	/*! Registers property in AMB
	 * \param zone Zone of the property to be registered.
	 * \param typeFactory Function to be used to create instance of the AbstractPropertyType for registered property
	 * \return AbstractPropertyType* if signal exits otherwise nullptr(in this case we do not know its datatype)
	 */
	virtual std::shared_ptr<AbstractPropertyType> addPropertySupport(Zone::Type zone, std::function<AbstractPropertyType* (void)> typeFactory);

	//
	// data:
	//

	/*! AmbPlugin instance
	 * \property parent
	 */
	AbstractSource& source;

	/*!
	 * AbstractRoutingEngine instance
	 * \property routingEngine
	 */
	AbstractRoutingEngine* routingEngine;

	/*! Helper typedef
	 * \internal
	 */
	typedef std::map< Zone::Type, std::shared_ptr<AbstractPropertyType> > ZonePropertyType;

	/*!
	 * Supported property values map
	 * \property properties
	 */
	std::map< VehicleProperty::Property, ZonePropertyType > properties;

	/*!
	  * configuration
	  */
	std::map< std::string, std::string> configuration;
};

#endif // _AMBPLUGINIMPL_H_

/** @} */
