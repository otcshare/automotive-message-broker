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

#ifndef TPMSPLUGIN_H
#define TPMSPLUGIN_H

#include <abstractsource.h>
#include <string>

using namespace std;

class TpmsPlugin: public AbstractSource
{

public:
	TpmsPlugin(AbstractRoutingEngine* re);

    string uuid();
	PropertyList supported();
	void subscribeToPropertyChanges(VehicleProperty::Property property);
	void unsubscribeToPropertyChanges(VehicleProperty::Property property);
	
	void getPropertyAsync(AsyncPropertyReply *reply);
    void setProperty(VehicleProperty::Property, AbstractPropertyType*);

	void propertyChanged(VehicleProperty::Property property, AbstractPropertyType* value, string uuid) {}
	void supportedChanged(PropertyList) {}
	
	
private:
	PropertyList mRequests;
    float leftFrontPressure, rightFrontPressure, leftRearPressure, rightRearPressure;
    float leftFrontTemperature, rightFrontTemperature, leftRearTemperature, rightRearTemperature;
};

#endif // TPMSPLUGIN_H
