/*
    Copyright (C) 2012  Intel Corporation
    Copyright (C) 2015  AWTC Europe

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
#include "ambplugin.h"
#include "abstractroutingengine.h"
#include "debugout.h"
#include "listplusplus.h"

#include <glib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <math.h>

extern "C" void create(AbstractRoutingEngine* routingEngine, map<string, string> config)
{
        auto plugin = new AmbPlugin<ChronySink>(routingEngine, config);
        plugin->init();
}

ChronySink::ChronySink(AbstractRoutingEngine* re, const std::map<std::string, std::string>& config, AbstractSource& parent)
	:AmbPluginImpl(re, config, parent)
{
}

void ChronySink::init()
{
	routingEngine->subscribeToProperty("GpsTime", &source);
}

PropertyList ChronySink::subscriptions()
{
}

void ChronySink::propertyChanged(AbstractPropertyType *value)
{
	int sockfd;
	struct sockaddr_un s;
	struct chrony_sock_sample chronydata;

	sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sockfd < 0) return;

	s.sun_family = AF_UNIX;
	strcpy(s.sun_path, CHRONYD_SOCKET );

	if(connect(sockfd, (struct sockaddr *)&s, sizeof(s)) == -1) 
	{
	        return;
	}

	gettimeofday(&(chronydata.tv), NULL);
	chronydata.offset  = (value->value<double>() - chronydata.tv.tv_sec) - (chronydata.tv.tv_usec / 1000000.0);
	chronydata.offset -= (amb::currentTime()-value->timestamp);
	chronydata.pulse = 0;
	chronydata.leap = 0;
	chronydata.magic = 0x534f434b;
	send(sockfd,&chronydata,sizeof(chronydata),0);

	close(sockfd);
}

