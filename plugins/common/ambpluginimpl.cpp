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

#include <vehicleproperty.h>
#include <listplusplus.h>

#include "ambpluginimpl.h"

//----------------------------------------------------------------------------
// AmbPluginImpl
//----------------------------------------------------------------------------

AmbPluginImpl::AmbPluginImpl(AbstractRoutingEngine* re, const map<string, string>& config, AbstractSource& parent) :
    parent(parent),
    routingEngine(re)
{
}

void AmbPluginImpl::init()
{
}

void AmbPluginImpl::getPropertyAsync(AsyncPropertyReply *reply)
{
    if(!reply) {
        DebugOut(DebugOut::Error) << "AmbPluginImpl::getPropertyAsync - reply is null" << std::endl;
        return;
    }

    DebugOut() << "AmbPluginImpl::getPropertyAsync called for property: " << reply->property<<endl;

    reply->success = false;
    reply->error = AsyncPropertyReply::InvalidOperation;
    AbstractPropertyType *value = findPropertyType(reply->property, reply->zoneFilter);
    if (value) {
        reply->value = value;
        reply->success = true;
        reply->error = AsyncPropertyReply::NoError;
    }

    if(reply->completed)
        reply->completed(reply);
}

void AmbPluginImpl::getRangePropertyAsync(AsyncRangePropertyReply *reply)
{
    if(!reply) {
        DebugOut(DebugOut::Error) << "AmbPluginImpl::getRangePropertyAsync - reply is null" << std::endl;
        return;
    }

    DebugOut() << "AmbPluginImpl::getRangePropertyAsync not supported "<< std::endl;
    reply->success = false;
    reply->error = AsyncPropertyReply::InvalidOperation;
    if(reply->completed)
        reply->completed(reply);
}

AsyncPropertyReply *AmbPluginImpl::setProperty(const AsyncSetPropertyRequest& request )
{
    AsyncPropertyReply* reply = new AsyncPropertyReply(request);
    reply->success = false;
    reply->error = AsyncPropertyReply::InvalidOperation;

    AbstractPropertyType *value = findPropertyType(request.property, request.zoneFilter);
    if (value && request.value) {
        DebugOut(2) << "updating property "<< request.property << " to: " << request.value->toString() << endl;
        value->fromString(request.value->toString());
        DebugOut(2) << "New value of property "<< request.property << " is: " << value->toString() << endl;
        value->timestamp = amb::currentTime();
        routingEngine->updateProperty(value, uuid());

        reply->success = true;
        reply->error = AsyncPropertyReply::NoError;
    }

    try {
        if(reply->completed)
            reply->completed(reply);
    }
    catch (...) { }

    return reply;
}

void AmbPluginImpl::subscribeToPropertyChanges(const VehicleProperty::Property& property)
{
    std::list<Zone::Type> zones = getPropertyInfo(property).zones();
    for( auto it=zones.begin(); it!=zones.end(); ++it) {
        AbstractPropertyType *value = findPropertyType(property, *it);
        if(value)
            routingEngine->updateProperty(value, uuid());
    }
}

PropertyList AmbPluginImpl::supported() const
{
    PropertyList props;
    for(auto itPropMap = properties.begin(); itPropMap != properties.end(); ++itPropMap)
        props.push_back(itPropMap->first);
    return props;
}

int AmbPluginImpl::supportedOperations() const
{
    return AbstractSource::Get /*| AbstractSource::Set*/;
}

void AmbPluginImpl::unsubscribeToPropertyChanges(const VehicleProperty::Property& property)
{
}

// if signal does not exits return nullptr(we do not know its datatype), if zone does not exists, add it
AbstractPropertyType* AmbPluginImpl::findPropertyType(const VehicleProperty::Property& propertyName, const Zone::Type& zone)
{
    auto itPropMap = properties.find(propertyName);
    if(itPropMap == properties.end())
        return nullptr;

    for( auto it = itPropMap->second.begin(); it != itPropMap->second.end(); ++it ) {
        if(it->first == zone)
            return it->second.get();
    }

    return nullptr;
}

std::shared_ptr<AbstractPropertyType> AmbPluginImpl::addPropertySupport(Zone::Type zone, std::function<AbstractPropertyType* (void)> typeFactory)
{
    std::shared_ptr<AbstractPropertyType> propertyType(typeFactory());
    if(!propertyType)
        return propertyType;

    VehicleProperty::Property name(propertyType->name);
    PropertyList registeredProperties(VehicleProperty::capabilities());
    bool registeredType(false);
    if(!ListPlusPlus<VehicleProperty::Property>(&registeredProperties).contains(name))
    {
        registeredType = VehicleProperty::registerProperty(name, typeFactory);
    }
    if(!registeredType){ // Property type wasn't registered by us. Is it predefined in AMB API or some other source plug-in has already registered it ???
        PropertyList supportedProperties(routingEngine->supported());// Is this property already supported by any other source plug-in ?
        if(ListPlusPlus<VehicleProperty::Property>(&supportedProperties).contains(name)) {
            return nullptr;// Some other source plug-in supports this property, we can't support it - AMB doesn't support more sources for one property :-)
        }
        std::shared_ptr<AbstractPropertyType> registeredPropertyType(VehicleProperty::getPropertyTypeForPropertyNameValue(name));
        if(!registeredPropertyType)
            return nullptr;
        propertyType.swap(registeredPropertyType);
    }
    propertyType->zone = zone;
    propertyType->sourceUuid = uuid();
    ZonePropertyType& zonePropType = properties[name];
    zonePropType.insert( make_pair(zone, propertyType));
    return propertyType;
}

PropertyInfo AmbPluginImpl::getPropertyInfo(const VehicleProperty::Property& property) const
{
    auto it = properties.find(property);
    if(it != properties.end()) {
        std::list<Zone::Type> zones;
        for(auto itZonePropType = it->second.begin(); itZonePropType != it->second.end(); ++itZonePropType)
            zones.push_back(itZonePropType->first);

        return PropertyInfo( 0, zones );
    }

    return PropertyInfo::invalid();
}

void AmbPluginImpl::propertyChanged(AbstractPropertyType* value)
{
}

void AmbPluginImpl::supportedChanged(const PropertyList& supportedProperties)
{
}
