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
#include <iostream>
#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <glib.h>
#include <sstream>
#include <listplusplus.h>
#include <timestamp.h>
#include "uuidhelper.h"

#include "debugout.h"
#define __SMALLFILE__ std::string(__FILE__).substr(std::string(__FILE__).rfind("/")+1)
libwebsocket_context *context = NULL;
WebSocketSource *source;
AbstractRoutingEngine *m_re;

double oldTimestamp=0;
double totalTime=0;
double numUpdates=0;
double averageLatency=0;

static int callback_http_only(libwebsocket_context *context,struct libwebsocket *wsi,enum libwebsocket_callback_reasons reason,void *user, void *in, size_t len);
static struct libwebsocket_protocols protocols[] = {
	{
		"http-only",
		callback_http_only,
		0,
		128,
	},
	{  /* end of list */
		NULL,
		NULL,
		0,
		0
	}
};

//Called when a client connects, subscribes, or unsubscribes.
void WebSocketSource::checkSubscriptions()
{
	PropertyList notSupportedList;
	while (queuedRequests.size() > 0)
	{
		VehicleProperty::Property prop = queuedRequests.front();
		queuedRequests.pop_front();
		if (ListPlusPlus<VehicleProperty::Property>(&activeRequests).contains(prop))
		{
			return;
		}
		activeRequests.push_back(prop);
		stringstream s;
		///TODO: fix transid here:
		s << "{\"type\":\"method\",\"name\":\"subscribe\",\"data\":[\"" << prop << "\"],\"transactionid\":\"" << "d293f670-f0b3-11e1-aff1-0800200c9a66" << "\"}";

		string replystr = s.str();
		DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Reply:" << replystr << "\n";
		//printf("Reply: %s\n",replystr.c_str());

		char *new_response = new char[LWS_SEND_BUFFER_PRE_PADDING + strlen(replystr.c_str()) + LWS_SEND_BUFFER_POST_PADDING];
		new_response+=LWS_SEND_BUFFER_PRE_PADDING;
		strcpy(new_response,replystr.c_str());
		if(clientsocket)
			libwebsocket_write(clientsocket, (unsigned char*)new_response, strlen(new_response), LWS_WRITE_TEXT);
		delete (char*)(new_response-LWS_SEND_BUFFER_PRE_PADDING);
	}
}
void WebSocketSource::setConfiguration(map<string, string> config)
{
	//printf("WebSocketSource::setConfiguration has been called\n");
	std::string ip;
	int port;
	configuration = config;
	for (map<string,string>::iterator i=configuration.begin();i!=configuration.end();i++)
	{
		DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Incoming setting for WebSocketSource:" << (*i).first << ":" << (*i).second << "\n";
		//printf("Incoming setting: %s:%s\n",(*i).first.c_str(),(*i).second.c_str());
		if ((*i).first == "ip")
		{
			ip = (*i).second;
		}
		if ((*i).first == "port")
		{
			port = boost::lexical_cast<int>((*i).second);
		}
		if ((*i).first == "ssl")
		{
			if ((*i).second == "true")
			{
				m_sslEnabled = true;
			}
			else
			{
				m_sslEnabled = false;
			} 	
		}
	}
	//printf("Connecting to websocket server at %s port %i\n",ip.c_str(),port);
	DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Connecting to websocket server at" << ip << ":" << port << "\n";
	int sslval = 0;
	if (m_sslEnabled)
	{
		DebugOut(5) << "SSL ENABLED" << endl;
		sslval = 2;
	}

	clientsocket = libwebsocket_client_connect(context, ip.c_str(), port, sslval,"/", "localhost", "websocket",protocols[0].name, -1);
	

}

PropertyInfo WebSocketSource::getPropertyInfo(VehicleProperty::Property property)
{
	return PropertyInfo::invalid();
}

