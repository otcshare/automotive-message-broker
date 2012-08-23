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
#include <gio/gio.h>
#include <libwebsockets.h>

class WebSocketSinkManager: public AbstractSinkManager
{
public:
	WebSocketSinkManager(AbstractRoutingEngine* engine);
	void addSingleShotSink(libwebsocket* socket, VehicleProperty::Property property,string id);
	void addSink(libwebsocket* socket, VehicleProperty::Property property,string uuid);
private:
  AbstractRoutingEngine *m_engine;
	struct libwebsocket_protocols protocollist[2];
};

#endif // WEBSOCKETSINKMANAGER_H
