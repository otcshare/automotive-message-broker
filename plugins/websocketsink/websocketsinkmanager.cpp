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


#include "websocketsinkmanager.h"
#include "websocketsink.h"
#include <sstream>
#include <json-glib/json-glib.h>
#include <listplusplus.h>
#define __SMALLFILE__ std::string(__FILE__).substr(std::string(__FILE__).rfind("/")+1)

//Global variables, these will be moved into the class
struct pollfd pollfds[100];
int count_pollfds = 0;
libwebsocket_context *context;
WebSocketSinkManager *sinkManager;
static int websocket_callback(struct libwebsocket_context *context,struct libwebsocket *wsi,enum libwebsocket_callback_reasons reason, void *user,void *in, size_t len);
bool gioPollingFunc(GIOChannel *source,GIOCondition condition,gpointer data);



WebSocketSinkManager::WebSocketSinkManager(AbstractRoutingEngine* engine):AbstractSinkManager(engine)
{
	m_engine = engine;
	
	//Protocol list for libwebsockets.
	protocollist[0] = { "http-only", websocket_callback, 0 };
	protocollist[1] = { NULL, NULL, 0 };

	int port = 23000;
	const char *interface = "lo";
	const char *ssl_cert_path = NULL;
	const char *ssl_key_path = NULL;
	int options = 0;
	
	//Create a listening socket on port 23000 on localhost.
	context = libwebsocket_create_context(port, interface, protocollist,libwebsocket_internal_extensions,ssl_cert_path, ssl_key_path, -1, -1, options);
}
void WebSocketSinkManager::addSingleShotSink(libwebsocket* socket, VehicleProperty::Property property,string id)
{
	AsyncPropertyRequest velocityRequest;
	if (property == "running_status_speedometer")
	{
		velocityRequest.property = VehicleProperty::VehicleSpeed;
	}
	else if (property == "running_status_engine_speed")
	{
		velocityRequest.property = VehicleProperty::EngineSpeed;
	}
	else if ("running_status_steering_wheel_angle")
	{
		velocityRequest.property = VehicleProperty::SteeringWheelAngle;
	}
	else if ("running_status_transmission_gear_status")
	{
		velocityRequest.property = VehicleProperty::TransmissionShiftPosition;
	}
	else
	{
		PropertyList foo = VehicleProperty::capabilities();
		if (ListPlusPlus<VehicleProperty::Property>(&foo).contains(property))
		{
			velocityRequest.property = property;
		}
		else
		{
			//Invalid property requested.
			return;
		}
		
	}
	velocityRequest.completed = [socket,id,property](AsyncPropertyReply* reply)
	{
		printf("Got property:%s\n",reply->value.toString().c_str());
		//uint16_t velocity = boost::any_cast<uint16_t>(reply->value);
		stringstream s;
		
		//TODO: Dirty hack hardcoded stuff, jsut to make it work.
		string tmpstr = "";
		/*if (property == VehicleProperty::VehicleSpeed)
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
		else
		{
			//PropertyList foo = VehicleProperty::capabilities();
			//if (ListPlusPlus<VehicleProperty::Property>(&foo).contains(property))
			//{
			  tmpstr = property;
			//}
			//else
			//{
				
			//}
		}*/
		tmpstr = property;
		s << "{\"type\":\"methodReply\",\"name\":\"get\",\"data\":[{\"name\":\"" << tmpstr << "\",\"value\":\"" << reply->value.toString() << "\"}],\"transactionid\":\"" << id << "\"}";
		
		string replystr = s.str();
		printf("Reply: %s\n",replystr.c_str());

		char *new_response = new char[LWS_SEND_BUFFER_PRE_PADDING + strlen(replystr.c_str()) + LWS_SEND_BUFFER_POST_PADDING];
		new_response+=LWS_SEND_BUFFER_PRE_PADDING;
		strcpy(new_response,replystr.c_str());
		libwebsocket_write(socket, (unsigned char*)new_response, strlen(new_response), LWS_WRITE_TEXT);
		
		//TODO: run valgrind on this. libwebsocket's documentation says NOTHING about this, yet malloc insists it's true.
		//delete new_response; <- Unneeded. Apparently libwebsocket free's it.
		
	};

	AsyncPropertyReply* reply = routingEngine->getPropertyAsync(velocityRequest);
}
void WebSocketSinkManager::removeSink(libwebsocket* socket,VehicleProperty::Property property, string uuid)
{
	if (m_sinkMap.find(property) != m_sinkMap.end())
	{
		WebSocketSink* sink = m_sinkMap[property];
		delete sink;
		m_sinkMap.erase(property);
		stringstream s;
		s << "{\"type\":\"methodReply\",\"name\":\"unsubscribe\",\"data\":[\"" << property << "\"],\"transactionid\":\"" << uuid << "\"}";
		
		string replystr = s.str();
		printf("Reply: %s\n",replystr.c_str());

		char *new_response = new char[LWS_SEND_BUFFER_PRE_PADDING + strlen(replystr.c_str()) + LWS_SEND_BUFFER_POST_PADDING];
		new_response+=LWS_SEND_BUFFER_PRE_PADDING;
		strcpy(new_response,replystr.c_str());
		libwebsocket_write(socket, (unsigned char*)new_response, strlen(new_response), LWS_WRITE_TEXT);

	}
}
void WebSocketSinkManager::addSink(libwebsocket* socket, VehicleProperty::Property property,string uuid)
{
	stringstream s;
	
	//TODO: Dirty hack hardcoded stuff, jsut to make it work.
	string tmpstr = "";
	if (property == "running_status_speedometer")
	{
		tmpstr = VehicleProperty::VehicleSpeed;
	}
	else if (property == "running_status_engine_speed")
	{
		tmpstr = VehicleProperty::EngineSpeed;
	}
	else if ("running_status_steering_wheel_angle")
	{
		tmpstr = VehicleProperty::SteeringWheelAngle;
	}
	else if ("running_status_transmission_gear_status")
	{
		tmpstr = VehicleProperty::TransmissionShiftPosition;
	}
	else
	{
		PropertyList foo = VehicleProperty::capabilities();
		if (ListPlusPlus<VehicleProperty::Property>(&foo).contains(property))
		{
			tmpstr = property;
		}
		else
		{
			//Invalid property requested.
			return;
		}
		
	}
	s << "{\"type\":\"methodReply\",\"name\":\"subscribe\",\"data\":[\"" << property << "\"],\"transactionid\":\"" << uuid << "\"}";
	
	string replystr = s.str();
	printf("Reply: %s\n",replystr.c_str());

	char *new_response = new char[LWS_SEND_BUFFER_PRE_PADDING + strlen(replystr.c_str()) + LWS_SEND_BUFFER_POST_PADDING];
	new_response+=LWS_SEND_BUFFER_PRE_PADDING;
	strcpy(new_response,replystr.c_str());
	libwebsocket_write(socket, (unsigned char*)new_response, strlen(new_response), LWS_WRITE_TEXT);

	WebSocketSink *sink = new WebSocketSink(m_engine,socket,uuid,property,tmpstr);
	m_sinkMap[property] = sink;
}

