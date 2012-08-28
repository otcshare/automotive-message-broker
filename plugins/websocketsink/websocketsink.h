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


#ifndef WEBSOCKETSINK_H
#define WEBSOCKETSINK_H
#include <glib.h>
#include <abstractroutingengine.h>
#include "abstractsink.h"
#include <libwebsockets.h>
class WebSocketSink : public AbstractSink
{

public:
	WebSocketSink(AbstractRoutingEngine* re,libwebsocket *wsi,string uuid,VehicleProperty::Property property,std::string ambdproperty);
	~WebSocketSink();
	string uuid() ;
	void propertyChanged(VehicleProperty::Property property, AbstractPropertyType value, string  uuid);
	void supportedChanged(PropertyList supportedProperties);
	PropertyList subscriptions();
private:
	string m_amdbproperty;
	AbstractRoutingEngine *m_re;
	libwebsocket *m_wsi;
	string m_uuid;
	string m_property;
};

#endif // WEBSOCKETSINK_H
