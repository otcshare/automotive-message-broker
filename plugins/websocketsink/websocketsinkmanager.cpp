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
#include <json/json.h>
#include <json/json_object.h>
#include <json/json_tokener.h>
#include <listplusplus.h>
#define __SMALLFILE__ std::string(__FILE__).substr(std::string(__FILE__).rfind("/")+1)

//Global variables, these will be moved into the class
struct pollfd pollfds[100];
int count_pollfds = 0;
libwebsocket_context *context;
WebSocketSinkManager *sinkManager;
static int websocket_callback(struct libwebsocket_context *context,struct libwebsocket *wsi,enum libwebsocket_callback_reasons reason, void *user,void *in, size_t len);
bool gioPollingFunc(GIOChannel *source,GIOCondition condition,gpointer data);



WebSocketSinkManager::WebSocketSinkManager(AbstractRoutingEngine* engine, map<string, string> config):AbstractSinkManager(engine, config)
{
	m_engine = engine;


	//Create a listening socket on port 23000 on localhost.


}
void WebSocketSinkManager::init()
{
  	//Protocol list for libwebsockets.
	protocollist[0] = { "http-only", websocket_callback, 0 };
	protocollist[1] = { NULL, NULL, 0 };


	setConfiguration(configuration);
}
list< VehicleProperty::Property > WebSocketSinkManager::getSupportedProperties()
{
	return m_engine->supported();
}
void WebSocketSinkManager::setConfiguration(map<string, string> config)
{
// 	//Config has been passed, let's start stuff up.
	configuration = config;
	struct lws_context_creation_info info;
	memset(&info, 0, sizeof info);

	//Default values
	int port = 23000;
	std::string interface = "lo";
	std::string ssl_cert_path;
	std::string ssl_key_path;
	int options = 0;
	bool ssl = false;
	//Try to load config
	for (map<string,string>::iterator i=configuration.begin();i!=configuration.end();i++)
	{
		//printf("Incoming setting: %s:%s\n",(*i).first.c_str(),(*i).second.c_str());
		DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Incoming setting:" << (*i).first << ":" << (*i).second << "\n";
		if ((*i).first == "interface")
		{
			interface = (*i).second;
		}
		if ((*i).first == "port")
		{
			port = boost::lexical_cast<int>((*i).second);
		}
		if ((*i).first == "cert")
		{
			ssl_cert_path = (*i).second;
		}
		if ((*i).first == "key")
		{
			ssl_key_path = (*i).second;
		}
		if ((*i).first == "ssl")
		{
			if ((*i).second == "true")
			{
				ssl = true;
			}
			else
			{
				ssl = false;
			}
		}
	}
	info.iface = interface.c_str();
	info.protocols = protocollist;
	info.extensions = libwebsocket_get_internal_extensions();
	info.gid = -1;
	info.uid = -1;
	info.options = options;
	info.port = port;
	if (ssl)
	{
		info.ssl_cert_filepath = ssl_cert_path.c_str();
		info.ssl_private_key_filepath = ssl_key_path.c_str();
	}
	context = libwebsocket_create_context(&info);
	
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
	else if (property == "running_status_steering_wheel_angle")
	{
		velocityRequest.property = VehicleProperty::SteeringWheelAngle;
	}
	else if (property == "running_status_transmission_gear_status")
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
			DebugOut(0)<<"websocketsink: Invalid property requested: "<<property;
			return;
		}

	}
	velocityRequest.completed = [socket,id,property](AsyncPropertyReply* reply)
	{
		DebugOut()<<"Got property: "<<reply->value->toString().c_str()<<endl;
		//uint16_t velocity = boost::any_cast<uint16_t>(reply->value);
		stringstream s;
		s.precision(15);
		//TODO: Dirty hack hardcoded stuff, jsut to make it work.
		string tmpstr = "";
		tmpstr = property;

		/// TODO: timestamp and sequence need to be inside the "data" object:

		s << "{\"type\":\"methodReply\",\"name\":\"get\",\"data\":{";
		s << "\"property\":\"" << tmpstr << "\",\"value\":\"" << reply->value->toString() << "\",\"timestamp\":\""<<reply->value->timestamp<<"\",";
		s <<"\"sequence\": \""<<reply->value->sequence<<"\"}";
		s << ",\"transactionid\":\"" << id << "\"}";
		

		string replystr = s.str();
		//printf("Reply: %s\n",replystr.c_str());
		DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Reply:" << replystr << endl;

		char *new_response = new char[LWS_SEND_BUFFER_PRE_PADDING + strlen(replystr.c_str()) + LWS_SEND_BUFFER_POST_PADDING];
		new_response+=LWS_SEND_BUFFER_PRE_PADDING;
		strcpy(new_response,replystr.c_str());
		libwebsocket_write(socket, (unsigned char*)new_response, strlen(new_response), LWS_WRITE_TEXT);

		//TODO: run valgrind on this. libwebsocket's documentation says NOTHING about this, yet malloc insists it's true.
		//delete new_response; <- Unneeded. Apparently libwebsocket free's it.
		delete (char*)(new_response-LWS_SEND_BUFFER_PRE_PADDING); //Needs to subtract pre-padding, to get back to the start of the pointer.
		delete reply;
	};

	AsyncPropertyReply* reply = routingEngine->getPropertyAsync(velocityRequest);
}

