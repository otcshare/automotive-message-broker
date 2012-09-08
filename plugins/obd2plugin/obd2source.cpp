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


#include "obd2source.h"
#include <iostream>
#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <glib.h>
#include <sstream>
#include <json-glib/json-glib.h>
#include <listplusplus.h>
#include "debugout.h"
#define __SMALLFILE__ std::string(__FILE__).substr(std::string(__FILE__).rfind("/")+1)
AbstractRoutingEngine *m_re;

bool sendElmCommand(obdLib *obd,std::string command)
{
  	std::vector<unsigned char> replyVector;
	std::string reply;
	obd->sendObdRequestString(command.append("\r").c_str(),command.length()+1,&replyVector,500,3);
	for (unsigned int i=0;i<replyVector.size();i++)
	{
		reply += replyVector[i];
	}
	if (reply.find("OK") == -1)
	{
		//No OK reply found
		return false;
	}
	else
	{
		return true;
	}

}
void threadLoop(gpointer data)
{
	GAsyncQueue *privCommandQueue = g_async_queue_ref(((OBD2Source*)data)->commandQueue);
	GAsyncQueue *privResponseQueue = g_async_queue_ref(((OBD2Source*)data)->responseQueue);
	GAsyncQueue *privSingleShotQueue = g_async_queue_ref(((OBD2Source*)data)->singleShotQueue);
	GAsyncQueue *privSubscriptionAddQueue = g_async_queue_ref(((OBD2Source*)data)->subscriptionAddQueue);
	GAsyncQueue *privSubscriptionRemoveQueue = g_async_queue_ref(((OBD2Source*)data)->subscriptionRemoveQueue);
	obdLib *obd = new obdLib();
	
	std::list<std::string> reqList;
	std::list<std::string> repeatReqList;
	std::map<std::string,std::string> commandMap;
	std::vector<unsigned char> replyVector;
	std::string reply;
	while (true)
	{
		//gpointer query = g_async_queue_pop(privCommandQueue);
		
		
		gpointer query = g_async_queue_try_pop(privSingleShotQueue);
		if (query != nullptr)
		{
			printf("Got request!\n");
			ObdRequest *req = (ObdRequest*)query;
			reqList.push_back(req->req);
			delete req;
		}
		query = g_async_queue_try_pop(privSubscriptionAddQueue);
		if (query != nullptr)
		{
			printf("Got request!\n");
			ObdRequest *req = (ObdRequest*)query;
			repeatReqList.push_back(req->req);
			delete req;
		}
		query = g_async_queue_try_pop(privCommandQueue);
		if (query != nullptr)
		{
			printf("Got Command!\n");
			ObdRequest *req = (ObdRequest*)query;
			//commandMap[req->req] = req->arg;
			if (req->req == "connect")
			{
				std::string port = req->arg;
				obd->openPort(port.c_str(),115200);
				
				obd->sendObdRequestString("ATZ\r",4,&replyVector,500,3);
				for (unsigned int i=0;i<replyVector.size();i++)
				{
					reply += replyVector[i];
				}
				if (reply.find("ELM") == -1)
				{
					//No reply found
					printf("Error!\n");
				}
				else
				{
					printf("Reply to reset: %s\n",reply.c_str());
				}
				if (!sendElmCommand(obd,"ATE0"))
				{
				  printf("Error sending echo\n");
				}
				if (!sendElmCommand(obd,"ATH0"))
				{
				  printf("Error sending headers off\n");
				}
				if (!sendElmCommand(obd,"ATL0"))
				{
				  printf("Error turning linefeeds off\n");
				}
			}
			delete req;
		}
		for (std::list<std::string>::const_iterator i=reqList.cbegin();i!= reqList.cend();i++)
		{
			repeatReqList.push_back(*i);
		}
		for (std::list<std::string>::const_iterator i=repeatReqList.cbegin();i!= repeatReqList.cend();i++)
		{
			if (!obd->sendObdRequest((*i).c_str(),(*i).length(),&replyVector))
			{
				printf("Error sending obd2 request\n");
				continue;
			}
			if (replyVector[0] == 0x41)
			{
				if (replyVector[1] == 0x0C)
				{
					double rpm = ((replyVector[2] << 8) + replyVector[3]) / 4.0;
					ObdReply *rep = new ObdReply();
					rep->req = "0C";
					rep->reply = boost::lexical_cast<string>(rpm);
					g_async_queue_push(privResponseQueue,rep);
					//printf("RPM: %f\n",rpm);
				}
				else if (replyVector[1] == 0x0D)
				{
				      int mph = replyVector[2];
				      ObdReply *rep = new ObdReply();
				      rep->req = "0D";
				      rep->reply = boost::lexical_cast<string>(mph);
				      g_async_queue_push(privResponseQueue,rep);
				}
				else
				{
					printf("Unknown response type: %i\n",replyVector[1]);
				}
			}
			
			//printf("Reply: %i %i\n",replyVector[2],replyVector[3]);
		}
		usleep(10000);
	}
	
}
static gboolean updateProperties(gpointer data)
{
	OBD2Source* src = (OBD2Source*)data;
	
	//src->randomizeProperties();
	gpointer retval = g_async_queue_try_pop(src->responseQueue);
	if (retval != nullptr)
	{
		ObdReply *reply = (ObdReply*)retval;
		if (reply->req == "0C")
		{
			src->engineSpeed(boost::lexical_cast<float>(reply->reply));
		}
		else if (reply->req == "0D")
		{
			src->vehicleSpeed(boost::lexical_cast<int>(reply->reply));
		}
	}
	return true;
}

