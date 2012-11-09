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

#ifndef WEBSOCKETSINKMANAGER_H
#define WEBSOCKETSINKMANAGER_H

#include <abstractroutingengine.h>
#include <abstractsink.h>
#include "websocketsink.h"
#include <gio/gio.h>
#include <map>
#include <libwebsockets.h>
#include "debugout.h"
#include <stdexcept>
#include "sys/types.h"
#include <stdlib.h>

class WebSocketSinkManager: public AbstractSinkManager
{
public:
	WebSocketSinkManager(AbstractRoutingEngine* engine, map<string, string> config);
	void addSingleShotSink(libwebsocket* socket, VehicleProperty::Property property,string id);
	void addSingleShotRangedSink(libwebsocket* socket, VehicleProperty::Property property,double start, double end, string id);
	void addSink(libwebsocket* socket, VehicleProperty::Property property,string uuid);
	void disconnectAll(libwebsocket* socket);
	void removeSink(libwebsocket* socket,VehicleProperty::Property property,string uuid);
	void addPoll(int fd);
	void removePoll(int fd);
	void init();
	map<std::string, list<WebSocketSink*> > m_sinkMap;
	void setConfiguration(map<string, string> config);
	void setValue(string property,string value);
private:
	map<int,GIOChannel*> m_ioChannelMap;
	map<int,guint> m_ioSourceMap;
  AbstractRoutingEngine *m_engine;
	struct libwebsocket_protocols protocollist[2];
};

#endif // WEBSOCKETSINKMANAGER_H
