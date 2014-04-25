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
#include "common.h"

#include <sstream>
#include <json/json.h>
#include <json/json_object.h>
#include <json/json_tokener.h>
#include <listplusplus.h>
#include <memory>

#include <QVariantMap>
#include <QJsonDocument>
#include <QStringList>
#include <QByteArray>

#define __SMALLFILE__ std::string(__FILE__).substr(std::string(__FILE__).rfind("/")+1)

//Global variables, these will be moved into the class
struct pollfd pollfds[100];
int count_pollfds = 0;
libwebsocket_context *context;
WebSocketSinkManager *sinkManager;
static int websocket_callback(struct libwebsocket_context *context,struct libwebsocket *wsi,enum libwebsocket_callback_reasons reason, void *user,void *in, size_t len);
bool gioPollingFunc(GIOChannel *source,GIOCondition condition,gpointer data);

WebSocketSinkManager::WebSocketSinkManager(AbstractRoutingEngine* engine, map<string, string> config)
	:AbstractSinkManager(engine, config), partialMessageIndex(0), expectedMessageFrames(0)
{
	m_engine = engine;


	if(config.find("binaryProtocol") != config.end())
	{
		doBinary = config["binaryProtocol"] == "true";
	}

	//Create a listening socket on port 23000 on localhost.


}
void WebSocketSinkManager::init()
{
	//Protocol list for libwebsockets.
	protocollist[0] = { "http-only", websocket_callback, 0 };
	protocollist[1] = { NULL, NULL, 0 };


	setConfiguration(configuration);
}
PropertyList WebSocketSinkManager::getSupportedProperties()
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
	info.extensions = nullptr;

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
		if ((*i).first == "useExtensions")
		{
			{
				if((*i).second == "true")
				{
					info.extensions = libwebsocket_get_internal_extensions();
				}
				else info.extensions = nullptr;
			}
		}
	}
	info.iface = interface.c_str();
	info.protocols = protocollist;
	info.gid = -1;
	info.uid = -1;
	info.options = options;
	info.port = port;
	info.user = this;
	if (ssl)
	{
		info.ssl_cert_filepath = ssl_cert_path.c_str();
		info.ssl_private_key_filepath = ssl_key_path.c_str();
	}
	context = libwebsocket_create_context(&info);
	
}

void WebSocketSinkManager::addSingleShotSink(libwebsocket* socket, VehicleProperty::Property property, Zone::Type zone, string id)
{
	AsyncPropertyRequest request;
	PropertyList foo = VehicleProperty::capabilities();
	if (contains(foo,property))
	{
		request.property = property;
	}
	else
	{
		DebugOut(0)<<"websocketsink: Invalid property requested: "<<property;
		return;
	}

	request.zoneFilter = zone;
	request.completed = [socket,id,property](AsyncPropertyReply* reply)
	{
		DebugOut()<<"Got property: "<<reply->property.c_str()<<endl;
		if(!reply->success || !reply->value)
		{
			DebugOut()<<"Property value is null"<<endl;
			delete reply;
			return;
		}

		QVariantMap data;
		data["property"] = property.c_str();
		data["zone"] = reply->value->zone;
		data["value"] = reply->value->toString().c_str();
		data["timestamp"] = reply->value->timestamp;
		data["sequence"] = reply->value->sequence;

		QVariantMap replyvar;

		replyvar["type"]="methodReply";
		replyvar["name"]="get";
		replyvar["data"]= data;
		replyvar["transactionid"]=id.c_str();

		QByteArray replystr;

		if(doBinary)
			replystr = QJsonDocument::fromVariant(replyvar).toBinaryData();
		else
		{
			replystr = QJsonDocument::fromVariant(replyvar).toJson();
			cleanJson(replystr);
		}

		lwsWrite(socket, replystr, replystr.length());

		delete reply;
	};

	AsyncPropertyReply* reply = routingEngine->getPropertyAsync(request);
}

