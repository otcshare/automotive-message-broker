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


#include "examplesink.h"
#include "abstractroutingengine.h"
#include "debugout.h"

#include <glib.h>


extern "C" AbstractSinkManager * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new ExampleSinkManager(routingengine, config);
}

ExampleSink::ExampleSink(AbstractRoutingEngine* engine, map<string, string> config): AbstractSink(engine, config)
{
	routingEngine->subscribeToProperty(VehicleProperty::EngineSpeed, this);
	routingEngine->subscribeToProperty(VehicleProperty::VehicleSpeed, this);

	supportedChanged(routingEngine->supported());

}


PropertyList ExampleSink::subscriptions()
{

}

void ExampleSink::supportedChanged(PropertyList supportedProperties)
{
	printf("Support changed!\n");
	routingEngine->subscribeToProperty(VehicleProperty::EngineSpeed, this);
	routingEngine->subscribeToProperty(VehicleProperty::VehicleSpeed, this);
	routingEngine->subscribeToProperty(VehicleProperty::Latitude, this);
	routingEngine->subscribeToProperty(VehicleProperty::Longitude, this);

	AsyncPropertyRequest velocityRequest;
	velocityRequest.property = VehicleProperty::VehicleSpeed;
	velocityRequest.completed = [](AsyncPropertyReply* reply)
	{
		if(!reply->success)
			DebugOut(0)<<"Velocity Async request failed";
		else
			DebugOut(0)<<"Velocity Async request completed: "<<reply->value->toString()<<endl;
		delete reply;
	};

	routingEngine->getPropertyAsync(velocityRequest);

	AsyncPropertyRequest vinRequest;
	vinRequest.property = VehicleProperty::VIN;
	vinRequest.completed = [](AsyncPropertyReply* reply)
	{
		if(!reply->success)
			DebugOut(0)<<"VIN Async request failed";
		else
			DebugOut(0)<<"VIN Async request completed: "<<reply->value->toString()<<endl;
		delete reply;
	};

	routingEngine->getPropertyAsync(vinRequest);

	AsyncPropertyRequest wmiRequest;
	wmiRequest.property = VehicleProperty::WMI;
	wmiRequest.completed = [](AsyncPropertyReply* reply)
	{
		if(!reply->success)
			DebugOut(0)<<"WMI Async request failed";
		else
		DebugOut(1)<<"WMI Async request completed: "<<reply->value->toString()<<endl;
		delete reply;
	};

	routingEngine->getPropertyAsync(wmiRequest);

	AsyncPropertyRequest batteryVoltageRequest;
	batteryVoltageRequest.property = VehicleProperty::BatteryVoltage;
	batteryVoltageRequest.completed = [](AsyncPropertyReply* reply)
	{
		if(!reply->success)
			DebugOut(0)<<"BatteryVoltage Async request failed";
		else
			DebugOut(1)<<"BatteryVoltage Async request completed: "<<reply->value->toString()<<endl;
		delete reply;
	};

	routingEngine->getPropertyAsync(batteryVoltageRequest);

	AsyncPropertyRequest doorsPerRowRequest;
	doorsPerRowRequest.property = VehicleProperty::DoorsPerRow;
	doorsPerRowRequest.completed = [](AsyncPropertyReply* reply)
	{
		if(!reply->success)
			DebugOut(0)<<"Doors per row Async request failed";
		else
			DebugOut(1)<<"Doors per row: "<<reply->value->toString()<<endl;
		delete reply;
	};

	routingEngine->getPropertyAsync(doorsPerRowRequest);

	AsyncPropertyRequest airbagStatus;
	airbagStatus.property = VehicleProperty::AirbagStatus;
	airbagStatus.completed = [](AsyncPropertyReply* reply)
	{
		if(!reply->success)
			DebugOut(0)<<"Airbag Async request failed";
		else
			DebugOut(1)<<"Airbag Status: "<<reply->value->toString()<<endl;
		delete reply;
	};

	routingEngine->getPropertyAsync(airbagStatus);

	AsyncPropertyRequest exteriorBrightness;
	exteriorBrightness.property = VehicleProperty::ExteriorBrightness;
	exteriorBrightness.completed = [](AsyncPropertyReply* reply)
	{
		if(!reply->success)
			DebugOut(0)<<"Exterior Brightness Async request failed";
		else
			DebugOut(1)<<"Exterior Brightness: "<<reply->value->toString()<<endl;
		delete reply;
	};

	routingEngine->getPropertyAsync(exteriorBrightness);

	auto getRangedCb = [](gpointer data)
	{
		AbstractRoutingEngine* routingEngine = (AbstractRoutingEngine*)data;

		AsyncRangePropertyRequest vehicleSpeedFromLastWeek;

		vehicleSpeedFromLastWeek.timeBegin = amb::currentTime() - 10;
		vehicleSpeedFromLastWeek.timeEnd = amb::currentTime();
		vehicleSpeedFromLastWeek.property = VehicleProperty::VehicleSpeed;
		vehicleSpeedFromLastWeek.completed = [](AsyncRangePropertyReply* reply)
		{
			std::list<AbstractPropertyType*> values = reply->values;
			for(auto itr = values.begin(); itr != values.end(); itr++)
			{
				auto val = *itr;
				DebugOut(1)<<"Value from past: ("<<val->name<<"): "<<val->toString()<<" time: "<<val->timestamp<<endl;
			}

			delete reply;
		};

		routingEngine->getRangePropertyAsync(vehicleSpeedFromLastWeek);

		return 0;
	};

	g_timeout_add(10000, getRangedCb, routingEngine);
}

void ExampleSink::propertyChanged(VehicleProperty::Property property, AbstractPropertyType* value, std::string uuid)
{
	DebugOut()<<property<<" value: "<<value->toString()<<endl;
}

std::string ExampleSink::uuid()
{
	return "f7e4fab2-eb73-4842-9fb0-e1c550eb2d81";
}
