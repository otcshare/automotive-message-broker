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
#include "timestamp.h"

#include <iostream>
#include <boost/assert.hpp>
#include <glib.h>

using namespace std;

#include "debugout.h"

uint16_t accelerationX = 0;
Transmission::TransmissionPositions transmissionShiftPostion = Transmission::Neutral;
uint16_t steeringWheelAngle=0;
uint16_t throttlePos = 0;
uint16_t engineCoolant = 40;
bool machineGun = false;

static gboolean timeoutCallback(gpointer data)
{
	ExampleSourcePlugin* src = (ExampleSourcePlugin*)data;
	
	src->randomizeProperties();
	
	return true;
}

ExampleSourcePlugin::ExampleSourcePlugin(AbstractRoutingEngine* re, map<string, string> config)
:AbstractSource(re, config), velocity(0), engineSpeed(0)
{
	re->setSupported(supported(), this);
	debugOut("setting timeout");
	g_timeout_add(1000, timeoutCallback, this );
	
}



extern "C" AbstractSource * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new ExampleSourcePlugin(routingengine, config);
	
}

string ExampleSourcePlugin::uuid()
{
	return "6dd4268a-c605-4a06-9034-59c1e8344c8e";
}


void ExampleSourcePlugin::getPropertyAsync(AsyncPropertyReply *reply)
{
	DebugOut()<<"ExampleSource: getPropertyAsync called for property: "<<reply->property<<endl;



	if(reply->property == VehicleProperty::VehicleSpeed)
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
	}
	else if(reply->property == VehicleProperty::AccelerationX)
	{
		VehicleProperty::AccelerationType temp(accelerationX);
		reply->value = &temp;
		reply->completed(reply);
	}
	else if(reply->property == VehicleProperty::TransmissionShiftPosition)
	{
		VehicleProperty::TransmissionShiftPositionType temp(transmissionShiftPostion);
		reply->value = &temp;
		reply->completed(reply);
	}
	else if(reply->property == VehicleProperty::SteeringWheelAngle)
	{
		VehicleProperty::SteeringWheelAngleType temp(steeringWheelAngle);
		reply->value = &temp;
		reply->completed(reply);
	}
	else if(reply->property == VehicleProperty::VIN)
	{
		VehicleProperty::VINType temp("ABC00000000000000");
		reply->value = &temp;
		reply->completed(reply);
	}
	else if(reply->property == VehicleProperty::WMI)
	{
		VehicleProperty::WMIType temp("abc");
		reply->value = &temp;
		reply->completed(reply);
	}
	else if(reply->property == VehicleProperty::BatteryVoltage)
	{
		VehicleProperty::BatteryVoltageType temp(12.6);
		reply->value = &temp;
		reply->completed(reply);
	}
}

void ExampleSourcePlugin::getRangePropertyAsync(AsyncRangePropertyReply *reply)
{

}

AsyncPropertyReply *ExampleSourcePlugin::setProperty(AsyncSetPropertyRequest request )
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
	props.push_back(VehicleProperty::ThrottlePosition);
	props.push_back(VehicleProperty::EngineCoolantTemperature);
	props.push_back(VehicleProperty::VIN);
	props.push_back(VehicleProperty::WMI);
	props.push_back(VehicleProperty::BatteryVoltage);
	props.push_back(VehicleProperty::MachineGunTurretStatus);
	
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
	transmissionShiftPostion = Transmission::TransmissionPositions(1 + (6.00 * (rand() / (RAND_MAX + 1.0))));
	steeringWheelAngle = 1 + (359.00 * (rand() / (RAND_MAX + 1.0)));
	throttlePos = 1 + (100.00 * (rand() / (RAND_MAX + 1.0)));
	engineCoolant = 1 + (40.00 * (rand() / (RAND_MAX + 140.0)));
	
	DebugOut()<<"setting velocity to: "<<velocity<<endl;
	DebugOut()<<"setting enginespeed to: "<<engineSpeed<<endl;
	
	VehicleProperty::VehicleSpeedType vel(velocity);
	VehicleProperty::EngineSpeedType es(engineSpeed);
	VehicleProperty::AccelerationType ac(accelerationX);
	VehicleProperty::SteeringWheelAngleType swa(steeringWheelAngle);
	VehicleProperty::TransmissionShiftPositionType tsp(transmissionShiftPostion);
	VehicleProperty::ThrottlePositionType tp(throttlePos);
	VehicleProperty::EngineCoolantTemperatureType ec(engineCoolant);
	VehicleProperty::MachineGunTurretStatusType mgt(machineGun);

	machineGun = !machineGun;

	vel.timestamp = amb::currentTime();
	es.timestamp = amb::currentTime();
	ac.timestamp = amb::currentTime();
	swa.timestamp = amb::currentTime();
	tsp.timestamp = amb::currentTime();
	tp.timestamp = amb::currentTime();
	ec.timestamp = amb::currentTime();

	routingEngine->updateProperty(VehicleProperty::VehicleSpeed, &vel, uuid());
	routingEngine->updateProperty(VehicleProperty::EngineSpeed, &es, uuid());
	routingEngine->updateProperty(VehicleProperty::AccelerationX, &ac, uuid());
	routingEngine->updateProperty(VehicleProperty::SteeringWheelAngle, &swa, uuid());
	routingEngine->updateProperty(VehicleProperty::TransmissionShiftPosition,&tsp, uuid());
	routingEngine->updateProperty(VehicleProperty::ThrottlePosition, &tp, uuid());
	routingEngine->updateProperty(VehicleProperty::EngineCoolantTemperature, &ec, uuid());
	//routingEngine->updateProperty(VehicleProperty::MachineGunTurretStatus, &mgt);
}