bool gioPollingFunc(GIOChannel *source, GIOCondition condition, gpointer data)
{
	//This is the polling function. If it return false, glib will stop polling this FD.

	oldTimestamp = amb::currentTime();

	struct pollfd pollstruct;
	int newfd = g_io_channel_unix_get_fd(source);
	pollstruct.fd = newfd;
	pollstruct.events = condition;
	pollstruct.revents = condition;
	libwebsocket_service_fd(context,&pollstruct);
	if (condition & G_IO_HUP)
	{
		//Hang up. Returning false closes out the GIOChannel.
		//printf("Callback on G_IO_HUP\n");
		return false;
	}
	if (condition & G_IO_IN)
	{

	}
	DebugOut() << "gioPollingFunc" << condition << endl;

	return true;
}

static int checkTimeouts(gpointer data)
{
	WebSocketSource *src = (WebSocketSource*)data;
	for (auto i=src->uuidTimeoutMap.begin();i!= src->uuidTimeoutMap.end();i++)
	{
		if (src->uuidRangedReplyMap.find((*i).first) != src->uuidRangedReplyMap.end())
		{
			//A source exists!
			if (amb::currentTime() > (*i).second)
			{
				//We've reached timeout
				DebugOut() << "Timeout reached for request ID:" << (*i).first << "\n";
				src->uuidRangedReplyMap[(*i).first]->success = false;
				src->uuidRangedReplyMap[(*i).first]->completed(src->uuidRangedReplyMap[(*i).first]);
				src->uuidRangedReplyMap.erase((*i).first);
				src->uuidTimeoutMap.erase((*i).first);
				i--;

				if (src->uuidTimeoutMap.size() == 0)
				{
					return 0;
				}

			}
			else
			{
				//No timeout yet, keep waiting.
			}
		}
		else
		{
			//Reply has already come back, ignore and erase from list.
			src->uuidTimeoutMap.erase((*i).first);
			i--;

			if (src->uuidTimeoutMap.size() == 0)
			{
				return 0;
			}
		}

	}
	return 0;
}

