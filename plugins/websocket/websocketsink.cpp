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

#include <QJsonDocument>
#include <QVariantMap>

static int lwsWrite(struct libwebsocket *lws, const std::string& strToWrite)
{
	std::unique_ptr<char[]> buffer(new char[LWS_SEND_BUFFER_PRE_PADDING + strToWrite.length() + LWS_SEND_BUFFER_POST_PADDING]);

	char *buf = buffer.get() + LWS_SEND_BUFFER_PRE_PADDING;
	strcpy(buf, strToWrite.c_str());

	//NOTE: delete[] on buffer is not needed since std::unique_ptr<char[]> is used
	return libwebsocket_write(lws, (unsigned char*)buf, strToWrite.length(), LWS_WRITE_TEXT);
}


WebSocketSink::WebSocketSink(AbstractRoutingEngine* re,libwebsocket *wsi,string uuid,VehicleProperty::Property property,std::string ambdproperty) : AbstractSink(re,map<string, string> ())
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

#ifndef QTBINARY_DATA
	stringstream s;
	
	//TODO: Dirty hack hardcoded stuff, jsut to make it work.
	std::string tmpstr="";
	if (m_property != property)
	{
		tmpstr = m_property;
	}
	else
	{
		tmpstr = property;
	}
	
	s.precision(15);
	
	s << "{\"type\":\"valuechanged\",\"name\":\"" << tmpstr << "\",\"data\":";
	s << "{ \"value\":\"" << value->toString() << "\",\"zone\":\""<<value->zone;
	s << "\",\"timestamp\":\""<<value->timestamp<<"\",\"sequence\":\""<<value->sequence<<"\"},";
	s << "\"transactionid\":\"" << m_uuid << "\"}";
	
	string replystr = s.str();
	//printf("Reply: %s\n",replystr.c_str());
	
	DebugOut() << "Reply:" << replystr << "\n";
#else
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

	string replystr = QJsonDocument::fromVariant(reply).toBinaryData().data();
#endif

	lwsWrite(m_wsi, replystr);
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