void WebSocketSinkManager::addSingleShotRangedSink(libwebsocket* socket, PropertyList properties, double start, double end, double seqstart,double seqend, string id)
{
	AsyncRangePropertyRequest rangedRequest;

	rangedRequest.timeBegin = start;
	rangedRequest.timeEnd = end;
	rangedRequest.sequenceBegin = seqstart;
	rangedRequest.sequenceEnd = seqend;

	rangedRequest.completed = [socket,id](AsyncRangePropertyReply* reply)
	{
		QVariantMap replyvar;
		QVariantList list;

		std::list<AbstractPropertyType*> values = reply->values;
		for(auto itr = values.begin(); itr != values.end(); itr++)
		{
			QVariantMap obj;
			obj["value"]= (*itr)->toString().c_str();
			obj["timestamp"] = (*itr)->timestamp;
			obj["sequence"] = (*itr)->sequence;

			list.append(obj);
		}

		replyvar["type"]="methodReply";
		replyvar["name"]="getRanged";
		replyvar["data"]=list;
		replyvar["transactionid"]=id.c_str();

		QByteArray replystr;

		if(doBinary)
			replystr = QJsonDocument::fromVariant(replyvar).toBinaryData();
		else
		{
			replystr = QJsonDocument::fromVariant(replyvar).toJson();
			cleanJson(replystr);
		}

		lwsWrite(socket, replystr, replystr.length());

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

		QVariantMap reply;
		reply["type"]="methodReply";
		reply["name"]="unsubscribe";
		reply["data"]=property.c_str();
		reply["transactionid"]= uuid.c_str();

		QByteArray replystr;

		if(doBinary)
			replystr = QJsonDocument::fromVariant(reply).toBinaryData();
		else
		{
			replystr = QJsonDocument::fromVariant(reply).toJson();
			cleanJson(replystr);
		}

		lwsWrite(socket, replystr, replystr.length());
	}
}
void WebSocketSinkManager::setValue(libwebsocket* socket,VehicleProperty::Property property,string value,Zone::Type zone,string uuid)
{
	AbstractPropertyType* type = VehicleProperty::getPropertyTypeForPropertyNameValue(property,value);

	AsyncSetPropertyRequest request;
	request.property = property;
	request.value = type;
	request.zoneFilter = zone;
	request.completed = [&](AsyncPropertyReply* reply)
	{
		QVariantMap data;
		data["property"] = property.c_str();
		data["zone"] = zone;
		data["source"] = reply->value->sourceUuid.c_str();

		QVariantMap replyvar;
		replyvar["type"]="methodReply";
		replyvar["name"]="set";
		replyvar["data"]= data;
		replyvar["transactionid"]=uuid.c_str();

		QByteArray replystr;

		if(doBinary)
			replystr = QJsonDocument::fromVariant(replyvar).toBinaryData();
		else
		{
			replystr = QJsonDocument::fromVariant(replyvar).toJson();
			cleanJson(replystr);
		}

		lwsWrite(socket, replystr, replystr.length());

		delete reply;
	};

	m_engine->setProperty(request);
	DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "AbstractRoutingEngine::setProperty called with arguments:" << property << value << "\n";
	delete type;

}
void WebSocketSinkManager::addSink(libwebsocket* socket, VehicleProperty::Property property,string uuid)
{
	PropertyList foo = VehicleProperty::capabilities();
	if (!contains(foo,property))
	{
		DebugOut(DebugOut::Warning)<<"Invalid property requested: "<<property<<endl;
		return;
	}

	QVariantMap reply;

	reply["type"] = "methodReply";
	reply["name"] = "subscribe";
	reply["data"] = property.c_str();
	reply["transactionid"] = uuid.c_str();

	QByteArray replystr;

	if(doBinary)
		replystr = QJsonDocument::fromVariant(reply).toBinaryData();
	else
	{
		replystr = QJsonDocument::fromVariant(reply).toJson();
		cleanJson(replystr);
	}

	lwsWrite(socket, replystr, replystr.length());

	WebSocketSink *sink = new WebSocketSink(m_engine,socket,uuid,property,property);
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
		for (auto sinkItr = sinks->begin(); sinkItr != sinks->end(); sinkItr++)
		{
			if ((*sinkItr)->socket() == socket)
			{
				//This is the sink in question.
				WebSocketSink* sink = (*sinkItr);
				if(!contains(toDeleteList, sink))
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
			break;
		}
		case LWS_CALLBACK_CLOSED:
		{
			sinkManager->disconnectAll(wsi);
			break;
		}
		case LWS_CALLBACK_CLIENT_RECEIVE:
		{
			break;
		}
		case LWS_CALLBACK_SERVER_WRITEABLE:
		{
			break;
		}

		case LWS_CALLBACK_RECEIVE:
		{

		}
		case LWS_CALLBACK_HTTP:
		{
			//TODO: Verify that ALL requests get sent via LWS_CALLBACK_HTTP, so we can use that instead of LWS_CALLBACK_RECIEVE
			//TODO: Do we want exceptions, or just to return an invalid json reply? Probably an invalid json reply.
			DebugOut() << __SMALLFILE__ << ":" << __LINE__ << " Requested: " << (char*)in << "\n";

			QByteArray d((char*)in,len);

			WebSocketSinkManager * manager = sinkManager;

			if(manager->expectedMessageFrames && manager->partialMessageIndex < manager->expectedMessageFrames)
			{
				manager->incompleteMessage += d;
				manager->partialMessageIndex++;
				break;
			}
			else if(manager->expectedMessageFrames && manager->partialMessageIndex == manager->expectedMessageFrames)
			{
				d = manager->incompleteMessage + d;
				manager->expectedMessageFrames = 0;
			}

			QJsonDocument doc;
			if(doBinary)
				doc = QJsonDocument::fromBinaryData(d);
			else
				doc = QJsonDocument::fromJson(d);

			if(doc.isNull())
			{
				DebugOut(DebugOut::Error)<<"Invalid message"<<endl;
				return 0;
			}

			QVariantMap call = doc.toVariant().toMap();

			string type = call["type"].toString().toStdString();
			string name = call["name"].toString().toStdString();
			string id = call["transactionid"].toString().toStdString();

			if (type == "multiframe")
			{

				manager->expectedMessageFrames = call["frames"].toInt();
				manager->partialMessageIndex = 1;
				manager->incompleteMessage = "";
			}
			else if (type == "method")
			{
				if(name == "getRanged")
				{
					QVariantMap data = call["data"].toMap();

					PropertyList propertyList;

					propertyList.push_back(data["property"].toString().toStdString());

					double timeBegin = data["timeBegin"].toDouble();
					double timeEnd = data["timeEnd"].toDouble();
					double sequenceBegin = data["sequenceBegin"].toInt();
					double sequenceEnd = data["sequenceEnd"].toInt();

					if ((timeBegin < 0 && timeEnd > 0) || (timeBegin > 0 && timeEnd < 0))
					{
						DebugOut(DebugOut::Warning)<<"Invalid time begin/end pair"<<endl;
					}
					else if ((sequenceBegin < 0 && sequenceEnd > 0) || (sequenceBegin > 0 && sequenceEnd < 0))
					{
						DebugOut(DebugOut::Warning)<<"Invalid sequence begin/end pair"<<endl;
					}
					else
					{
						sinkManager->addSingleShotRangedSink(wsi,propertyList,timeBegin,timeEnd,sequenceBegin,sequenceEnd,id);
					}
				}
				else if (name == "get")
				{
					QVariantMap data = call["data"].toMap();
					Zone::Type zone = Zone::None;
					if(data.contains("zone"))
					{
						zone = data["zone"].toInt();
					}
					sinkManager->addSingleShotSink(wsi,data["property"].toString().toStdString(),zone,id);

				}
				else if (name == "set")
				{
					QVariantMap data = call["data"].toMap();
					Zone::Type zone(Zone::None);
					if(data.contains("zone"))
					{
						zone = data["zone"].toInt();
					}
					sinkManager->setValue(wsi,data["property"].toString().toStdString(), data["value"].toString().toStdString(), zone, id);
				}
				else if (name == "subscribe")
				{
					std::string data = call["data"].toString().toStdString();
					sinkManager->addSink(wsi, data, id);

				}
				else if (name == "unsubscribe")
				{
					std::string data = call["data"].toString().toStdString();
					sinkManager->removeSink(wsi,data,id);

				}
				else if (name == "getSupportedEventTypes")
				{
					QVariantMap reply;
					QStringList list;

					PropertyList supported = sinkManager->getSupportedProperties();
					for(VehicleProperty::Property i : supported)
					{
						list.append(i.c_str());
					}

					reply["type"] = "methodReply";
					reply["name"] = "getSupportedEventTypes";
					reply["transactionid"] = id.c_str();
					reply["data"] = list;

					QByteArray replystr;

					if(doBinary)
						replystr = QJsonDocument::fromVariant(reply).toBinaryData();
					else
					{
						replystr = QJsonDocument::fromVariant(reply).toJson();
						cleanJson(replystr);
					}

					lwsWrite(wsi, replystr, replystr.length());
				}
				else
				{
					DebugOut(0)<<"Unknown method called."<<endl;
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

bool gioPollingFunc(GIOChannel *source, GIOCondition condition, gpointer data)
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
