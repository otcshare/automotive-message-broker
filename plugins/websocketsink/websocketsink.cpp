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




WebSocketSink::WebSocketSink(AbstractRoutingEngine* re,libwebsocket *wsi,string uuid,VehicleProperty::Property property) : AbstractSink(re)
{
	m_uuid = uuid;
	m_wsi = wsi;
	re->subscribeToProperty(property,this);
}
string WebSocketSink::uuid()
{
	return m_uuid;
}
void WebSocketSink::propertyChanged(VehicleProperty::Property property, boost::any value, string  uuid)
{
  //printf("Got property:%i\n",boost::any_cast<uint16_t>(reply->value));
		uint16_t velocity = boost::any_cast<uint16_t>(value);
		stringstream s;
		
		//TODO: Dirty hack hardcoded stuff, jsut to make it work.
		string tmpstr = "";
		if (property == VehicleProperty::VehicleSpeed)
		{
		  tmpstr = "running_status_speedometer";
		}
		else if (property == VehicleProperty::EngineSpeed)
		{
		  tmpstr = "running_status_engine_speed";
		}
		else if (property == VehicleProperty::SteeringWheelAngle)
		{
		  tmpstr = "running_status_steering_wheel_angle";
		}
		else if (property == VehicleProperty::TransmissionShiftPosition)
		{
		  tmpstr = "running_status_transmission_gear_status";
		}
		
		
		s << "{\"type\":\"valuechanged\",\"name\":\"" << tmpstr << "\",\"data\":\"" << velocity << "\",\"transactionid\":\"" << m_uuid << "\"}";
		
		string replystr = s.str();
		printf("Reply: %s\n",replystr.c_str());

		char *new_response = new char[LWS_SEND_BUFFER_PRE_PADDING + strlen(replystr.c_str()) + LWS_SEND_BUFFER_POST_PADDING];
		new_response+=LWS_SEND_BUFFER_PRE_PADDING;
		strcpy(new_response,replystr.c_str());
		libwebsocket_write(m_wsi, (unsigned char*)new_response, strlen(new_response), LWS_WRITE_TEXT);
}
void WebSocketSink::supportedChanged(PropertyList supportedProperties)
{
}
PropertyList WebSocketSink::subscriptions()
{
	return PropertyList();
} 

