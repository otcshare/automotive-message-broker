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

#include <boost/date_time/posix_time/posix_time.hpp>


extern "C" AbstractSinkManager * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new ExampleSinkManager(routingengine, config);
}

ExampleSink::ExampleSink(AbstractRoutingEngine* engine, map<string, string> config): AbstractSink(engine, config)
{
	routingEngine->subscribeToProperty(VehicleProperty::EngineSpeed, this);
	routingEngine->subscribeToProperty(VehicleProperty::VehicleSpeed, this);

}


PropertyList ExampleSink::subscriptions()
{

}

void ExampleSink::supportedChanged(PropertyList supportedProperties)
{
	printf("Support changed!\n");
	routingEngine->subscribeToProperty(VehicleProperty::EngineSpeed, this);
	routingEngine->subscribeToProperty(VehicleProperty::VehicleSpeed, this);

	AsyncPropertyRequest velocityRequest;
	velocityRequest.property = VehicleProperty::VehicleSpeed;
	velocityRequest.completed = [](AsyncPropertyReply* reply)
	{
		DebugOut()<<"Velocity Async request completed: "<<reply->value->toString()<<endl; delete reply;
	};

	routingEngine->getPropertyAsync(velocityRequest);

	AsyncPropertyRequest vinRequest;
	vinRequest.property = VehicleProperty::VIN;
	vinRequest.completed = [](AsyncPropertyReply* reply) { DebugOut()<<"VIN Async request completed: "<<reply->value->toString()<<endl; delete reply; };

	routingEngine->getPropertyAsync(vinRequest);

	AsyncPropertyRequest wmiRequest;
	wmiRequest.property = VehicleProperty::WMI;
	wmiRequest.completed = [](AsyncPropertyReply* reply) { DebugOut()<<"WMI Async request completed: "<<reply->value->toString()<<endl; delete reply; };

	routingEngine->getPropertyAsync(wmiRequest);

	AsyncPropertyRequest batteryVoltageRequest;
	batteryVoltageRequest.property = VehicleProperty::BatteryVoltage;
	batteryVoltageRequest.completed = [](AsyncPropertyReply* reply) { DebugOut()<<"BatteryVoltage Async request completed: "<<reply->value->toString()<<endl; delete reply; };

	routingEngine->getPropertyAsync(batteryVoltageRequest);

	AsyncRangePropertyRequest vehicleSpeedFromLastWeek;

	vehicleSpeedFromLastWeek.timeBegin = 1354233906.54099;
	vehicleSpeedFromLastWeek.timeEnd = 1354234153.03318;
	vehicleSpeedFromLastWeek.property = VehicleProperty::VehicleSpeed;
	vehicleSpeedFromLastWeek.completed = [](AsyncRangePropertyReply* reply)
	{
		std::list<AbstractPropertyType*> values = reply->values;
		for(auto itr = values.begin(); itr != values.end(); itr++)
		{
			auto val = *itr;
			DebugOut(0)<<"Velocity value from past: "<<val->toString()<<" time: "<<val->timestamp<<endl;
		}
	};

	routingEngine->getRangePropertyAsync(vehicleSpeedFromLastWeek);

}

void ExampleSink::propertyChanged(VehicleProperty::Property property, AbstractPropertyType* value, std::string uuid)
{
	DebugOut()<<property<<" value: "<<value->toString()<<endl;
}

std::string ExampleSink::uuid()
{
	return "f7e4fab2-eb73-4842-9fb0-e1c550eb2d81";
}
