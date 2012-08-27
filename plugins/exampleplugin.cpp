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

#include "exampleplugin.h"

#include <iostream>
#include <boost/assert.hpp>
#include <glib.h>

using namespace std;

#include "debugout.h"

uint16_t accelerationX = 0;
uint16_t transmissionShiftPostion = 0;
uint16_t steeringWheelAngle=0;

static gboolean timeoutCallback(gpointer data)
{
	ExampleSourcePlugin* src = (ExampleSourcePlugin*)data;
	
	src->randomizeProperties();
	
	return true;
}

ExampleSourcePlugin::ExampleSourcePlugin(AbstractRoutingEngine* re)
:AbstractSource(re), velocity(0), engineSpeed(0)
{
	re->setSupported(supported(), this);
	
	debugOut("setting timeout");
	g_timeout_add(1000, timeoutCallback, this );
	
}



extern "C" AbstractSource * create(AbstractRoutingEngine* routingengine)
{
	return new ExampleSourcePlugin(routingengine);
	
}

string ExampleSourcePlugin::uuid()
{
	return "6dd4268a-c605-4a06-9034-59c1e8344c8e";
}

boost::any ExampleSourcePlugin::getProperty(VehicleProperty::Property property)
{
	if(property == VehicleProperty::VehicleSpeed)
	{
		return velocity;
	}
	else if(property == VehicleProperty::EngineSpeed)
	{
		return engineSpeed;
	}
}

void ExampleSourcePlugin::getPropertyAsync(AsyncPropertyReply *reply)
{
	if(reply->property == VehicleProperty::VehicleSpeed)
	{
		reply->value = velocity;
		reply->completed(reply);
	}
	else if(reply->property == VehicleProperty::EngineSpeed)
	{
		reply->value = engineSpeed;
		reply->completed(reply);
	}
	else if(reply->property == VehicleProperty::AccelerationX)
	{
		reply->value = accelerationX;
		reply->completed(reply);
	}
	else if(reply->property == VehicleProperty::TransmissionShiftPosition)
	{
		reply->value = transmissionShiftPostion;
		reply->completed(reply);
	}
	else if(reply->property == VehicleProperty::SteeringWheelAngle)
	{
		reply->value = steeringWheelAngle;
		reply->completed(reply);
	}
}

void ExampleSourcePlugin::setProperty(VehicleProperty::Property , boost::any )
{

}

void ExampleSourcePlugin::subscribeToPropertyChanges(VehicleProperty::Property property)
{
	mRequests.push_back(property);
}

PropertyList ExampleSourcePlugin::supported()
{
	PropertyList props;
	props.push_back(VehicleProperty::EngineSpeed);
	props.push_back(VehicleProperty::VehicleSpeed);
	props.push_back(VehicleProperty::AccelerationX);
	props.push_back(VehicleProperty::TransmissionShiftPosition);
	props.push_back(VehicleProperty::SteeringWheelAngle);
	
	return props;
}

void ExampleSourcePlugin::unsubscribeToPropertyChanges(VehicleProperty::Property property)
{
	mRequests.remove(property);
}

void ExampleSourcePlugin::randomizeProperties()
{
	velocity = 1 + (255.00 * (rand() / (RAND_MAX + 1.0)));
	engineSpeed = 1 + (15000.00 * (rand() / (RAND_MAX + 1.0)));
	accelerationX = 1 + (15000.00 * (rand() / (RAND_MAX + 1.0)));
	transmissionShiftPostion = 1 + (6.00 * (rand() / (RAND_MAX + 1.0)));
	steeringWheelAngle = 1 + (359.00 * (rand() / (RAND_MAX + 1.0)));
	
	DebugOut()<<"setting velocity to: "<<velocity<<endl;
	DebugOut()<<"setting enginespeed to: "<<engineSpeed<<endl;
	
	routingEngine->updateProperty(VehicleProperty::VehicleSpeed, velocity);
	routingEngine->updateProperty(VehicleProperty::EngineSpeed, engineSpeed);
	routingEngine->updateProperty(VehicleProperty::AccelerationX, accelerationX);
	routingEngine->updateProperty(VehicleProperty::SteeringWheelAngle, steeringWheelAngle);
	routingEngine->updateProperty(VehicleProperty::TransmissionShiftPosition, transmissionShiftPostion);
}
