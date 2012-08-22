/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2012  Michael Carpenter <email>

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


#include "tcpsinkplugin.h"
#include <glib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>




TcpSinkPlugin::TcpSinkPlugin(AbstractRoutingEngine* re) : AbstractSink(re)
{

}
string TcpSinkPlugin::uuid()
{
	return "e43f6cad-60e3-4454-9638-01ffa9ab8c8f";
}
void TcpSinkPlugin::propertyChanged(VehicleProperty::Property property, boost::any value, string  uuid)
{
}
void TcpSinkPlugin::supportedChanged(PropertyList supportedProperties)
{
}
PropertyList TcpSinkPlugin::subscriptions()
{
	return PropertyList();
} 

