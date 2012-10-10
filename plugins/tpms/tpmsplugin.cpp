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

#include "tpmsplugin.h"

#include <iostream>
#include <boost/assert.hpp>
#include <glib.h>

using namespace std;

#include "debugout.h"


static gboolean timeoutCallback(gpointer data)
{
	TpmsPlugin* src = (TpmsPlugin*)data;
	
	return true;
}


string TpmsPlugin::uuid()
{
  return "something TPMS";
}


TpmsPlugin::TpmsPlugin(AbstractRoutingEngine* re)
:AbstractSource(re)
{
	re->setSupported(supported(), this);
	debugOut("setting timeout");
	g_timeout_add(1000, timeoutCallback, this );
	
    leftFrontPressure = rightFrontPressure = leftRearPressure = rightRearPressure = 0;
    leftFrontTemperature = rightFrontTemperature = leftRearTemperature = rightRearTemperature = 0;
}



extern "C" AbstractSource * create(AbstractRoutingEngine* routingengine)
{
	return new TpmsPlugin(routingengine);	
}



void TpmsPlugin::getPropertyAsync(AsyncPropertyReply *reply)
{
	DebugOut()<<"TpmsPlugin: getPropertyAsync called for property: "<<reply->property<<endl;
    /*	if(reply->property == VehicleProperty::VehicleSpeed)
	{
		VehicleProperty::VehicleSpeedType temp(velocity);
		reply->value = &temp;
		reply->completed(reply);
	}
	else if(reply->property == VehicleProperty::EngineSpeed)
	{
		VehicleProperty::EngineSpeedType temp(engineSpeed);
		reply->value = &temp;
		reply->completed(reply);
        }*/
}


void TpmsPlugin::setProperty(VehicleProperty::Property, AbstractPropertyType *)
{

}


PropertyList TpmsPlugin::supported()
{
	PropertyList props;
	props.push_back(VehicleProperty::TirePressureLeftFront);
	props.push_back(VehicleProperty::TirePressureRightFront);
	props.push_back(VehicleProperty::TirePressureLeftRear);
	props.push_back(VehicleProperty::TirePressureRightRear);
	
	return props;
}

void TpmsPlugin::subscribeToPropertyChanges(VehicleProperty::Property property)
{
	mRequests.push_back(property);
}

void TpmsPlugin::unsubscribeToPropertyChanges(VehicleProperty::Property property)
{
	mRequests.remove(property);
}

