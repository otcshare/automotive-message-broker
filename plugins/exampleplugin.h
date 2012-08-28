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

#ifndef EXAMPLEPLUGIN_H
#define EXAMPLEPLUGIN_H

#include <abstractsource.h>
#include <string>

using namespace std;

class ExampleSourcePlugin: public AbstractSource
{

public:
	ExampleSourcePlugin(AbstractRoutingEngine* re);
	
	string uuid();
	void getPropertyAsync(AsyncPropertyReply *reply);
	void setProperty(VehicleProperty::Property, AbstractPropertyType);
	void subscribeToPropertyChanges(VehicleProperty::Property property);
	void unsubscribeToPropertyChanges(VehicleProperty::Property property);
	PropertyList supported();
	
	void propertyChanged(VehicleProperty::Property property, AbstractPropertyType value, string uuid) {}
	void supportedChanged(PropertyList) {}
	
	void randomizeProperties();
	
private:
	PropertyList mRequests;
	uint16_t velocity;
	uint16_t engineSpeed;
};

#endif // EXAMPLEPLUGIN_H