static int callback_http_only(libwebsocket_context *context,struct libwebsocket *wsi,enum libwebsocket_callback_reasons reason,void *user, void *in, size_t len)
{
	unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 4096 + LWS_SEND_BUFFER_POST_PADDING];
	int l;
	DebugOut() << __SMALLFILE__ << ":" << __LINE__ << reason << "callback_http_only" << endl;
	switch (reason)
	{
		case LWS_CALLBACK_CLOSED:
			//fprintf(stderr, "mirror: LWS_CALLBACK_CLOSED\n");
			//wsi_mirror = NULL;
			//printf("Connection closed!\n");
			break;

		//case LWS_CALLBACK_PROTOCOL_INIT:
		case LWS_CALLBACK_CLIENT_ESTABLISHED:
		{
			//This happens when a client initally connects. We need to request the support event types.
			source->clientConnected = true;
			source->checkSubscriptions();
			//printf("Incoming connection!\n");
			DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Incoming connection" << endl;
			stringstream s;
			s << "{\"type\":\"method\",\"name\":\"getSupportedEventTypes\",\"data\":[],\"transactionid\":\"" << "d293f670-f0b3-11e1-aff1-0800200c9a66" << "\"}";

			string replystr = s.str();
			DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Reply:" << replystr << "\n";
			char *new_response = new char[LWS_SEND_BUFFER_PRE_PADDING + strlen(replystr.c_str()) + LWS_SEND_BUFFER_POST_PADDING];
			new_response+=LWS_SEND_BUFFER_PRE_PADDING;
			strcpy(new_response,replystr.c_str());
			libwebsocket_write(wsi, (unsigned char*)(new_response), strlen(new_response), LWS_WRITE_TEXT);
			delete (char*)(new_response-LWS_SEND_BUFFER_PRE_PADDING);

			break;
		}
		case LWS_CALLBACK_CLIENT_RECEIVE:
		{
			double prejsonparsetime = (amb::currentTime() - oldTimestamp)*1000;

			DebugOut(2)<<"websocket source pre-json parse time: "<<prejsonparsetime<<endl;

			json_object *rootobject;
			json_tokener *tokener = json_tokener_new();
			enum json_tokener_error err;
			do
			{
				rootobject = json_tokener_parse_ex(tokener, (char*)in,len);
			} while ((err = json_tokener_get_error(tokener)) == json_tokener_continue);
			if (err != json_tokener_success)
			{
				fprintf(stderr, "Error: %s\n", json_tokener_error_desc(err));
				// Handle errors, as appropriate for your application.
			}
			if (tokener->char_offset < len) // XXX shouldn't access internal fields
			{
				// Handle extra characters after parsed object as desired.
				// e.g. issue an error, parse another object from that point, etc...
			}
			//Incoming JSON reqest.
			

			DebugOut(5)<<"source received: "<<string((char*)in)<<endl;
			
			json_object *typeobject= json_object_object_get(rootobject,"type");
			json_object *nameobject= json_object_object_get(rootobject,"name");
			json_object *transidobject= json_object_object_get(rootobject,"transactionid");


			string type = string(json_object_get_string(typeobject));
			string name = string(json_object_get_string(nameobject));
			
			string id;
			
			if (json_object_get_type(transidobject) == json_type_string)
			{
				id = json_object_get_string(transidobject);
			}
			else
			{
				stringstream strstr;
				strstr << json_object_get_int(transidobject);
				id = strstr.str();
			}
			
			list<pair<string,string> > pairdata;
			if (type == "valuechanged")
			{
				json_object *dataobject = json_object_object_get(rootobject,"data");
				
				json_object *valueobject = json_object_object_get(dataobject,"value");
				json_object *timestampobject = json_object_object_get(dataobject,"timestamp");
				json_object *sequenceobject= json_object_object_get(dataobject,"sequence");
				
				string value = string(json_object_get_string(valueobject));
				string timestamp = string(json_object_get_string(timestampobject));
				string sequence = string(json_object_get_string(sequenceobject));
				//printf("Value changed: %s, %s\n",name.c_str(),data.front().c_str());
				DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Value changed:" << name << value << endl;
				//Name should be a valid property
				//	routingEngine->updateProperty(VehicleProperty::VehicleSpeed, velocity);
				//data.front()
				try
				{
					AbstractPropertyType* type = VehicleProperty::getPropertyTypeForPropertyNameValue(name,value);
					type->timestamp = boost::lexical_cast<double,std::string>(timestamp);
					type->sequence = boost::lexical_cast<double,std::string>(sequence);
					m_re->updateProperty(type, source->uuid());
					double currenttime = amb::currentTime();

					/** This is now the latency between when something is available to read on the socket, until
					 *  a property is about to be updated in AMB.  This includes libwebsockets parsing and the
					 *  JSON parsing in this section.
					 */
					
					DebugOut(2)<<"websocket parse latency: "<<(currenttime - oldTimestamp)*1000<<"ms"<<endl;
					DebugOut(2)<<"websocket network + parse latency: "<<(currenttime - type->timestamp)*1000<<"ms"<<endl;
					totalTime += (currenttime - oldTimestamp)*1000;
					numUpdates ++;
					averageLatency = totalTime / numUpdates;

					DebugOut(2)<<"Average parse latency: "<<averageLatency<<endl;

					delete type;
				}
				catch (exception ex)
				{
					//printf("Exception %s\n",ex.what());
					DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Exception:" << ex.what() << "\n";
				}
				json_object_put(valueobject);
				json_object_put(timestampobject);
				json_object_put(sequenceobject);
				json_object_put(dataobject);
				//printf("Done\n");
				/*if (name == "get")
				{
					if (data.size() > 0)
					{
					}
				}*/
			}
			else if (type == "methodReply")
			{
				json_object *dataobject = json_object_object_get(rootobject,"data");
				if (name == "getSupportedEventTypes")
				{
					//printf("Got supported events!\n");
					DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Got getSupportedEventTypes request"<<endl;
					PropertyList props;
					if (json_object_get_type(dataobject) == json_type_array)
					{
						array_list *dataarray = json_object_get_array(dataobject);
						for (int i=0;i<array_list_length(dataarray);i++)
						{
							json_object *arrayobj = (json_object*)array_list_get_idx(dataarray,i);
							props.push_back(string(json_object_get_string(arrayobj)));
						}
						//array_list_free(dataarray);
					}
					else
					{
						props.push_back(string(json_object_get_string(dataobject)));
					}
					source->setSupported(props);
					//m_re->updateSupported(m_supportedProperties,PropertyList());
				}
				else if (name == "getRanged")
				{
					std::list<AbstractPropertyType*> propertylist;
					array_list *dataarray = json_object_get_array(dataobject);
					for (int i=0;i<array_list_length(dataarray);i++)
					{
						json_object *arrayobj = (json_object*)array_list_get_idx(dataarray,i);
						json_object *keyobject = json_object_object_get(arrayobj,"name");
						json_object *valueobject = json_object_object_get(arrayobj,"value");
						json_object *timestampobject = json_object_object_get(arrayobj,"timestamp");
						json_object *sequenceobject = json_object_object_get(arrayobj,"sequence");
						std::string name = json_object_get_string(keyobject);
						std::string value = json_object_get_string(valueobject);
						std::string timestamp = json_object_get_string(timestampobject);
						std::string sequence = json_object_get_string(sequenceobject);

						///TODO: we might only have to free the dataobject at the end instead of this:

						json_object_put(keyobject);
						json_object_put(valueobject);
						json_object_put(timestampobject);
						json_object_put(sequenceobject);
							
						AbstractPropertyType* type = VehicleProperty::getPropertyTypeForPropertyNameValue(name,value);
						propertylist.push_back(type);
					}
					//array_list_free(dataarray);
					if (source->uuidRangedReplyMap.find(id) != source->uuidRangedReplyMap.end())
					{
						source->uuidRangedReplyMap[id]->values = propertylist;
						source->uuidRangedReplyMap[id]->success = true;
						source->uuidRangedReplyMap[id]->completed(source->uuidRangedReplyMap[id]);
						source->uuidRangedReplyMap.erase(id);
					}
					else
					{
						DebugOut() << "getRanged methodReply has been recieved, without a request being in!. This is likely due to a request coming in after the timeout has elapsed.\n";
					}
				}
				else if (name == "get")
				{
					
					DebugOut() << __SMALLFILE__ << ":" << __LINE__ << "Got \"GET\" event:" << pairdata.size()<<endl;
					if (source->uuidReplyMap.find(id) != source->uuidReplyMap.end())
					{
						json_object *propertyobject = json_object_object_get(dataobject,"property");
						json_object *valueobject = json_object_object_get(dataobject,"value");
						json_object *timestampobject = json_object_object_get(dataobject,"timestamp");
						json_object *sequenceobject = json_object_object_get(dataobject,"sequence");
						std::string property = json_object_get_string(propertyobject);
						std::string value = json_object_get_string(valueobject);
						std::string timestamp = json_object_get_string(timestampobject);
						std::string sequence = json_object_get_string(sequenceobject);
						json_object_put(propertyobject);
						json_object_put(valueobject);
						json_object_put(timestampobject);
						json_object_put(sequenceobject);
						
						AbstractPropertyType* v = VehicleProperty::getPropertyTypeForPropertyNameValue(property,value);
						v->timestamp = boost::lexical_cast<double,std::string>(timestamp);
						v->sequence = boost::lexical_cast<double,std::string>(sequence);
						if (source->uuidReplyMap.find(id) != source->uuidReplyMap.end())
						{
							source->uuidReplyMap[id]->value = v;
							source->uuidReplyMap[id]->success = true;
							source->uuidReplyMap[id]->completed(source->uuidReplyMap[id]);
							source->uuidReplyMap.erase(id);

						}
						else
						{
							DebugOut() << "get methodReply has been recieved, without a request being in!. This is likely due to a request coming in after the timeout has elapsed.\n";
						}

						delete v;
					}
					else
					{
						DebugOut() << __SMALLFILE__ << ":" << __LINE__ << "GET Method Reply INVALID! Multiple properties detected, only single are supported!!!" << "\n";
					}
					
					//data will contain a property/value map.
				}
				json_object_put(dataobject);
			}
			json_object_put(rootobject);

			break;

		}
		case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED:
		{
			//printf("Requested extension: %s\n",(char*)in);
			return 0;
			break;
		}
		case LWS_CALLBACK_ADD_POLL_FD:
		{
			DebugOut(5) << __SMALLFILE__ << ":" << __LINE__ << "Adding poll for websocket IO channel" << endl;
			//Add a FD to the poll list.
			GIOChannel *chan = g_io_channel_unix_new(libwebsocket_get_socket_fd(wsi));

			/// TODO: I changed this to be more consistent with the websocket sink end. it may not be correct. TEST

			g_io_add_watch(chan,GIOCondition(G_IO_IN | G_IO_PRI | G_IO_ERR | G_IO_HUP),(GIOFunc)gioPollingFunc,0);
			g_io_channel_set_close_on_unref(chan,true);
			g_io_channel_unref(chan); //Pass ownership of the GIOChannel to the watch.
			
			break;
		}
		return 0;
	}
}
void WebSocketSource::setSupported(PropertyList list)
{
	DebugOut() <<__SMALLFILE__ << ":" << __LINE__ <<"SET SUPPORTED"<<endl;
	m_supportedProperties = list;
	m_re->updateSupported(list,PropertyList(),this);
}

