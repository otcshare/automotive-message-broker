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


#include "websocketsink.h"
#include <glib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sstream>
#include "debugout.h"
#include "common.h"

#include <QJsonDocument>
#include <QVariantMap>

WebSocketSink::WebSocketSink(AbstractRoutingEngine* re, libwebsocket *wsi, string uuid, VehicleProperty::Property property, std::string ambdproperty) : AbstractSink(re,map<string, string> ())
{
	m_amdbproperty = ambdproperty;
	m_uuid = uuid;
	m_wsi = wsi;
	m_property = property;
	m_re = re;
	re->subscribeToProperty(ambdproperty,this);
}
const string WebSocketSink::uuid()
{
	return m_uuid;
}
void WebSocketSink::propertyChanged(AbstractPropertyType *value)
{
	VehicleProperty::Property property = value->name;

	QVariantMap data;
	QVariantMap reply;

	data["value"] = value->toString().c_str();
	data["zone"] = value->zone;
	data["timestamp"]=value->timestamp;
	data["sequence"]=value->sequence;

	reply["data"]=data;
	reply["type"]="valuechanged";
	reply["name"]=property.c_str();
	reply["transactionid"]=m_uuid.c_str();

	QByteArray replystr;
	if(doBinary)
		replystr = QJsonDocument::fromVariant(reply).toBinaryData();
	else
	{
		replystr = QJsonDocument::fromVariant(reply).toJson();
		cleanJson(replystr);
	}

	lwsWrite(m_wsi, replystr,replystr.length());
}
WebSocketSink::~WebSocketSink()
{
	m_re->unsubscribeToProperty(m_amdbproperty, this);
}
void WebSocketSink::supportedChanged(PropertyList supportedProperties)
{
}
PropertyList WebSocketSink::subscriptions()
{
	return PropertyList();
} 

/// 6% and 4% cpu with json
/// 5% and 4% with binary
