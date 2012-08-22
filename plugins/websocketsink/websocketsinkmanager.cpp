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

	int port = 7681;
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
	velocityRequest.property = VehicleProperty::VehicleSpeed;
	velocityRequest.completed = [socket,id](AsyncPropertyReply* reply) {
		printf("Got property:%i\n",boost::any_cast<uint16_t>(reply->value));
		uint16_t velocity = boost::any_cast<uint16_t>(reply->value);
		stringstream s;
		
		//TODO: Dirty hack hardcoded stuff, jsut to make it work.
		s << "{\"type\":\"reply\",\"name\":\"Velocity\",\"arguments\":\"[\"" << velocity << "\"],\"transactionid\":\"" << id << "\"}";
		
		string replystr = s.str();
		

		char *new_response = new char[LWS_SEND_BUFFER_PRE_PADDING + strlen(replystr.c_str()) + LWS_SEND_BUFFER_POST_PADDING];
		new_response+=LWS_SEND_BUFFER_PRE_PADDING;
		strcpy(new_response,replystr.c_str());
		libwebsocket_write(socket, (unsigned char*)new_response, strlen(new_response), LWS_WRITE_TEXT);
		
		//TODO: run valgrind on this. libwebsocket's documentation says NOTHING about this, yet malloc insists it's true.
		//delete new_response; <- Unneeded. Apparently libwebsocket free's it.
		
	};

	AsyncPropertyReply* reply = routingEngine->getPropertyAsync(velocityRequest);
}
void WebSocketSinkManager::addSink(libwebsocket* socket, VehicleProperty::Property property,string uuid)
{
	WebSocketSink *sink = new WebSocketSink(m_engine,socket,uuid);
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
			printf("requested URI: %s\n", (char*)in);
			
			/*{
			  "type":"method",
			  "name":"GetProperty",
			  "Arguments":["Velocity"],
			  "transactionid":"0f234002-95b8-48ac-aa06-cb49e372cc1c"
			  }
			  */
			JsonParser* parser = json_parser_new();
			GError* error = nullptr;
			if (!json_parser_load_from_data(parser,(char*)in,len,&error))
			{
			  printf("Error loading JSON\n");
			  return 0;
			}
			
			JsonNode* node = json_parser_get_root(parser);
			
			if(node == nullptr)
			{
				printf("Error getting root node of json\n");
				return 0;
			}
				//throw std::runtime_error("Unable to get JSON root object");
			
			JsonReader* reader = json_reader_new(node);
			
			if(reader == nullptr)
			{
				printf("json_reader is null!\n");
				return 0;
			}
				//throw std::runtime_error("Unable to create JSON reader");
			
			//DebugOut()<<"Config members: "<<json_reader_count_members(reader)<<endl;
			
			gchar** members = json_reader_list_members(reader);
			string type;
			string  name;
			list<string> arguments;
			//stringlist arguments
			string id;
			json_reader_read_member(reader,"type");
			type = json_reader_get_string_value(reader);
			json_reader_end_member(reader);
			
			json_reader_read_member(reader,"name");
			name = json_reader_get_string_value(reader);
			json_reader_end_member(reader);
			
			json_reader_read_member(reader,"Arguments");
			g_assert(json_reader_is_array(reader));
			for(int i=0; i < json_reader_count_elements(reader); i++)
			{
				json_reader_read_element(reader,i);
				string path = json_reader_get_string_value(reader);
				arguments.push_back(path);
				json_reader_end_element(reader);
			}
			json_reader_end_member(reader);
			
			
			
			json_reader_read_member(reader,"transactionid");
			id = json_reader_get_string_value(reader);
			json_reader_end_member(reader);
			
			
			if (type == "method")
			{
			  if (name == "GetProperty")
			  {
			    //GetProperty is going to be a singleshot sink.
			    string arg = arguments.front();
			    if (arg == "Velocity")
			    {			   
			      printf("Found velocity\n");
			    //m_engine->subscribeToProperty(VehicleProperty::VehicleSpeed,this);
			      
			    
			    sinkManager->addSingleShotSink(wsi,VehicleProperty::Property::VehicleSpeed,id);
			    //libwebsocket_write(wsi, (unsigned char*)new_response, strlen(new_response), LWS_WRITE_TEXT);
			    }
			    
			  }
			  else if (name == "Subscribe")
			  {
			    //Subscribe is a permanent sink, until unsubscription.
			    sinkManager->addSink(wsi,VehicleProperty::VehicleSpeed,id);
			  }
			}
			
			///TODO: this will probably explode:
			//mlc: I agree with Kevron here, it does explode.
			//if(error) g_error_free(error);
			
			g_object_unref(reader);
			g_object_unref(parser);
			break;
		}
		case LWS_CALLBACK_ADD_POLL_FD:
		{
			//Add a FD to the poll list.
			printf("Adding poll\n");
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
			//Remove FD from the poll list.
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

