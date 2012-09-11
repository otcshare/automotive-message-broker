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

extern "C" AbstractSinkManager * create(AbstractRoutingEngine* routingengine)
{
	return new ExampleSinkManager(routingengine);
}

ExampleSink::ExampleSink(AbstractRoutingEngine* engine): AbstractSink(engine)
{
	routingEngine->subscribeToProperty(VehicleProperty::EngineSpeed, this);
	routingEngine->subscribeToProperty(VehicleProperty::VehicleSpeed, this);

	AsyncPropertyRequest velocityRequest;
	velocityRequest.property = VehicleProperty::VehicleSpeed;
	velocityRequest.completed = [](AsyncPropertyReply* reply) { DebugOut()<<"Velocity Async request completed: "<<reply->value->toString()<<endl; delete reply; };

	AsyncPropertyReply* reply = routingEngine->getPropertyAsync(velocityRequest);

}


PropertyList ExampleSink::subscriptions()
{

}

void ExampleSink::supportedChanged(PropertyList supportedProperties)
{
	printf("Support changed!\n");
	routingEngine->subscribeToProperty(VehicleProperty::EngineSpeed, this);
	routingEngine->subscribeToProperty(VehicleProperty::VehicleSpeed, this);
}

void ExampleSink::propertyChanged(VehicleProperty::Property property, AbstractPropertyType* value, std::string uuid)
{
	DebugOut()<<property<<" value: "<<value->toString()<<endl;
}

std::string ExampleSink::uuid()
{
	return "f7e4fab2-eb73-4842-9fb0-e1c550eb2d81";
}
