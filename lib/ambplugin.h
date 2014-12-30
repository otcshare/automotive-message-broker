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

#ifndef _AMBPLUGIN_H_
#define _AMBPLUGIN_H_

#include <abstractsource.h>
#include "ambpluginimpl.h"
#include <string>

/*!
 * \file ambplugin.h
 * \brief Contains common code used in all IviPoC II plugins for Automotive message broker(AMB).
 */

/*!
 * AmbPlugin class contains common code used in all IviPoC II plugins for Automotive message broker(AMB).
 * For the AMB library API please visit <a href="https://github.com/otcshare/automotive-message-broker">Automotive message broker web page</a>.
 *
 * \class AmbPlugin
 *
 * Example of the minimal code to write a new plugin using AmbPlugin:
 *
 * \code
 * #include "ambpluginimpl.h"
 *
 * class MyPlugin: public AmbPluginImpl {
 *
 * public:
 *     MyPlugin(AbstractRoutingEngine* re, const std::map<std::string, std::string>& config, AbstractSink& parent);
 *     ~MyPlugin();
 *
 *     const std::string uuid() const { return "plugin_uuid"; }
 * };
 *
 * // library exported function for plugin loader
 * extern "C" void create(AbstractRoutingEngine* routingengine, std::map<std::string, std::string> config)
 * {
 *     new AmbPlugin<MyPlugin>(routingengine, config);
 * }
 * \endcode
 *
 *  @{
 */

template<class T>
class AmbPlugin : public AbstractSource {

	/*!
	 * Compile time check
	 * \internal
	 */
	static_assert(std::is_base_of<AmbPluginImpl, T>::value, "AmbPluginImpl has to be a base of T");

public:
	/*!
	 * \param re AbstractRoutingEngine
	 * \param config Map of the configuration string values loaded on startup from AMB configuration file
	 */
	AmbPlugin(AbstractRoutingEngine* re, const std::map<std::string, std::string>& config);
	virtual ~AmbPlugin() {}

	// from AbstractSource:
public:

	/*!
	 * \brief getPropertyAsync is called when a sink requests the value for given property.
	 * This is only called if the source supports the Get operation.
	 * \param reply the reply variable.  @see AsyncPropertyReply
	 */
	virtual void getPropertyAsync(AsyncPropertyReply *reply);

	/*!
	 * \brief getRangePropertyAsync is called when a sink requests a series of values for a given
	 * property within a specified time or sequencial range.  This will only be called if the source
	 * support the Ranged Operation.
	 * \param reply is the reply variable.  @see AsyncRangePropertyReply
	 */
	virtual void getRangePropertyAsync(AsyncRangePropertyReply *reply);

	/*!
	 * \brief setProperty is called when a sink requests to set a value for a given property.
	 * This is only called if the source supports the Set Operation.
	 * \param request the requested property to set.
	 * \return returns a pointer to the new value for the property.
	 */
	virtual AsyncPropertyReply * setProperty(AsyncSetPropertyRequest request);

	/*!
	 * \brief subscribeToPropertyChanges is called when a sink requests a subscription. Source plugins
	 * can keep track of subscriptions and may wish to sleep if there are no subscriptions.
	 * \param property the property that is being subscribed.
	 */
	virtual void subscribeToPropertyChanges(VehicleProperty::Property property);

	/*!
	 * \brief unsubscribeToPropertyChanges is called when a sink requests to unsubscribe from a given property's changes.
	 * \param property the property to unsubscribe to
	 */
	virtual void unsubscribeToPropertyChanges(VehicleProperty::Property property);

	/*!
	 * \brief supported is called by the routingEngine to understand what properties this source supports
	 * \return returns a list of supported properties. If the the supported properties changed, the source should call AbstractRoutingEngine::setSupported.
	 */
	virtual PropertyList supported();

	/*!
	 * \brief supportedOperations
	 * \return returns the supported operations.
	 */
	virtual int supportedOperations();

	/*!
	 * \brief getPropertyInfo used to return specific information about a property.
	 * The source should override this otherwise a PropertyInfo::invalid() will be returned for the property.
	 * \param property the property to get info for.
	 * \return a PropertyInfo object.
	 */
	virtual PropertyInfo getPropertyInfo(const VehicleProperty::Property & property);

	// from AbstractSink
public:

	/*! uuid() is a unique identifier
	  * \return A guid-style unique identifier
	  */
	virtual const string uuid();

	/*! propertyChanged is called when a subscribed to property changes.
	  * \param value value of the property that changed. this is a temporary pointer that will be destroyed.
	  * Do not destroy it.  If you need to store the value use value.anyValue(), value.value<T>() or
	  * value->copy() to copy.
	  */
	virtual void propertyChanged(AbstractPropertyType* value);

	/*! supportedChanged() is called when the supported properties changes
	 * \param supportedProperties the new list of supported properties.
	 */
	virtual void supportedChanged(const PropertyList & supportedProperties);


	// AmbPlugin's own methods
public:

	/*!
	 * Second phase of the plugin initialization.
	 * \fn init
	 */
	void init();

private:

	/**
	* \brief AmbPlugin class private implementation
	*/
	std::unique_ptr<T> d;
};

//----------------------------------------------------------------------------
// Function implementations
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// AmbPlugin
//----------------------------------------------------------------------------

template<typename T>
AmbPlugin<T>::AmbPlugin(AbstractRoutingEngine* re, const std::map<std::string, std::string>& config) :
	AbstractSource(re, config),
	d(new T(re, config, *this))
{

}

template<typename T>
void AmbPlugin<T>::getPropertyAsync(AsyncPropertyReply *reply)
{
	if(d)
		d->getPropertyAsync(reply);
}

template<typename T>
void AmbPlugin<T>::getRangePropertyAsync(AsyncRangePropertyReply *reply)
{
	if(d)
		d->getRangePropertyAsync(reply);
}

template<typename T>
AsyncPropertyReply* AmbPlugin<T>::setProperty(AsyncSetPropertyRequest request)
{
	if(d)
		return d->setProperty(request);
	return nullptr;
}

template<typename T>
void AmbPlugin<T>::subscribeToPropertyChanges(VehicleProperty::Property property)
{
	if(d)
		d->subscribeToPropertyChanges(property);
}

template<typename T>
void AmbPlugin<T>::unsubscribeToPropertyChanges(VehicleProperty::Property property)
{
	if(d)
		return d->unsubscribeToPropertyChanges(property);
}

template<typename T>
PropertyList AmbPlugin<T>::supported()
{
	return d ? d->supported() : PropertyList();
}

template<typename T>
int AmbPlugin<T>::supportedOperations()
{
	return d ? d->supportedOperations() : 0;
}

template<typename T>
PropertyInfo AmbPlugin<T>::getPropertyInfo(const VehicleProperty::Property &property)
{
	return d ? d->getPropertyInfo(property) : PropertyInfo::invalid();
}

template<typename T>
const string AmbPlugin<T>::uuid()
{
	return d ? d->uuid() : "";
}

template<typename T>
void AmbPlugin<T>::propertyChanged(AbstractPropertyType* value)
{
	if(d)
		d->propertyChanged(value);
}

template<typename T>
void AmbPlugin<T>::supportedChanged(const PropertyList &supportedProperties)
{
	if(d)
		d->supportedChanged(supportedProperties);
}

template<typename T>
void AmbPlugin<T>::init()
{
	if(d)
		d->init();
}

#endif // _AMBPLUGIN_H_

/** @} */