WebSocketSource::WebSocketSource(AbstractRoutingEngine *re, map<string, string> config) : AbstractSource(re, config)
{
	m_sslEnabled = false;
	clientConnected = false;
	source = this;
	m_re = re;
	struct lws_context_creation_info info;
	memset(&info, 0, sizeof info);
	info.protocols = protocols;
	info.extensions = libwebsocket_get_internal_extensions();
	info.gid = -1;
	info.uid = -1;
	info.port = CONTEXT_PORT_NO_LISTEN;
	//std::string ssl_key_path = "/home/michael/.ssh/id_rsa";
	//info.ssl_ca_filepath = ssl_key_path.c_str();
		
	context = libwebsocket_create_context(&info);
	//context = libwebsocket_create_context(CONTEXT_PORT_NO_LISTEN, NULL,protocols, libwebsocket_internal_extensions,NULL, NULL, -1, -1, 0);

	setConfiguration(config);
	re->setSupported(supported(), this);

	//printf("websocketsource loaded!!!\n");
	g_timeout_add(1000,checkTimeouts,this); //Do this once per second, check for functions that have timed out and reply with success = false;

}
PropertyList WebSocketSource::supported()
{
	return m_supportedProperties;
}

int WebSocketSource::supportedOperations()
{
	/// TODO: need to do this correctly based on what the host supports.
	return Get | Set | GetRanged;
}

