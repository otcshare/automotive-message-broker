
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


#ifndef MURPHYSOURCE_H
#define MURPHYSOURCE_H

#include <murphy/common.h>

#include <abstractsource.h>
#include <string>

// #include <vehicleproperty.h>
// #include <abstractpropertytype.h>

using namespace std;

class MurphySource : public AbstractSource
{

public:
    MurphySource(AbstractRoutingEngine* re, map<string, string> config);
    ~MurphySource();

    string uuid() { return "murphy"; };
    void getPropertyAsync(AsyncPropertyReply *reply);
    void getRangePropertyAsync(AsyncRangePropertyReply *reply);
    AsyncPropertyReply * setProperty(AsyncSetPropertyRequest request);
    void subscribeToPropertyChanges(VehicleProperty::Property property);
    void unsubscribeToPropertyChanges(VehicleProperty::Property property);
    PropertyList supported();

    int supportedOperations();

    void propertyChanged(VehicleProperty::Property property, AbstractPropertyType* value, string uuid) {}
    void supportedChanged(PropertyList) {}

    void processValue(string propertyName, AbstractPropertyType *value);

    bool hasProperty(string propertyName);

private:
    void checkSubscriptions();
    void setConfiguration(map<string, string> config);
    int connectToMurphy(mrp_mainloop_t *ml, const char *address);

    PropertyList m_supportedProperties;
    bool m_clientConnected;

    MurphySource *m_source;
    AbstractRoutingEngine *m_re;

    // known properties

    map<string, AbstractPropertyType *> murphyProperties;

    // murphy integration

    mrp_mainloop_t *m_ml;
    mrp_transport_t *m_tport;
    string m_address; // transport address
};

#endif // MURPHYSOURCE_H
