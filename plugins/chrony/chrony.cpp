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


#include "chrony.h"
#include "abstractroutingengine.h"
#include "debugout.h"
#include "listplusplus.h"

#include <glib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <math.h>

#define GPSTIME "GpsTime"

extern "C" AbstractSinkManager * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new ChronySinkManager(routingengine, config);
}

ChronySink::ChronySink(AbstractRoutingEngine* engine, map<string, string> config): AbstractSink(engine, config)
{
	routingEngine->subscribeToProperty(GPSTIME, this);
	supportedChanged(engine->supported());
}

PropertyList ChronySink::subscriptions()
{
}

void ChronySink::supportedChanged(const PropertyList & supportedProperties)
{
	DebugOut()<<"Support changed!"<<endl;
}

void ChronySink::propertyChanged(AbstractPropertyType *value)
{
	int sockfd;
	struct sockaddr_un s;
	struct chrony_sock_sample chronydata;

	VehicleProperty::Property property = value->name;
	DebugOut()<<property<<" value: "<<value->toString()<<endl;

	sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sockfd < 0) return;

	s.sun_family = AF_UNIX;
	strcpy(s.sun_path, CHRONYD_SOCKET );

	if(connect(sockfd, (struct sockaddr *)&s, sizeof(s)) == -1) 
	{
	        return;
	}

	chronydata.tv.tv_sec  = (int)floor(value->value<double>());
	chronydata.tv.tv_usec = 0;
	chronydata.offset = 0.0;
	chronydata.pulse = 0;
	chronydata.leap = 0;
	chronydata.magic = 0x534f434b;
	send(sockfd,&chronydata,sizeof(chronydata),0);

	close(sockfd);
}

const string ChronySink::uuid()
{
	return "35324592-db72-11e4-b432-0022684a4a24";
}