void WebSocketSinkManager::addSingleShotRangedSink(libwebsocket* socket, VehicleProperty::Property property, double start, double end, double seqstart,double seqend, string id)
{
	AsyncRangePropertyRequest rangedRequest;

	rangedRequest.timeBegin = start;
	rangedRequest.timeEnd = end;
	rangedRequest.sequenceBegin = seqstart;
	rangedRequest.sequenceEnd = seqend;

	PropertyList foo = VehicleProperty::capabilities();
	if (ListPlusPlus<VehicleProperty::Property>(&foo).contains(property))
	{
		rangedRequest.property = property;
	}
	else
	{
		DebugOut(0)<<"websocketsink: Invalid property requested: "<<property;
		return;
	}

	rangedRequest.completed = [socket,id](AsyncRangePropertyReply* reply)
	{
		stringstream s;

		//TODO: Dirty hack hardcoded stuff, jsut to make it work.
		stringstream data;
		data.precision(15);
		data<< "[";
		std::list<AbstractPropertyType*> values = reply->values;
		for(auto itr = values.begin(); itr != values.end(); itr++)
		{
			if(itr != values.begin())
			{
				data<<",";
			}

			data << "{ \"value\" : " << "\"" << (*itr)->toString() << "\", \"timestamp\" : \"" << (*itr)->timestamp << "\", \"sequence\" : \""<<(*itr)->sequence<<"\" }";
		}

		data<<"]";

		s << "{\"type\":\"methodReply\",\"name\":\"getRanged\",\"data\":"<<data.str()<<",\"transactionid\":\"" << id << "\"}";

		string replystr = s.str();
		//printf("Reply: %s\n",replystr.c_str());
		DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Reply:" << replystr << "\n";

		char *new_response = new char[LWS_SEND_BUFFER_PRE_PADDING + strlen(replystr.c_str()) + LWS_SEND_BUFFER_POST_PADDING];
		new_response+=LWS_SEND_BUFFER_PRE_PADDING;
		strcpy(new_response,replystr.c_str());
		libwebsocket_write(socket, (unsigned char*)new_response, strlen(new_response), LWS_WRITE_TEXT);

		//TODO: run valgrind on this. libwebsocket's documentation says NOTHING about this, yet malloc insists it's true.
		//delete new_response; <- Unneeded. Apparently libwebsocket free's it.
		delete (char*)(new_response-LWS_SEND_BUFFER_PRE_PADDING); //Needs to subtract pre-padding, to get back to the start of the pointer.
		delete reply;
	};

	AsyncRangePropertyReply* reply = routingEngine->getRangePropertyAsync(rangedRequest);
}

