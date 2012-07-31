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


#ifndef ABSTRACTSINK_H
#define ABSTRACTSINK_H

#include <string>
#include <list>
#include <functional>

#include "vehicleproperty.h"

using namespace std;

typedef function<void (VehicleProperty::Property, boost::any)> SetPropertySignal;
typedef function<void (VehicleProperty::Property)> SubscriptionSignal;

typedef list<AbstractSink*> SinkList;
typedef function<void (AbstractSink*)> SinkSignal;

class AbstractSink
{

public:
	AbstractSink();
	
	void setProperty(VehicleProperty::Property, boost::any);
	void subscribeToProperty(VehicleProperty::Property property);
	void unsubscribeToProperty(VehicleProperty::Property property);
	PropertyList supported(); 
	
	void setSupported(PropertyList properties);
	
	///Pure virtual methods:
	
	virtual string name() = 0;
	virtual void propertyChanged(VehicleProperty::Property property, boost::any value) = 0;
	virtual PropertyList subscriptions() = 0;
	
	
private:
	SetPropertySignal setPropertyCb;
	SubscriptionSignal subscribeToPropertyCb;
	SubscriptionSignal unsubscribeToPropertyCb;
	PropertyList mSupported;
};

class AbstractSinkManager
{
public:
	virtual SinkList sinks() = 0;
	
	void sinkCreated(AbstractSink*);
	void sinkRemoved(AbstractSink*);
	
private:
	SinkSignal sinkCreatedCb;
	SinkSignal sinkRemovedCb;
};

#endif // ABSTRACTSINK_H
