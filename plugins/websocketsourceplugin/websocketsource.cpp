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


#include "websocketsource.h"
#include <libwebsockets.h>
#include <iostream>
#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <glib.h>
#include <sstream>
#include <json-glib/json-glib.h>
#include "debugout.h"
#define __SMALLFILE__ std::string(__FILE__).substr(std::string(__FILE__).rfind("/")+1)
libwebsocket_context *context;
AbstractRoutingEngine *m_re;
bool gioPollingFunc(GIOChannel *source,GIOCondition condition,gpointer data)
{
	//This is the polling function. If it return false, glib will stop polling this FD.
	//printf("Polling...%i\n",condition);
	lws_tokens token;
	struct pollfd pollstruct;
	int newfd = g_io_channel_unix_get_fd(source);
	pollstruct.fd = newfd;
	pollstruct.events = condition;
	pollstruct.revents = condition;
	libwebsocket_service_fd(context,&pollstruct);
	if (condition == G_IO_HUP)
	{
		//Hang up. Returning false closes out the GIOChannel.
		printf("Callback on G_IO_HUP\n");
		return false;
	}
	if (condition == G_IO_IN)
	{
	}
	
	return true;
}

static int callback_http_only(libwebsocket_context *context,struct libwebsocket *wsi,enum libwebsocket_callback_reasons reason,void *user, void *in, size_t len)
{
  unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 4096 +
						  LWS_SEND_BUFFER_POST_PADDING];
	int l;
	printf("Switch: %i\n",reason);
	switch (reason) {

	case LWS_CALLBACK_CLOSED:
		//fprintf(stderr, "mirror: LWS_CALLBACK_CLOSED\n");
		//wsi_mirror = NULL;
		printf("Connection closed!\n");
		break;

	case LWS_CALLBACK_CLIENT_ESTABLISHED:
	{
	  printf("Incoming connection!\n");
	  stringstream s;
	  	s << "{\"type\":\"method\",\"name\":\"subscribe\",\"data\":[\"" << "VehicleSpeed" << "\"],\"transactionid\":\"" << "d293f670-f0b3-11e1-aff1-0800200c9a66" << "\"}";
	
	string replystr = s.str();
	printf("Reply: %s\n",replystr.c_str());

	char *new_response = new char[LWS_SEND_BUFFER_PRE_PADDING + strlen(replystr.c_str()) + LWS_SEND_BUFFER_POST_PADDING];
	new_response+=LWS_SEND_BUFFER_PRE_PADDING;
	strcpy(new_response,replystr.c_str());
	libwebsocket_write(wsi, (unsigned char*)new_response, strlen(new_response), LWS_WRITE_TEXT);  

	  break;
	}
	case LWS_CALLBACK_CLIENT_RECEIVE:
	{
	  //(char*)in;
		GError* error = nullptr;
		
		
		JsonParser* parser = json_parser_new();
		if (!json_parser_load_from_data(parser,(char*)in,len,&error))
		{
			DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Error loading JSON";
			return 0;
		}
		
		JsonNode* node = json_parser_get_root(parser);
		if(node == nullptr)
		{
			DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Error getting root node of json";
			//throw std::runtime_error("Unable to get JSON root object");
			return 0;
		}
		
		JsonReader* reader = json_reader_new(node);
		if(reader == nullptr)
		{
			DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "json_reader is null!";
			//throw std::runtime_error("Unable to create JSON reader");
			return 0;
		}
		
		
		
		
		
		string type;
		json_reader_read_member(reader,"type");
		type = json_reader_get_string_value(reader);
		json_reader_end_member(reader);
		
		string  name;
		json_reader_read_member(reader,"name");
		name = json_reader_get_string_value(reader);
		json_reader_end_member(reader);

		list<string> data;
		json_reader_read_member(reader,"data");
		if (json_reader_is_array(reader))
		{
			for(int i=0; i < json_reader_count_elements(reader); i++)
			{
				json_reader_read_element(reader,i);
				string path = json_reader_get_string_value(reader);
				data.push_back(path);
				json_reader_end_element(reader);
			}
		}
		else
		{
			string path = json_reader_get_string_value(reader);
			if (path != "")
			{
				data.push_back(path);
			}
		}
		json_reader_end_member(reader);
		
		string id;
		json_reader_read_member(reader,"transactionid");
		if (strcmp("gchararray",g_type_name(json_node_get_value_type(json_reader_get_value(reader)))) == 0)
		{
			//Type is a string
			id = json_reader_get_string_value(reader);
		}
		else
		{
			//Type is an integer
			stringstream strstr;
			strstr << json_reader_get_int_value(reader);
			id = strstr.str();
		}
		json_reader_end_member(reader);
		
		///TODO: this will probably explode:
		//mlc: I agree with Kevron here, it does explode.
		//if(error) g_error_free(error);
		
		g_object_unref(reader);
		g_object_unref(parser);
		
		
		if (type == "valuechanged")
		{
			//Name should be a valid property
			//	routingEngine->updateProperty(VehicleProperty::VehicleSpeed, velocity);
			//data.front()
			try
			{
				uint16_t val = boost::lexical_cast<uint16_t,string>(data.front());
				m_re->updateProperty(name,boost::any(val));
			}
			catch (exception ex)
			{
				printf("Exception %s\n",ex.what());
			}
			if (name == "get")
			{
				if (data.size() > 0)
				{
				}
			}
		}
		break;
	}
	case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED:
	{
		printf("Requested extension: %s\n",(char*)in);
		return 0;
		break;
	}
	case LWS_CALLBACK_ADD_POLL_FD:
	{
		//Add a FD to the poll list.
		GIOChannel *chan = g_io_channel_unix_new((int)(long)user);
		g_io_add_watch(chan,G_IO_IN,(GIOFunc)gioPollingFunc,0);
		g_io_add_watch(chan,G_IO_PRI,(GIOFunc)gioPollingFunc,0);
		break;
	}
	return 0;
}
}
static struct libwebsocket_protocols protocols[] = {
	{
		"http-only",
		callback_http_only,
		0,
	},
	{  /* end of list */
		NULL,
		NULL,
		0
	}
};
WebSocketSource::WebSocketSource(AbstractRoutingEngine *re) : AbstractSource(re)
{
	m_re = re;  
	context = libwebsocket_create_context(CONTEXT_PORT_NO_LISTEN, NULL,protocols, libwebsocket_internal_extensions,NULL, NULL, -1, -1, 0);
	
	//Read JSON that will tell us what to do:
	
	GError* error = nullptr;
	JsonParser* parser = json_parser_new();
	if (!json_parser_load_from_file(parser,"websocketsource.conf",&error))
	{
		g_error_free(error);
		if (!json_parser_load_from_file(parser,"../../plugins/websocketsourceplugin/websocketsource.conf",&error))
		{
			g_error_free(error);
			if (!json_parser_load_from_file(parser,"/etc/ambd/websocketsource.conf",&error))
			{
				g_error_free(error);
				DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Error loading JSON";
			}
		}
	}
	/*if (!json_parser_load_from_data(parser,(char*)in,len,&error))
	{
		DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Error loading JSON";
		return 0;
	}*/
	JsonNode* node = json_parser_get_root(parser);
	if(node == nullptr)
	{
		DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Error getting root node of json";
		//throw std::runtime_error("Unable to get JSON root object");
		return;
	}
	JsonReader* reader = json_reader_new(node);
	if(reader == nullptr)
	{
		DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "json_reader is null!";
		//throw std::runtime_error("Unable to create JSON reader");
		return;
	}
	
	
	list<string> data;
	json_reader_read_member(reader,"sinks");
	if (json_reader_is_array(reader))
	{
		for(int i=0; i < json_reader_count_elements(reader); i++)
		{
			json_reader_read_element(reader,i);
			json_reader_read_member(reader,"ip");
			string ip = json_reader_get_string_value(reader);
			json_reader_end_member(reader);
			
			json_reader_read_member(reader,"port");
			int port = json_reader_get_int_value(reader);
			json_reader_end_member(reader);
			printf("Connecting to %s on port %i\n",ip.c_str(),port);
			libwebsocket *wsi_dumb = libwebsocket_client_connect(context, ip.c_str(), port, 0,"/", "localhost", "websocket",protocols[0].name, -1);
			
			json_reader_end_element(reader);
		}
	}
	else
	{
		//string path = json_reader_get_string_value(reader);
		//if (path != "")
		//{
		//	data.push_back(path);
		//}
	}
	json_reader_end_member(reader);
	/*string type;
	json_reader_read_member(reader,"type");
	type = json_reader_get_string_value(reader);
	json_reader_end_member(reader);
	
	string  name;
	json_reader_read_member(reader,"name");
	name = json_reader_get_string_value(reader);
	json_reader_end_member(reader);
	*/	
		
	
	re->setSupported(supported(), this);
	
}
PropertyList WebSocketSource::supported()
{
	PropertyList props;
	props.push_back(VehicleProperty::EngineSpeed);
	props.push_back(VehicleProperty::VehicleSpeed);
	props.push_back(VehicleProperty::AccelerationX);
	props.push_back(VehicleProperty::TransmissionShiftPosition);
	props.push_back(VehicleProperty::SteeringWheelAngle);
	props.push_back(VehicleProperty::ThrottlePosition);
	props.push_back(VehicleProperty::EngineCoolantTemperature);
	
	return props;
}
extern "C" AbstractSource * create(AbstractRoutingEngine* routingengine)
{
	return new WebSocketSource(routingengine);
	
}
string WebSocketSource::uuid()
{
	return "d293f670-f0b3-11e1-aff1-0800200c9a66";
}
boost::any WebSocketSource::getProperty(VehicleProperty::Property property)
{
	if(property == VehicleProperty::VehicleSpeed)
	{
		//return velocity;
	}
	else if(property == VehicleProperty::EngineSpeed)
	{
		//return engineSpeed;
	}
}
void WebSocketSource::subscribeToPropertyChanges(VehicleProperty::Property property)
{
	printf("Subscribed to property: %s\n",property.c_str());
	mRequests.push_back(property);
}