void WebSocketSinkManager::removeSink(libwebsocket* socket,VehicleProperty::Property property, string uuid)
{
	if (m_sinkMap.find(property) != m_sinkMap.end())
	{
		list<WebSocketSink*> sinks = m_sinkMap[property];

		for(auto i = sinks.begin(); i != sinks.end(); i++)
		{
			delete *i;
		}

		m_sinkMap.erase(property);

		stringstream s;
		s << "{\"type\":\"methodReply\",\"name\":\"unsubscribe\",\"data\":[\"" << property << "\"],\"transactionid\":\"" << uuid << "\"}";

		string replystr = s.str();
		//printf("Reply: %s\n",replystr.c_str());
		DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Reply:" << replystr << "\n";

		char *new_response = new char[LWS_SEND_BUFFER_PRE_PADDING + strlen(replystr.c_str()) + LWS_SEND_BUFFER_POST_PADDING];
		new_response+=LWS_SEND_BUFFER_PRE_PADDING;
		strcpy(new_response,replystr.c_str());
		libwebsocket_write(socket, (unsigned char*)new_response, strlen(new_response), LWS_WRITE_TEXT);
		delete (char*)(new_response-LWS_SEND_BUFFER_PRE_PADDING);
	}
}
void WebSocketSinkManager::setValue(string property,string value)
{
	AbstractPropertyType* type = VehicleProperty::getPropertyTypeForPropertyNameValue(property,value);

	AsyncSetPropertyRequest request;
	request.property = property;
	request.value = type;
	request.completed = [](AsyncPropertyReply* reply)
	{
		///TODO: do something here on !reply->success
		delete reply;
	};

	m_engine->setProperty(request);
	DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "AbstractRoutingEngine::setProperty called with arguments:" << property << value << "\n";
	delete type;

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
	else if (property == "running_status_steering_wheel_angle")
	{
		tmpstr = VehicleProperty::SteeringWheelAngle;
	}
	else if (property == "running_status_transmission_gear_status")
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
	//printf("Reply: %s\n",replystr.c_str());
	DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Reply:" << replystr << "\n";

	char *new_response = new char[LWS_SEND_BUFFER_PRE_PADDING + strlen(replystr.c_str()) + LWS_SEND_BUFFER_POST_PADDING];
	new_response+=LWS_SEND_BUFFER_PRE_PADDING;
	strcpy(new_response,replystr.c_str());
	libwebsocket_write(socket, (unsigned char*)new_response, strlen(new_response), LWS_WRITE_TEXT);
	delete (char*)(new_response-LWS_SEND_BUFFER_PRE_PADDING);
	WebSocketSink *sink = new WebSocketSink(m_engine,socket,uuid,property,tmpstr);
	m_sinkMap[property].push_back(sink);
}
extern "C" AbstractSinkManager * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	sinkManager = new WebSocketSinkManager(routingengine, config);
	sinkManager->init();
	return sinkManager;
}
void WebSocketSinkManager::disconnectAll(libwebsocket* socket)
{
	std::list<WebSocketSink*> toDeleteList;

	for (auto i=m_sinkMap.begin(); i != m_sinkMap.end();i++)
	{
		std::list<WebSocketSink*> *sinks = & (*i).second;
		for (auto sinkItr =  sinks->begin(); sinkItr != sinks->end(); sinkItr++)
		{
			if ((*sinkItr)->socket() == socket)
			{
				//This is the sink in question.
				WebSocketSink* sink = (*sinkItr);
				if(!ListPlusPlus<WebSocketSink*>(&toDeleteList).contains(sink))
				{
					toDeleteList.push_back(sink);
				}

				sinks->erase(sinkItr);
				sinkItr = sinks->begin();
				DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Sink removed"<<endl;
			}
		}
	}

	for(auto i=toDeleteList.begin();i!=toDeleteList.end();i++)
	{
		delete *i;
	}
}
void WebSocketSinkManager::addPoll(int fd)
{
	GIOChannel *chan = g_io_channel_unix_new(fd);
	guint sourceid = g_io_add_watch(chan, GIOCondition(G_IO_IN | G_IO_HUP | G_IO_ERR),(GIOFunc)gioPollingFunc,chan);
	g_io_channel_set_close_on_unref(chan,true);
	g_io_channel_unref(chan); //Pass ownership of the GIOChannel to the watch.
	m_ioChannelMap[fd] = chan;
	m_ioSourceMap[fd] = sourceid;
}
void WebSocketSinkManager::removePoll(int fd)
{
	g_io_channel_shutdown(m_ioChannelMap[fd],false,0);
	//printf("Shutting down IO Channel\n");
	DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Shutting down IO Channel\n";
	g_source_remove(m_ioSourceMap[fd]); //Since the watch owns the GIOChannel, this should unref it enough to dissapear.

	//for (map<int,guint>::const_iterator i=m_ioSourceMap.cbegin();i!=m_ioSourceMap.cend();i++)
	for (map<int,guint>::iterator i=m_ioSourceMap.begin();i!=m_ioSourceMap.end();i++)
	{
		if((*i).first == fd)
		{
			//printf("Erasing source\n");
			DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Erasing source\n";
			m_ioSourceMap.erase(i);
			i--;
			if (m_ioSourceMap.size() == 0)
			{
				break;
			}
		}
	}
	//for (map<int,GIOChannel*>::const_iterator i=m_ioChannelMap.cbegin();i!=m_ioChannelMap.cend();i++)
	for (map<int,GIOChannel*>::iterator i=m_ioChannelMap.begin();i!=m_ioChannelMap.end();i++)
	{
		if((*i).first == fd)
		{
			//printf("Erasing channel\n");
			DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Erasing channel\n";
			m_ioChannelMap.erase(i);
			i--;
			if (m_ioChannelMap.size() == 0)
			{
				break;
			}
		}
	}
}

