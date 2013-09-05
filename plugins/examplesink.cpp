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
#include "listplusplus.h"

#include <glib.h>


extern "C" AbstractSinkManager * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new ExampleSinkManager(routingengine, config);
}

ExampleSink::ExampleSink(AbstractRoutingEngine* engine, map<string, string> config): AbstractSink(engine, config)
{
	routingEngine->subscribeToProperty(VehicleProperty::EngineSpeed, this);
	routingEngine->subscribeToProperty(VehicleProperty::VehicleSpeed, this);
	routingEngine->subscribeToProperty(VehicleProperty::Latitude, this);
	routingEngine->subscribeToProperty(VehicleProperty::Longitude, this);

	supportedChanged(engine->supported());
}


PropertyList ExampleSink::subscriptions()
{

}

void ExampleSink::supportedChanged(PropertyList supportedProperties)
{
	DebugOut()<<"Support changed!"<<endl;

	if(ListPlusPlus<VehicleProperty::Property>(&supportedProperties).contains(VehicleProperty::VehicleSpeed))
	{
		AsyncPropertyRequest velocityRequest;
		velocityRequest.property = VehicleProperty::VehicleSpeed;
		velocityRequest.completed = [](AsyncPropertyReply* reply)
		{
			if(!reply->success)
				DebugOut(DebugOut::Error)<<"Velocity Async request failed ("<<reply->error<<")"<<endl;
			else
				DebugOut(0)<<"Velocity Async request completed: "<<reply->value->toString()<<endl;
			delete reply;
		};

		routingEngine->getPropertyAsync(velocityRequest);
	}

	if(ListPlusPlus<VehicleProperty::Property>(&supportedProperties).contains(VehicleProperty::VIN))
	{
		AsyncPropertyRequest vinRequest;
		vinRequest.property = VehicleProperty::VIN;
		vinRequest.completed = [](AsyncPropertyReply* reply)
		{
			if(!reply->success)
				DebugOut(DebugOut::Error)<<"VIN Async request failed ("<<reply->error<<")"<<endl;
			else
				DebugOut(0)<<"VIN Async request completed: "<<reply->value->toString()<<endl;
			delete reply;
		};

		routingEngine->getPropertyAsync(vinRequest);
	}
	if(ListPlusPlus<VehicleProperty::Property>(&supportedProperties).contains(VehicleProperty::WMI))
	{
		AsyncPropertyRequest wmiRequest;
		wmiRequest.property = VehicleProperty::WMI;
		wmiRequest.completed = [](AsyncPropertyReply* reply)
		{
			if(!reply->success)
				DebugOut(DebugOut::Error)<<"WMI Async request failed ("<<reply->error<<")"<<endl;
			else
				DebugOut(1)<<"WMI Async request completed: "<<reply->value->toString()<<endl;
			delete reply;
		};

		routingEngine->getPropertyAsync(wmiRequest);
	}
	if(ListPlusPlus<VehicleProperty::Property>(&supportedProperties).contains(VehicleProperty::BatteryVoltage))
	{
		AsyncPropertyRequest batteryVoltageRequest;
		batteryVoltageRequest.property = VehicleProperty::BatteryVoltage;
		batteryVoltageRequest.completed = [](AsyncPropertyReply* reply)
		{
			if(!reply->success)
				DebugOut(DebugOut::Error)<<"BatteryVoltage Async request failed ("<<reply->error<<")"<<endl;
			else
				DebugOut(1)<<"BatteryVoltage Async request completed: "<<reply->value->toString()<<endl;
			delete reply;
		};

		routingEngine->getPropertyAsync(batteryVoltageRequest);
	}
	if(ListPlusPlus<VehicleProperty::Property>(&supportedProperties).contains(VehicleProperty::DoorsPerRow))
	{
		AsyncPropertyRequest doorsPerRowRequest;
		doorsPerRowRequest.property = VehicleProperty::DoorsPerRow;
		doorsPerRowRequest.completed = [](AsyncPropertyReply* reply)
		{
			if(!reply->success)
				DebugOut(DebugOut::Error)<<"Doors per row Async request failed ("<<reply->error<<")"<<endl;
			else
				DebugOut(1)<<"Doors per row: "<<reply->value->toString()<<endl;
			delete reply;
		};

		routingEngine->getPropertyAsync(doorsPerRowRequest);
	}

	if(ListPlusPlus<VehicleProperty::Property>(&supportedProperties).contains(VehicleProperty::AirbagStatus))
	{
		AsyncPropertyRequest airbagStatus;
		airbagStatus.property = VehicleProperty::AirbagStatus;
		airbagStatus.completed = [](AsyncPropertyReply* reply)
		{
			if(!reply->success)
				DebugOut(DebugOut::Error)<<"Airbag Async request failed ("<<reply->error<<")"<<endl;
			else
				DebugOut(1)<<"Airbag Status: "<<reply->value->toString()<<endl;
			delete reply;
		};

		routingEngine->getPropertyAsync(airbagStatus);
	}

	if(ListPlusPlus<VehicleProperty::Property>(&supportedProperties).contains(VehicleProperty::ExteriorBrightness))
	{
		AsyncPropertyRequest exteriorBrightness;
		exteriorBrightness.property = VehicleProperty::ExteriorBrightness;
		exteriorBrightness.completed = [](AsyncPropertyReply* reply)
		{
			if(!reply->success)
				DebugOut(DebugOut::Error)<<"Exterior Brightness Async request failed ("<<reply->error<<")"<<endl;
			else
				DebugOut(1)<<"Exterior Brightness: "<<reply->value->toString()<<endl;
			delete reply;
		};

		routingEngine->getPropertyAsync(exteriorBrightness);
	}

	auto getRangedCb = [](gpointer data)
	{
		AbstractRoutingEngine* routingEngine = (AbstractRoutingEngine*)data;

		AsyncRangePropertyRequest vehicleSpeedFromLastWeek;

		vehicleSpeedFromLastWeek.timeBegin = amb::currentTime() - 10;
		vehicleSpeedFromLastWeek.timeEnd = amb::currentTime();

		PropertyList requestList;
		requestList.push_back(VehicleProperty::VehicleSpeed);
		requestList.push_back(VehicleProperty::EngineSpeed);

		vehicleSpeedFromLastWeek.properties = requestList;
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

void ExampleSink::propertyChanged(AbstractPropertyType* value, const string &uuid)
{
	VehicleProperty::Property property = value->name;
	DebugOut()<<property<<" value: "<<value->toString()<<endl;
}

const string ExampleSink::uuid()
{
	return "f7e4fab2-eb73-4842-9fb0-e1c550eb2d81";
}