const string WebSocketSource::uuid()
{
	return "d293f670-f0b3-11e1-aff1-0800200c9a66";
}

void WebSocketSource::subscribeToPropertyChanges(VehicleProperty::Property property)
{
	//printf("Subscribed to property: %s\n",property.c_str());
	queuedRequests.push_back(property);
	if (clientConnected)
	{
		checkSubscriptions();
	}
}


void WebSocketSource::unsubscribeToPropertyChanges(VehicleProperty::Property property)
{
	removeRequests.push_back(property);
	if (clientConnected)
	{
		checkSubscriptions();
	}
}


void WebSocketSource::getPropertyAsync(AsyncPropertyReply *reply)
{
	std::string uuid = amb::createUuid();
	uuidReplyMap[uuid] = reply;
	uuidTimeoutMap[uuid] = amb::currentTime() + 10.0; ///TODO: 10 second timeout, make this configurable?
	stringstream s;
	
	s << "{\"type\":\"method\",\"name\":\"get\",\"data\":[\"" << reply->property << "\"],\"transactionid\":\"" << uuid << "\"}";
	string replystr = s.str();
	DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Sending:" << replystr <<endl;
	//printf("Reply: %s\n",replystr.c_str());
	char *new_response = new char[LWS_SEND_BUFFER_PRE_PADDING + strlen(replystr.c_str()) + LWS_SEND_BUFFER_POST_PADDING];
	new_response+=LWS_SEND_BUFFER_PRE_PADDING;
	strcpy(new_response,replystr.c_str());
	if(clientsocket)
		libwebsocket_write(clientsocket, (unsigned char*)new_response, strlen(new_response), LWS_WRITE_TEXT);
	delete (char*)(new_response-LWS_SEND_BUFFER_PRE_PADDING);
}

