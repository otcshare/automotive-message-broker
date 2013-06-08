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


#include <iostream>
#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <glib.h>
#include <sstream>
//#include <json-glib/json-glib.h>
#include <listplusplus.h>
#include <vehicleproperty.h>
#include "debugout.h"
#include "timestamp.h"
#include "testplugin.h"
#define __SMALLFILE__ std::string(__FILE__).substr(std::string(__FILE__).rfind("/")+1)
AbstractRoutingEngine *m_re;

//std::list<ObdPid*> Obd2Amb::supportedPidsList;

bool beginsWith(std::string a, std::string b)
{
	return (a.compare(0, b.length(), b) == 0);
}
void TestPlugin::updateProperty(VehicleProperty::Property property,AbstractPropertyType* value)
{

}

void TestPlugin::setSupported(PropertyList list)
{
	m_re->updateSupported(list,PropertyList());
}
void TestPlugin::setConfiguration(map<string, string> config)
{
// 	//Config has been passed, let's start stuff up.
}

TestPlugin::TestPlugin(AbstractRoutingEngine *re, map<string, string> config)
	: AbstractSource(re, config)
{
  DebugOut() << "Testing MapPropertyType... " << endl;
  MapPropertyType<BasicPropertyType<Door::Location>,BasicPropertyType<Door::Status>> propmap("something");
  MapPropertyType<BasicPropertyType<Door::Location>,BasicPropertyType<Door::Status>> propmaptwo("something");
  propmap.append(Door::LeftRear,Door::Ajar);
  GVariant *var = propmap.toVariant();
  gsize dictsize = g_variant_n_children(var);
  //DebugOut() << var << endl;
  propmaptwo.fromVariant(var);

  g_assert(propmap.toString() == propmap.toString());

  DebugOut() << "Testing ListPropertyType... " << endl;
  VehicleProperty::TripMetersType* tfirst = new VehicleProperty::TripMetersType();
  VehicleProperty::TripMetersType* tsecond = new VehicleProperty::TripMetersType();
  BasicPropertyType<uint16_t> v1(0);
  BasicPropertyType<uint16_t> v2(5);
  BasicPropertyType<uint16_t> v3(10);
  tfirst->append(&v1);
  tfirst->append(&v2);
  tfirst->append(&v3);
  tsecond->fromVariant(tfirst->toVariant());

  g_assert (tfirst->toString() == tsecond->toString());

  DebugOut() << "Exiting..." << endl;
  exit(-1);
}
TestPlugin::~TestPlugin()
{
	DebugOut() << "TestPlugin Destructor called!!!"<<endl;
}

PropertyList TestPlugin::supported()
{
	return PropertyList();
}

int TestPlugin::supportedOperations()
{
	return Get | Set;
}

extern "C" AbstractSource * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new TestPlugin(routingengine, config);
	
}
string TestPlugin::uuid()
{
	return "f77af740-f1f8-11e1-aff1-0800200c9a66";
}
void TestPlugin::subscribeToPropertyChanges(VehicleProperty::Property property)
{
}


void TestPlugin::unsubscribeToPropertyChanges(VehicleProperty::Property property)
{
}


void TestPlugin::getPropertyAsync(AsyncPropertyReply *reply)
{
	
}

AsyncPropertyReply *TestPlugin::setProperty(AsyncSetPropertyRequest request )
{
	AsyncPropertyReply* reply = new AsyncPropertyReply (request);
	return reply;
}
