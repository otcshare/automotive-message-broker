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

#include "openxcplugin.h"
#include "timestamp.h"

#include <iostream>
#include <boost/assert.hpp>
#include <glib.h>

using namespace std;

#include "debugout.h"

static gboolean timeoutCallback(gpointer data)
{
	OpenXCPlugin* src = (OpenXCPlugin*)data;
	
	src->randomizeProperties();
	
	return true;
}

OpenXCPlugin::OpenXCPlugin(AbstractRoutingEngine* re, map<string, string> config)
:AbstractSource(re, config), velocity(0), engineSpeed(0)
{
	re->setSupported(supported(), this);	
}



extern "C" AbstractSource * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new OpenXCPlugin(routingengine, config);
	
}

string OpenXCPlugin::uuid()
{
	return "openxc";
}


void OpenXCPlugin::getPropertyAsync(AsyncPropertyReply *reply)
{

}

void OpenXCPlugin::getRangePropertyAsync(AsyncRangePropertyReply *reply)
{

}

AsyncPropertyReply *OpenXCPlugin::setProperty(AsyncSetPropertyRequest request )
{

}

void OpenXCPlugin::subscribeToPropertyChanges(VehicleProperty::Property property)
{
	mRequests.push_back(property);
}

PropertyList OpenXCPlugin::supported()
{
	PropertyList props;
	
	return props;
}

int OpenXCPlugin::supportedOperations()
{
	return Get;
}

void OpenXCPlugin::unsubscribeToPropertyChanges(VehicleProperty::Property property)
{
	mRequests.remove(property);
}