void WebSocketSource::getRangePropertyAsync(AsyncRangePropertyReply *reply)
{
	std::string uuid = amb::createUuid();
	uuidRangedReplyMap[uuid] = reply;
	uuidTimeoutMap[uuid] = amb::currentTime() + 60; ///TODO: 60 second timeout, make this configurable?
	stringstream s;
	s.precision(15);
	s << "{\"type\":\"method\",\"name\":\"getRanged\",\"data\": {";

	s << "\"properties\":[";

	for (auto itr = reply->properties.begin(); itr != reply->properties.end(); itr++)
	{
		std::string prop = *itr;

		if(itr != reply->properties.begin())
		{
			s<<",";
		}

		s<<"\""<<prop<<"\"";
	}

	s<<"],";

	s << "\"timeBegin\":\"" << reply->timeBegin << "\",";
	s << "\"timeEnd\":\"" << reply->timeEnd << "\",";
	s << "\"sequenceBegin\":\"" << reply->sequenceBegin<< "\",";
	s << "\"sequenceEnd\":\"" << reply->sequenceEnd << "\"}";
	s<< ",\"transactionid\":\"" << uuid << "\"}";
	string replystr = s.str();
	DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Reply:" << replystr <<endl;
	//printf("Reply: %s\n",replystr.c_str());
	char *new_response = new char[LWS_SEND_BUFFER_PRE_PADDING + strlen(replystr.c_str()) + LWS_SEND_BUFFER_POST_PADDING];
	new_response+=LWS_SEND_BUFFER_PRE_PADDING;
	strcpy(new_response,replystr.c_str());
	if(clientsocket)
		libwebsocket_write(clientsocket, (unsigned char*)new_response, strlen(new_response), LWS_WRITE_TEXT);
	delete (char*)(new_response-LWS_SEND_BUFFER_PRE_PADDING);
}

AsyncPropertyReply * WebSocketSource::setProperty( AsyncSetPropertyRequest request )
{
	///TODO: fill in
		AsyncPropertyReply* reply = new AsyncPropertyReply(request);
	reply->success = true;
	stringstream s;
	s << "{\"type\":\"method\",\"name\":\"set\",\"data\":[\"property\" : \"" << request.property << "\",\"value\" : \"" << request.value << "\"],\"transactionid\":\"" << "d293f670-f0b3-11e1-aff1-0800200c9a66" << "\"}";
	string replystr = s.str();
	DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Reply:" << replystr << "\n";
	//printf("Reply: %s\n",replystr.c_str());
	char *new_response = new char[LWS_SEND_BUFFER_PRE_PADDING + strlen(replystr.c_str()) + LWS_SEND_BUFFER_POST_PADDING];
	new_response+=LWS_SEND_BUFFER_PRE_PADDING;
	strcpy(new_response,replystr.c_str());
	libwebsocket_write(clientsocket, (unsigned char*)new_response, strlen(new_response), LWS_WRITE_TEXT);
	delete (char*)(new_response-LWS_SEND_BUFFER_PRE_PADDING);
	reply->completed(reply);
	return reply;
}

extern "C" AbstractSource * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new WebSocketSource(routingengine, config);

}