void WebSocketSource::unsubscribeToPropertyChanges(VehicleProperty::Property property)
{
	mRequests.remove(property);
}

void WebSocketSource::getPropertyAsync(AsyncPropertyReply *reply)
{
	/*if(reply->property == VehicleProperty::VehicleSpeed)
	{
		reply->value = velocity;
		reply->completed(reply);
	}
	else if(reply->property == VehicleProperty::EngineSpeed)
	{
		reply->value = engineSpeed;
		reply->completed(reply);
	}
	else if(reply->property == VehicleProperty::AccelerationX)
	{
		reply->value = accelerationX;
		reply->completed(reply);
	}
	else if(reply->property == VehicleProperty::TransmissionShiftPosition)
	{
		reply->value = transmissionShiftPostion;
		reply->completed(reply);
	}
	else if(reply->property == VehicleProperty::SteeringWheelAngle)
	{
		reply->value = steeringWheelAngle;
		reply->completed(reply);
	}*/
}

void WebSocketSource::setProperty(VehicleProperty::Property , boost::any )
{

}

/*void ExampleSourcePlugin::getPropertyAsync(AsyncPropertyReply *reply)
{
	if(reply->property == VehicleProperty::VehicleSpeed)
	{
		reply->value = velocity;
		reply->completed(reply);
	}
	else if(reply->property == VehicleProperty::EngineSpeed)
	{
		reply->value = engineSpeed;
		reply->completed(reply);
	}
	else if(reply->property == VehicleProperty::AccelerationX)
	{
		reply->value = accelerationX;
		reply->completed(reply);
	}
	else if(reply->property == VehicleProperty::TransmissionShiftPosition)
	{
		reply->value = transmissionShiftPostion;
		reply->completed(reply);
	}
	else if(reply->property == VehicleProperty::SteeringWheelAngle)
	{
		reply->value = steeringWheelAngle;
		reply->completed(reply);
	}
}

void ExampleSourcePlugin::setProperty(VehicleProperty::Property , boost::any )
{

}



void ExampleSourcePlugin::randomizeProperties()
{
	velocity = 1 + (255.00 * (rand() / (RAND_MAX + 1.0)));
	engineSpeed = 1 + (15000.00 * (rand() / (RAND_MAX + 1.0)));
	accelerationX = 1 + (15000.00 * (rand() / (RAND_MAX + 1.0)));
	transmissionShiftPostion = 1 + (6.00 * (rand() / (RAND_MAX + 1.0)));
	steeringWheelAngle = 1 + (359.00 * (rand() / (RAND_MAX + 1.0)));
	throttlePos = 1 + (100.00 * (rand() / (RAND_MAX + 1.0)));
	engineCoolant = 1 + (40.00 * (rand() / (RAND_MAX + 140.0)));
	
	DebugOut()<<"setting velocity to: "<<velocity<<endl;
	DebugOut()<<"setting enginespeed to: "<<engineSpeed<<endl;
	
	routingEngine->updateProperty(VehicleProperty::VehicleSpeed, velocity);
	routingEngine->updateProperty(VehicleProperty::EngineSpeed, engineSpeed);
	routingEngine->updateProperty(VehicleProperty::AccelerationX, accelerationX);
	routingEngine->updateProperty(VehicleProperty::SteeringWheelAngle, steeringWheelAngle);
	routingEngine->updateProperty(VehicleProperty::TransmissionShiftPosition, transmissionShiftPostion);
	routingEngine->updateProperty(VehicleProperty::ThrottlePosition, throttlePos);
	routingEngine->updateProperty(VehicleProperty::EngineCoolantTemperature, engineCoolant);
}
*/