void OBD2Source::engineSpeed(double speed)
{
	VehicleProperty::VehicleSpeedType espeed(speed);
	m_re->updateProperty(VehicleProperty::EngineSpeed,&espeed);
}
void OBD2Source::vehicleSpeed(int speed)
{
	VehicleProperty::EngineSpeedType vspeed(speed);
	m_re->updateProperty(VehicleProperty::VehicleSpeed,&vspeed);
}
void OBD2Source::setSupported(PropertyList list)
{
	m_supportedProperties = list;
	m_re->updateSupported(list,PropertyList());
}
void OBD2Source::setConfiguration(map<string, string> config)
{
// 	//Config has been passed, let's start stuff up.
	configuration = config;
	
	//Default values
	std::string port = "/dev/ttyUSB0";
	
	//Try to load config
	printf("OBD2Source::setConfiguration\n");
	for (map<string,string>::const_iterator i=configuration.cbegin();i!=configuration.cend();i++)
	{
		printf("Incoming setting: %s:%s\n",(*i).first.c_str(),(*i).second.c_str());
		if ((*i).first == "port")
		{
			port = (*i).second;
			ObdRequest *requ = new ObdRequest();
			requ->req = "connect";
			requ->arg = port;
			g_async_queue_push(commandQueue,requ);
		}
	}
}
OBD2Source::OBD2Source(AbstractRoutingEngine *re) : AbstractSource(re)
{
	g_timeout_add(250, updateProperties, this );
	clientConnected = false;
	m_re = re;  
	
	//Read JSON that will tell us what to do:
	/*
	GError* error = nullptr;
	JsonParser* parser = json_parser_new();
	if (!json_parser_load_from_file(parser,"obd2source.conf",&error))
	{
		g_error_free(error);
		error = nullptr;
		if (!json_parser_load_from_file(parser,"../../plugins/obd2sourceplugin/obd2source.conf",&error))
		{
			error = nullptr;
			g_error_free(error);
			if (!json_parser_load_from_file(parser,"/etc/ambd/obd2source.conf",&error))
			{
				g_error_free(error);
				error = nullptr;
				DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Error loading JSON";
				return;
			}
		}
	}
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
			
			json_reader_end_element(reader);
		}
	}
	json_reader_end_member(reader);
	*/
	m_supportedProperties.push_back(VehicleProperty::EngineSpeed);
	m_supportedProperties.push_back(VehicleProperty::VehicleSpeed);
	re->setSupported(supported(), this);
	/*if (openPort(std::string("/dev/pts/7"),115200))
	{
	  printf("Error opening OBD2 port\n");
	}*/
	commandQueue = g_async_queue_new();
	subscriptionAddQueue = g_async_queue_new();
	subscriptionRemoveQueue = g_async_queue_new();
	responseQueue = g_async_queue_new();
	singleShotQueue = g_async_queue_new();
	g_thread_new("mythread",(GThreadFunc)&threadLoop,this);
	ObdRequest *requ = new ObdRequest();
	requ->req = "010C\r";
	g_async_queue_push(subscriptionAddQueue,requ);
	
	requ = new ObdRequest();
	requ->req = "010D\r";
	g_async_queue_push(subscriptionAddQueue,requ);
}

PropertyList OBD2Source::supported()
{
	return m_supportedProperties;
}
extern "C" AbstractSource * create(AbstractRoutingEngine* routingengine)
{
	return new OBD2Source(routingengine);
	
}
string OBD2Source::uuid()
{
	return "f77af740-f1f8-11e1-aff1-0800200c9a66";
}
void OBD2Source::subscribeToPropertyChanges(VehicleProperty::Property property)
{
	//printf("Subscribed to property: %s\n",property.c_str());
	queuedRequests.push_back(property);
	if (clientConnected)
	{
		
	}
}


void OBD2Source::unsubscribeToPropertyChanges(VehicleProperty::Property property)
{
	removeRequests.push_back(property);
	if (clientConnected)
	{
		
	}
}


void OBD2Source::getPropertyAsync(AsyncPropertyReply *reply)
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

void OBD2Source::setProperty(VehicleProperty::Property , AbstractPropertyType * )
{

}