extern "C" AbstractSinkManager * create(AbstractRoutingEngine* routingengine)
{
	sinkManager = new WebSocketSinkManager(routingengine);
	return sinkManager;
}
static int websocket_callback(struct libwebsocket_context *context,struct libwebsocket *wsi,enum libwebsocket_callback_reasons reason, void *user,void *in, size_t len)
{
	printf("Switch: %i\n",reason);
	
	switch (reason)
	{
		case LWS_CALLBACK_CLIENT_WRITEABLE:
		{
			//Connection has been established.
			printf("Connection established\n");
			break;
		}
		case LWS_CALLBACK_CLIENT_RECEIVE:
		{
			printf("Client writable\n");
		}
		case LWS_CALLBACK_SERVER_WRITEABLE:
		{
			printf("Server writable\n");
		}
		
		case LWS_CALLBACK_RECEIVE:
		{
			printf("Data Received: %s\n",(char*)in);
		}
		case LWS_CALLBACK_HTTP:
		{
			//TODO: Verify that ALL requests get sent via LWS_CALLBACK_HTTP, so we can use that instead of LWS_CALLBACK_RECIEVE
			//TODO: Do we want exceptions, or just to return an invalid json reply? Probably an invalid json reply.
			DebugOut() << __SMALLFILE__ << ":" << __LINE__ << " Requested: " << (char*)in;
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
			
			
			if (type == "method")
			{
				if (name == "get")
				{
					if (data.size() > 0)
					{
						//GetProperty is going to be a singleshot sink.
						//string arg = arguments.front();
						sinkManager->addSingleShotSink(wsi,data.front(),id);
						/*if (data.front()== "running_status_speedometer")
						{			   
							sinkManager->addSingleShotSink(wsi,VehicleProperty::VehicleSpeed,id);
						}
						else if (data.front() == "running_status_engine_speed")
						{
							sinkManager->addSingleShotSink(wsi,VehicleProperty::EngineSpeed,id);
						}
						else if (data.front() == "running_status_steering_wheel_angle")
						{
							sinkManager->addSingleShotSink(wsi,VehicleProperty::SteeringWheelAngle,id);
						}
						else if (data.front() == "running_status_transmission_gear_status")
						{
							sinkManager->addSingleShotSink(wsi,VehicleProperty::TransmissionShiftPosition,id);
						}
						else
						{
						  PropertyList foo = VehicleProperty::capabilities();
						  if (ListPlusPlus<VehicleProperty::Property>(&foo).contains(data.front()))
						  {
						    sinkManager->addSingleShotSink(wsi,data.front(),id);
						  }
						}*/
					}
					else
					{
						DebugOut() << __SMALLFILE__ << ":" << __LINE__ << " \"get\" method called with no data! Transaction ID:" << id;
					}
				}
				else if (name == "subscribe")
				{
					sinkManager->addSink(wsi,data.front(),id);
				  /*
					if (data.front()== "running_status_speedometer")
					{
						sinkManager->addSink(wsi,VehicleProperty::VehicleSpeed,id);
					}
					else if (data.front()== "running_status_engine_speed")
					{
						sinkManager->addSink(wsi,VehicleProperty::EngineSpeed,id);
					}
					else if (data.front() == "running_status_steering_wheel_angle")
					{
						sinkManager->addSink(wsi,VehicleProperty::SteeringWheelAngle,id);
					}
					else if (data.front() == "running_status_transmission_gear_status")
					{
						sinkManager->addSink(wsi,VehicleProperty::TransmissionShiftPosition,id);
					}
					else
					{
						//Unsupported subscription type.
						PropertyList foo = VehicleProperty::capabilities();
						if (ListPlusPlus<VehicleProperty::Property>(&foo).contains(data.front()))
						{
							sinkManager->addSink(wsi,data.front(),id);
						}
						else
						{
							DebugOut() << __SMALLFILE__ << ":" << __LINE__ << " Unsupported subscription type:" << data.front();
						}
					}*/
				}
				else if (name == "unsubscribe")
				{
					sinkManager->removeSink(wsi,data.front(),id);
					/*if (data.front()== "running_status_speedometer")
					{
						sinkManager->removeSink(wsi,VehicleProperty::VehicleSpeed,id);
					}
					else if (data.front()== "running_status_engine_speed")
					{
						sinkManager->removeSink(wsi,VehicleProperty::EngineSpeed,id);
					}
					else if (data.front() == "running_status_steering_wheel_angle")
					{
						sinkManager->removeSink(wsi,VehicleProperty::SteeringWheelAngle,id);
					}
					else if (data.front() == "running_status_transmission_gear_status")
					{
						sinkManager->removeSink(wsi,VehicleProperty::TransmissionShiftPosition,id);
					}
					else
					{
						PropertyList foo = VehicleProperty::capabilities();
						if (ListPlusPlus<VehicleProperty::Property>(&foo).contains(data.front()))
						{
							sinkManager->removeSink(wsi,data.front(),id);
						}
						else
						{
							//Unsupported unsubscribe
							DebugOut() << __SMALLFILE__ << ":" << __LINE__ << " Unsupported unsubscription type:" << data.front();
						}
					}*/
				}
				else if (name == "getSupportedEventTypes")
				{
					string typessupported = "";
					if (data.size() == 0)
					{
						//Send what properties we support
						typessupported = "\"running_status_speedometer\",\"running_status_engine_speed\",\"running_status_steering_wheel_angle\",\"running_status_transmission_gear_status\"";
						PropertyList foo = VehicleProperty::capabilities();
						//ListPlusPlus<VehicleProperty::Property>(&foo)
						PropertyList::const_iterator i=foo.cbegin();
						while (i != foo.cend())
						{
							typessupported.append(",\"").append((*i)).append("\"");
							i++;
						}
					}
					else
					{
						//Send what events a particular property supports
						if (data.front()== "running_status_speedometer")
						{
							typessupported = "\"get\",\"subscribe\",\"unsubscribe\",\"getSupportedEventTypes\"";
						}
						else if (data.front()== "running_status_engine_speed")
						{
							typessupported = "\"get\",\"subscribe\",\"unsubscribe\",\"getSupportedEventTypes\"";
						}
						else if (data.front() == "running_status_steering_wheel_angle")
						{
							typessupported = "\"get\",\"subscribe\",\"unsubscribe\",\"getSupportedEventTypes\"";
						}
						else if (data.front() == "running_status_transmission_gear_status")
						{
							typessupported = "\"get\",\"subscribe\",\"unsubscribe\",\"getSupportedEventTypes\"";
						}
						else
						{
							PropertyList foo = VehicleProperty::capabilities();
							if (ListPlusPlus<VehicleProperty::Property>(&foo).contains(data.front()))
							{
								//sinkManager->addSingleShotSink(wsi,data.front(),id);
								typessupported = "\"get\",\"subscribe\",\"unsubscribe\",\"getSupportedEventTypes\"";
							}
						}
					}
					stringstream s;
					string s2;

					s << "{\"type\":\"methodReply\",\"name\":\"getSupportedEventTypes\",\"data\":[" << typessupported << "],\"transactionid\":\"" << id << "\"}";
					string replystr = s.str();
					DebugOut() << __SMALLFILE__ << ":" << __LINE__ << " JSON Reply: " << replystr;
					//printf("Reply: %s\n",replystr.c_str());
					char *new_response = new char[LWS_SEND_BUFFER_PRE_PADDING + strlen(replystr.c_str()) + LWS_SEND_BUFFER_POST_PADDING];
					new_response+=LWS_SEND_BUFFER_PRE_PADDING;
					strcpy(new_response,replystr.c_str());
					libwebsocket_write(wsi, (unsigned char*)new_response, strlen(new_response), LWS_WRITE_TEXT);	    
				}
			}
			break;
		}
		case LWS_CALLBACK_ADD_POLL_FD:
		{
			//Add a FD to the poll list.
			//printf("Adding poll\n");
			GIOChannel *chan = g_io_channel_unix_new((int)(long)user);
			g_io_add_watch(chan,G_IO_IN,(GIOFunc)gioPollingFunc,0);
			g_io_add_watch(chan,G_IO_PRI,(GIOFunc)gioPollingFunc,0);
			pollfds[count_pollfds].fd = (int)(long)user;
			pollfds[count_pollfds].events = (int)len;
			pollfds[count_pollfds++].revents = 0;
			break;
		}
		case LWS_CALLBACK_DEL_POLL_FD:
		{
			//Remove FD from the poll list. I'm not convinced this is needed anymore
			for (int n = 0; n < count_pollfds; n++)
			{
				if (pollfds[n].fd == (int)(long)user)
				{
					while (n < count_pollfds)
					{
						pollfds[n] = pollfds[n + 1];
						n++;
					}
				}
			}
			count_pollfds--;
			break;
		}
		case LWS_CALLBACK_SET_MODE_POLL_FD:
		{
			//Set the poll mode
			GIOChannel *chan = g_io_channel_unix_new((int)(long)user);
			g_io_add_watch(chan,(GIOCondition)(int)len,(GIOFunc)gioPollingFunc,0);
			break;
		}
		case LWS_CALLBACK_CLEAR_MODE_POLL_FD:
		{
			//Don't handle this yet.
			break;
		}
		default:
		{
			printf("Unhandled callback: %i\n",reason);
			break;
		}
	}
	return 0; 
}

bool gioPollingFunc(GIOChannel *source,GIOCondition condition,gpointer data)
{
	//This is the polling function. If it return false, glib will stop polling this FD.
	printf("Polling...%i\n",condition);
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
	
	return true;
}