static int websocket_callback(struct libwebsocket_context *context,struct libwebsocket *wsi,enum libwebsocket_callback_reasons reason, void *user,void *in, size_t len)
{
	//printf("Switch: %i\n",reason);
	DebugOut(5) << __SMALLFILE__ << ":" << __LINE__ << "websocket_callback:" << reason << endl;


	switch (reason)
	{
		case LWS_CALLBACK_CLIENT_WRITEABLE:
		{
			//Connection has been established.
			//printf("Connection established\n");
			break;
		}
		case LWS_CALLBACK_CLOSED:
		{
			//Connection is closed, we need to remove all related sinks
			sinkManager->disconnectAll(wsi);
			/*g_io_
			GIOChannel *chan = g_io_channel_unix_new((int)(long)user);
			g_io_add_watch(chan,G_IO_IN,(GIOFunc)gioPollingFunc,0);
			g_io_add_watch(chan,G_IO_PRI,(GIOFunc)gioPollingFunc,0);
			pollfds[count_pollfds].fd = (int)(long)user;
			pollfds[count_pollfds].events = (int)len;
// 			pollfds[count_pollfds++].revents = 0;*/
			break;
		}
		case LWS_CALLBACK_CLIENT_RECEIVE:
		{
			//printf("Client writable\n");
			break;
		}
		case LWS_CALLBACK_SERVER_WRITEABLE:
		{
			//printf("Server writable\n");
			break;
		}

		case LWS_CALLBACK_RECEIVE:
		{
			//printf("Data Received: %s\n",(char*)in);
			//The lack of a break; here is intentional.
		}
		case LWS_CALLBACK_HTTP:
		{
			//TODO: Verify that ALL requests get sent via LWS_CALLBACK_HTTP, so we can use that instead of LWS_CALLBACK_RECIEVE
			//TODO: Do we want exceptions, or just to return an invalid json reply? Probably an invalid json reply.
			DebugOut() << __SMALLFILE__ << ":" << __LINE__ << " Requested: " << (char*)in << "\n";

			std::string tempInput((char*)in);

			json_object *rootobject;
			json_tokener *tokener = json_tokener_new();
			enum json_tokener_error err;
			do
			{
				rootobject = json_tokener_parse_ex(tokener, tempInput.c_str(),len);
			} while ((err = json_tokener_get_error(tokener)) == json_tokener_continue);
			if (err != json_tokener_success)
			{
				fprintf(stderr, "Error: %s\n", json_tokener_error_desc(err));
				throw std::runtime_error("JSON Parsing error");
				// Handle errors, as appropriate for your application.
			}
			if(!rootobject)
			{
				DebugOut(0)<<"failed to parse json: "<<tempInput<<endl;
			}

			if (tokener->char_offset < len) // XXX shouldn't access internal fields
			{
				// Handle extra characters after parsed object as desired.
				// e.g. issue an error, parse another object from that point, etc...

			}
			// Success, use jobj here.
			json_object *typeobject = json_object_object_get(rootobject,"type");
			json_object *nameobject = json_object_object_get(rootobject,"name");
			json_object *transidobject = json_object_object_get(rootobject,"transactionid");
			
			string type = string(json_object_get_string(typeobject));
			string name = string(json_object_get_string(nameobject));
			string id;
			if (json_object_get_type(transidobject) == json_type_string)
			{
				id = string(json_object_get_string(transidobject));
			}
			else
			{
				stringstream strstr;
				strstr << json_object_get_int(transidobject);
				id = strstr.str();
			}
			json_object_put(typeobject);
			json_object_put(nameobject);
			json_object_put(transidobject);
			if (type == "method" && name == "getRanged")
			{
				json_object *dataobject = json_object_object_get(rootobject,"data");
				if (json_object_get_type(dataobject) == json_type_object)
				{
					json_object *timeBeginObject = json_object_object_get(dataobject,"timeBegin");
					json_object *timeEndObject = json_object_object_get(dataobject,"timeEnd");
					json_object *sequenceBeginObject = json_object_object_get(dataobject,"sequenceBegin");
					json_object *sequenceEndObject = json_object_object_get(dataobject,"sequenceEnd");
					json_object *propertyObject = json_object_object_get(dataobject,"property");
					double timeBegin = boost::lexical_cast<double,std::string>(json_object_get_string(timeBeginObject));
					double timeEnd = boost::lexical_cast<double,std::string>(json_object_get_string(timeEndObject));
					double sequenceBegin = boost::lexical_cast<double,std::string>(json_object_get_string(sequenceBeginObject));
					double sequenceEnd = boost::lexical_cast<double,std::string>(json_object_get_string(sequenceEndObject));
					string property = string(json_object_get_string(propertyObject));
					json_object_put(timeBeginObject);
					json_object_put(timeEndObject);
					json_object_put(sequenceBeginObject);
					json_object_put(sequenceEndObject);
					json_object_put(propertyObject);
					if ((timeBegin < 0 && timeEnd > 0) || (timeBegin > 0 && timeEnd < 0))
					{
						//Invalid time begin/end pair
					}
					if ((sequenceBegin < 0 && sequenceEnd > 0) || (sequenceBegin > 0 && sequenceEnd < 0))
					{
						//Invalid sequence begin/end pair
					}
					sinkManager->addSingleShotRangedSink(wsi,property,timeBegin,timeEnd,sequenceBegin,sequenceEnd,id);
				}
			}
			else
			{

				vector<string> data;
				list<string> key;
				list<string> value;
				json_object *dataobject = json_object_object_get(rootobject,"data");
				if (json_object_get_type(dataobject) == json_type_array)
				{
					array_list *arraylist = json_object_get_array(dataobject);
					for (int i=0;i<array_list_length(arraylist);i++)
					{
						json_object *arrayobject = (json_object*)array_list_get_idx(arraylist,i);
						if (json_object_get_type(arrayobject) == json_type_object)
						{
							json_object *propobject = json_object_object_get(arrayobject,"property");
							json_object *valueobject = json_object_object_get(arrayobject,"value");
							string keystr = string(json_object_get_string(propobject));
							string valuestr = string(json_object_get_string(valueobject));
							key.push_back(keystr);
							value.push_back(valuestr);
							json_object_put(propobject);
							json_object_put(valueobject);
						}
						else if (json_object_get_type(arrayobject) == json_type_string)
						{
							string path = string(json_object_get_string(arrayobject));
							data.push_back(path);
						}
					}
					//array_list_free(arraylist);
				}
				else
				{
					string path = json_object_get_string(dataobject);
					if (path != "")
					{
						data.push_back(path);
					}
				}
				json_object_put(dataobject);
				if (type == "method")
				{
					if (name == "get")
					{
						if (data.size() > 0)
						{
							//GetProperty is going to be a singleshot sink.
							//string arg = arguments.front();
							sinkManager->addSingleShotSink(wsi,data.front(),id);
						}
						else
						{
							DebugOut() << __SMALLFILE__ << ":" << __LINE__ << " \"get\" method called with no data! Transaction ID:" << id << "\n";
						}
					}
					else if (name == "set")
					{
						if (data.size() > 0)
						{
						  //Should not happen
						}
						else if (value.size() > 0)
						{
							if (key.size() != value.size())
							{
								DebugOut() << __SMALLFILE__ << ":" << __LINE__ << "\"set\" method called with an invalid key value pair count\n";
							}
							else
							{
								list<string>::iterator d = value.begin();
								for (list<string>::iterator i=key.begin();i!=key.end();i++)
								{
									DebugOut() << __SMALLFILE__ << ":" << __LINE__ << "websocketsinkmanager setting" << (*i) << "to" << (*d) << "\n";
									//(*i);
									sinkManager->setValue((*i),(*d));
									//(*d);
									d++;
								}

							}
						}
					}
					else if (name == "subscribe")
					{
						//Websocket wants to subscribe to an event, data.front();
						for (auto i=data.begin();i!=data.end();i++)
						{
							sinkManager->addSink(wsi,(*i),id);
						}
					}
					else if (name == "unsubscribe")
					{
						//Websocket wants to unsubscribe to an event, data.front();
						for (auto i=data.begin();i!=data.end();i++)
						{
							sinkManager->removeSink(wsi,(*i),id);
						}
					}
					else if (name == "getSupportedEventTypes")
					{
						//If data.front() dosen't contain a property name, return a list of properties supported.
						//if it does, then return the event types that particular property supports.
						string typessupported = "";
						if (data.size() == 0)
						{
							//Send what properties we support
							typessupported = "\"running_status_speedometer\",\"running_status_engine_speed\",\"running_status_steering_wheel_angle\",\"running_status_transmission_gear_status\"";
							
							PropertyList foo = sinkManager->getSupportedProperties();
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
								PropertyList foo = sinkManager->getSupportedProperties();
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
						DebugOut() << __SMALLFILE__ << ":" << __LINE__ << " JSON Reply: " << replystr << "\n";
						//printf("Reply: %s\n",replystr.c_str());
						char *new_response = new char[LWS_SEND_BUFFER_PRE_PADDING + strlen(replystr.c_str()) + LWS_SEND_BUFFER_POST_PADDING];
						new_response+=LWS_SEND_BUFFER_PRE_PADDING;
						strcpy(new_response,replystr.c_str());
						libwebsocket_write(wsi, (unsigned char*)new_response, strlen(new_response), LWS_WRITE_TEXT);
						delete (char*)(new_response-LWS_SEND_BUFFER_PRE_PADDING);
					}
					else
					{
						DebugOut(0)<<"Unknown method called."<<endl;
					}
				}
			}

			

			
			break;
		}
		case LWS_CALLBACK_ADD_POLL_FD:
		{
			//printf("Adding poll %i\n",sinkManager);
			DebugOut(5) << __SMALLFILE__ <<":"<< __LINE__ << "Adding poll" << endl;
			if (sinkManager != 0)
			{
				//sinkManager->addPoll((int)(long)user);
				sinkManager->addPoll(libwebsocket_get_socket_fd(wsi));
			}
			else
			{
				DebugOut(5) << "Error, invalid sink manager!!" << endl;
			}
			break;
		}
		case LWS_CALLBACK_DEL_POLL_FD:
		{
			sinkManager->removePoll(libwebsocket_get_socket_fd(wsi));
			break;
		}
		case LWS_CALLBACK_SET_MODE_POLL_FD:
		{
			//Set the poll mode
			break;
		}
		case LWS_CALLBACK_CLEAR_MODE_POLL_FD:
		{
			//Don't handle this yet.
			break;
		}
		default:
		{
			//printf("Unhandled callback: %i\n",reason);
			DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Unhandled callback:" << reason << "\n";
			break;
		}
	}
	return 0; 
}

bool gioPollingFunc(GIOChannel *source, GIOCondition condition,gpointer data)
{
	DebugOut(5) << "Polling..." << condition << endl;

	if(condition & G_IO_ERR)
	{
		DebugOut(0)<< __SMALLFILE__ <<":"<< __LINE__ <<" websocketsink polling error."<<endl;
	}

	if (condition & G_IO_HUP)
	{
		//Hang up. Returning false closes out the GIOChannel.
		//printf("Callback on G_IO_HUP\n");
		DebugOut(0)<<"socket hangup event..."<<endl;
		return false;
	}

	//This is the polling function. If it return false, glib will stop polling this FD.
	//printf("Polling...%i\n",condition);
	
	lws_tokens token;
	struct pollfd pollstruct;
	int newfd = g_io_channel_unix_get_fd(source);
	pollstruct.fd = newfd;
	pollstruct.events = condition;
	pollstruct.revents = condition;
	libwebsocket_service_fd(context,&pollstruct);

	return true;
}
