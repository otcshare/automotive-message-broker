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

#include "demosinkplugin.h"
#include "abstractroutingengine.h"

#include <iostream>
#include <stdexcept>
#include <boost/assert.hpp>
#include <glib.h>

using namespace std;

#include "debugout.h"

string findReplace(string str, string tofind, string replacewith, string exclusions="")
{
	size_t i=0;

	size_t exclusionPos = exclusions.find(tofind,0);

	while((i = str.find(tofind,i)) != string::npos)
	{
		if( exclusionPos != string::npos )
		{
			if(str.substr(i-exclusionPos,exclusions.length()) == exclusions)
			{
				i+=replacewith.size();
				continue;
			}
		}

		str.replace(i,tofind.size(),replacewith);
		i+=replacewith.size();
	}

}


DemoSink::DemoSink(AbstractRoutingEngine* re)
:AbstractSink(re)
{

}

DemoSink::~DemoSink()
{

}

extern "C" AbstractSinkManager * create(AbstractRoutingEngine* routingengine)
{
	return new DemoSinkManager(routingengine);
}

string DemoSink::uuid()
{
	return "5b0e8a04-d6d7-43af-b827-1663627a25d9";
}

void DemoSink::propertyChanged(VehicleProperty::Property property, AbstractPropertyType *value, string uuid)
{
	std::string app = configuration["script"];

	string cmdline = findReplace(app,"%1",value->toString());
	GError* error = NULL;

	g_spawn_command_line_async(cmdline.c_str(), &error);

}

void DemoSink::supportedChanged(PropertyList list)
{
	routingEngine->subscribeToProperty(VehicleProperty::ButtonEvent, this);
	routingEngine->subscribeToProperty(VehicleProperty::TurnSignal, this);
}
