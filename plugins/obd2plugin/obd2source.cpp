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
//#include <json-glib/json-glib.h>
#include <listplusplus.h>
#include "debugout.h"
#include "bluetooth.hpp"
#include "timestamp.h"
#include "bluetooth5.h"
#include "superptr.hpp"

#ifdef USE_BLUEZ5
Bluetooth5 bt;
#endif

#define __SMALLFILE__ std::string(__FILE__).substr(std::string(__FILE__).rfind("/")+1)
AbstractRoutingEngine *m_re;

//std::list<ObdPid*> Obd2Amb::supportedPidsList;
Obd2Amb *obd2AmbInstance = new Obd2Amb;
VehicleProperty::Property Obd2Connected = "Obd2Connected";
int calledPersecond = 0;

bool sendElmCommand(obdLib *obd,std::string command)
{
	std::vector<unsigned char> replyVector;
	std::string reply;
	obd->sendObdRequestString(command.append("\r").c_str(),command.length()+1,&replyVector,10,3);
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

bool beginsWith(std::string a, std::string b)
{
	return (a.compare(0, b.length(), b) == 0);
}

bool connect(obdLib* obd, std::string device, std::string strbaud, int fd = -1)
{
	//printf("First: %s\nSecond: %s\n",req->arg.substr(0,req->arg.find(':')).c_str(),req->arg.substr(req->arg.find(':')+1).c_str());
	std::string port = device;
	DebugOut() << "Obd2Source::Connect()" << device << strbaud << endl;
	int baud = boost::lexical_cast<int>(strbaud);

	if(fd != -1)
	{
		if(obd->openPort(fd, baud) == -1)
			return false;
	}
	else
	{
		if(obd->openPort(port.c_str(),baud) == -1)
			return false;
	}

	ObdPid::ByteArray replyVector;
	std::string reply;
	obd->sendObdRequestString("ATZ\r",4,&replyVector,500,3);
	for (unsigned int i=0;i<replyVector.size();i++)
	{
		reply += replyVector[i];
	}
	if (reply.find("ELM") == -1)
	{
		//No reply found
		//printf("Error!\n");
		DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Error resetting ELM"<<endl;
		return false;
	}
	else
	{
		//printf("Reply to reset: %s\n",reply.c_str());
	}
	if (!sendElmCommand(obd,"ATSP0"))
	{
		//printf("Error sending echo\n");
		DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Error setting auto protocol"<<endl;
		return false;
	}
	if (!sendElmCommand(obd,"ATE0"))
	{
		//printf("Error sending echo\n");
		DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Error turning off echo"<<endl;
		return false;
	}
	if (!sendElmCommand(obd,"ATH0"))
	{
		//printf("Error sending headers off\n");
		DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Error turning off headers"<<endl;
		return false;
	}
	if (!sendElmCommand(obd,"ATL0"))
	{
		//printf("Error turning linefeeds off\n");
		DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Error turning off linefeeds"<<endl;
		return false;
	}
	obd->sendObdRequestString("010C1\r", 6, &replyVector, 500, 5);

	return true;
}

void threadLoop(gpointer data)
{
	GAsyncQueue *privCommandQueue = g_async_queue_ref(((OBD2Source*)data)->commandQueue);
	GAsyncQueue *privResponseQueue = g_async_queue_ref(((OBD2Source*)data)->responseQueue);
	GAsyncQueue *privSingleShotQueue = g_async_queue_ref(((OBD2Source*)data)->singleShotQueue);
	GAsyncQueue *privSubscriptionAddQueue = g_async_queue_ref(((OBD2Source*)data)->subscriptionAddQueue);
	GAsyncQueue *privSubscriptionRemoveQueue = g_async_queue_ref(((OBD2Source*)data)->subscriptionRemoveQueue);
	GAsyncQueue *privStatusQueue = g_async_queue_ref(((OBD2Source*)data)->statusQueue);

	obdLib *obd = new obdLib();
	OBD2Source *source = (OBD2Source*)data;

	obd->setCommsCallback([](const char* mssg, void* data) { DebugOut(6)<<mssg<<endl; },NULL);
	obd->setDebugCallback([](const char* mssg, void* data, obdLib::DebugLevel debugLevel) { DebugOut(debugLevel)<<mssg<<endl; },NULL);

	std::list<ObdPid*> reqList;
	std::list<ObdPid*> repeatReqList;
	ObdPid::ByteArray replyVector;
	std::string port;
	std::string baud;
	bool connected=false;
	int emptycount = 0;
	int timeoutCount = 0;
	while (source->m_threadLive)
	{
		gpointer query = g_async_queue_try_pop(privSingleShotQueue);
		if (query != nullptr)
		{
			//printf("Got request!\n");

			ObdPid *req = (ObdPid*)query;
			DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Got single shot request: " << req->pid.substr(0,req->pid.length()-1) << ":" << req->property <<endl;
			repeatReqList.push_back(req);
		}
		query = g_async_queue_try_pop(privSubscriptionAddQueue);
		if (query != nullptr)
		{

			ObdPid *req = (ObdPid*)query;
			//DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Got subscription request for "<<req->req<<endl;
			reqList.push_back(req);
		}
		query = g_async_queue_try_pop(privCommandQueue);
		if (query != nullptr)
		{
			//ObdPid *req = (ObdPid*)query;
			CommandRequest *req = (CommandRequest*)query;
			//commandMap[req->req] = req->arg;
			//printf("Command: %s\n",req->req.c_str());
			DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Command:" << req->req << endl;
			if (req->req == "connectifnot")
			{
				if (!connected)
				{
					if (source->m_isBluetooth)
					{
#ifdef USE_BLUEZ5
						bt.getDeviceForAddress(source->m_btDeviceAddress, [&obd, baud, &privStatusQueue, &connected](int fd)
						{
							connected = connect(obd, "", baud, fd);

							if(connected)
							{
								StatusMessage *statusreq = new StatusMessage();
								statusreq->statusStr = "connected";
								g_async_queue_push(privStatusQueue, statusreq);
							}
							else
							{
								StatusMessage *statusreq = new StatusMessage();
								statusreq->statusStr = "disconnected";
								g_async_queue_push(privStatusQueue, statusreq);
							}
						});
#else
						BluetoothDevice bt;
						std::string tempPort = bt.getDeviceForAddress(source->m_btDeviceAddress, source->m_btAdapterAddress);
						if(tempPort != "")
						{
							DebugOut(3) << "Using bluetooth device \"" << source->m_btDeviceAddress << "\" bound to: " << tempPort << endl;
							port = tempPort;
						}
						else
						{
							DebugOut(DebugOut::Error) << "Error creating bluetooth device" << endl;
							continue;
						}

						connected = connect(obd, port, baud);

						if(connected)
						{
							StatusMessage *statusreq = new StatusMessage();
							statusreq->statusStr = "connected";
							g_async_queue_push(privStatusQueue, statusreq);
						}
						else
						{
							StatusMessage *statusreq = new StatusMessage();
							statusreq->statusStr = "disconnected";
							g_async_queue_push(privStatusQueue, statusreq);
						}

#endif
					}
					else
					{

						connected = connect(obd, port, baud);

						if(connected)
						{
							StatusMessage *statusreq = new StatusMessage();
							statusreq->statusStr = "connected";
							g_async_queue_push(privStatusQueue, statusreq);
						}
						else
						{
							StatusMessage *statusreq = new StatusMessage();
							statusreq->statusStr = "disconnected";
							g_async_queue_push(privStatusQueue, statusreq);
						}
					}
				}
			}
			else if (req->req == "setportandbaud")
			{
				port = req->arglist[0];
				baud = req->arglist[1];
			}
			else if (req->req == "disconnect")
			{
				DebugOut() << __SMALLFILE__ << ":" << __LINE__ << "Using queued disconnect" << (ulong)req << endl;
				obd->closePort();
#ifdef USE_BLUEZ5
				bt.disconnect();
#else
				BluetoothDevice bt;
				bt.disconnect(source->m_btDeviceAddress, source->m_btAdapterAddress);
#endif
				connected = false;
				StatusMessage *statusreq = new StatusMessage();
				statusreq->statusStr = "disconnected";
				g_async_queue_push(privStatusQueue,statusreq);
			}
			delete req;
		}
		query = g_async_queue_try_pop(privSubscriptionRemoveQueue);
		if (query != nullptr)
		{
			DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Got unsubscription request"<<endl;
			ObdPid *req = (ObdPid*)query;
			for (std::list<ObdPid*>::iterator i=reqList.begin();i!= reqList.end();i++)
			{
				if ((*i)->property == req->property)
				{
					reqList.erase(i);
					delete (*i);
					i--;
					if (reqList.size() == 0)
					{
						break;
					}
				}
			}
			//reqList.push_back(req->req);
			delete req;
		}
		if (reqList.size() > 0 && !connected)
		{
			/*CommandRequest *req = new CommandRequest();
	  req->req = "connect";
	  req->arglist.push_back(port);
	  req->arglist.push_back(baud);
	  g_async_queue_push(privCommandQueue,req);
	  continue;*/
		}
		else if (reqList.size() == 0 && connected)
		{
			emptycount++;
			if (emptycount < 1000)
			{
				usleep(10000);
				continue;
			}
			emptycount = 0;
			CommandRequest *req = new CommandRequest();
			req->req = "disconnect";
			g_async_queue_push(privCommandQueue,req);
			continue;
		}
		if (!connected)
		{
			usleep(10000);
			continue;
		}
		for (std::list<ObdPid*>::iterator i=reqList.begin();i!= reqList.end();i++)
		{
			repeatReqList.push_back(*i);
		}
		int badloop = 0;
		for (std::list<ObdPid*>::iterator i=repeatReqList.begin();i!= repeatReqList.end();i++)
		{
			DebugOut(10) << __SMALLFILE__ << ":" << __LINE__ << "Requesting pid: " << (*i)->pid.substr(0,(*i)->pid.length()-1) << (*i)->property << endl;
			if (source->m_blacklistPidCountMap.find((*i)->pid) != source->m_blacklistPidCountMap.end())
			{
				//Don't erase the pid, just skip over it.
				int count = (*source->m_blacklistPidCountMap.find((*i)->pid)).second;
				if (count > 10)
				{
					continue;
				}
			}
			badloop++;

			bool result = false;

			if(beginsWith((*i)->pid,"AT") || beginsWith((*i)->pid, "ST"))
			{
				result = obd->sendObdRequestString((*i)->pid.c_str(),(*i)->pid.length(),&replyVector);
			}
			else result = obd->sendObdRequestString((*i)->pid.c_str(),(*i)->pid.length(),&replyVector,5,3);

			if (!result)
			{
				//This only happens during a error with the com port. Close it and re-open it later.
				DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Unable to send request:" << (*i)->pid.substr(0,(*i)->pid.length()-1) << endl;
				if (obd->lastError() == obdLib::NODATA)
				{
					DebugOut() << __SMALLFILE__ << ":" << __LINE__ << "OBDLib::NODATA for pid" << (*i)->pid.substr(0,(*i)->pid.length()-1) << " expected property: " << (*i)->property << endl;
					if (source->m_blacklistPidCountMap.find((*i)->pid) != source->m_blacklistPidCountMap.end())
					{
						//pid value i not yet in the list.
						int count = (*source->m_blacklistPidCountMap.find((*i)->pid)).second;
						if (count > 10)
						{

						}
						source->m_blacklistPidCountMap.erase(source->m_blacklistPidCountMap.find((*i)->pid));
						source->m_blacklistPidCountMap.insert(pair<std::string,int>((*i)->pid,count));
					}
					else
					{
						source->m_blacklistPidCountMap.insert(pair<std::string,int>((*i)->pid,1));
					}
					StatusMessage *statusreq = new StatusMessage();
					statusreq->statusStr = "error:nodata";
					statusreq->property = (*i)->property;
					g_async_queue_push(privStatusQueue,statusreq);
					continue;
				}
				else if (obd->lastError() == obdLib::TIMEOUT)
				{
					timeoutCount++;
					if (timeoutCount < 2)
					{
						DebugOut() << __SMALLFILE__ << ":" << __LINE__ << "OBDLib::TIMEOUT for pid" << (*i)->pid << endl;
						StatusMessage *statusreq = new StatusMessage();
						statusreq->statusStr = "error:timeout";
						g_async_queue_push(privStatusQueue,statusreq);
						continue;
					}
				}
				else
				{
					DebugOut() << __SMALLFILE__ << ":" << __LINE__ << "OBD Other error:" << obd->lastError() << endl;
				}

				CommandRequest *req = new CommandRequest();
				DebugOut() << __SMALLFILE__ << ":" << __LINE__ << "Queuing up a disconnect" << (ulong)req << endl;
				req->req = "disconnect";
				g_async_queue_push(privCommandQueue,req);
				i = repeatReqList.end();
				i--;
				continue;
			}
			if (source->m_blacklistPidCountMap.find((*i)->pid) != source->m_blacklistPidCountMap.end())
			{
				//If we get the pid response, then we want to clear out the blacklist list.
				source->m_blacklistPidCountMap.erase(source->m_blacklistPidCountMap.find((*i)->pid));
			}
			timeoutCount = 0;
			//ObdPid *pid = ObdPid::pidFromReply(replyVector);
			ObdPid *pid = obd2AmbInstance->createPidFromReply(replyVector);
			if (!pid)
			{
				//Invalid reply
				DebugOut() << "Invalid reply"<<endl;
				continue;
			}
			else
			{
				DebugOut(11) << __SMALLFILE__ <<":"<< __LINE__ << "Reply recieved and queued for:" << (*i)->pid.substr(0,(*i)->pid.length()-1) << endl;
				std::string repstr;
				for (int i=0;i<replyVector.size();i++)
				{
					if (replyVector[i] != 13)
					{
						repstr += (char)replyVector[i];
					}
					//DebugOut(11) << replyVector[i];
				}
				DebugOut(11) << "Reply:" << repstr << endl;
			}
			g_async_queue_push(privResponseQueue,pid);
		}
		if (badloop == 0)
		{
			//We had zero non-blacklisted events. Pause for a moment here to keep from burning CPU.
			//usleep(10000);
		}
		repeatReqList.clear();

	}
	if (connected)
	{
		obd->closePort();
	}
}
static int updateProperties( gpointer data)
{

	OBD2Source* src = (OBD2Source*)data;

	while (gpointer retval = g_async_queue_try_pop(src->statusQueue))
	{
		StatusMessage *reply = (StatusMessage*)retval;
		if (reply->statusStr == "disconnected")
		{
			src->obd2Connected.setValue(false);
			src->updateProperty(&src->obd2Connected);
		}
		else if (reply->statusStr == "connected")
		{
			src->obd2Connected.setValue(false);
			src->updateProperty(&src->obd2Connected);
		}
		else if (reply->statusStr == "error:nodata" || reply->statusStr == "error:timeout")
		{
			AsyncPropertyReply* srcReply = nullptr;

			for(auto i : src->propertyReplyList)
			{
				if(i->property == reply->property)
				{
					srcReply = i;
					break;
				}
			}

			if (srcReply)
			{
				DebugOut(5) << __SMALLFILE__ <<":"<< __LINE__ << reply->statusStr << " on property:" << reply->property << endl;
				srcReply->success = false;
				srcReply->error = AsyncPropertyReply::InvalidOperation;
				srcReply->completed(srcReply);
				removeOne(&src->propertyReplyList, srcReply);

				/// Remove support for this pid:
				PropertyList list = src->supported();
				removeOne(&list, reply->property);
				src->setSupported(list);
			}
			else
			{
				DebugOut(5) << __SMALLFILE__ <<":"<< __LINE__ << reply->statusStr << " on unrequested property:" << reply->property << endl;
			}
		}
		delete reply;
	}
	while(gpointer retval = g_async_queue_try_pop(src->responseQueue))
	{
		ObdPid *reply = (ObdPid*)retval;

		auto value = amb::make_unique(VehicleProperty::getPropertyTypeForPropertyNameValue(reply->property, reply->value));
		value->priority = AbstractPropertyType::Instant;
		src->updateProperty(value.get());
	}

	return true;
}

void OBD2Source::updateProperty(AbstractPropertyType* value)
{
	VehicleProperty::Property property = value->name;
	if(property == Obd2Connected)
		obd2Connected.setValue(value->anyValue());

	AsyncPropertyReply* reply = nullptr;

	for(auto i : propertyReplyList)
	{
		if(i->property == property)
		{
			reply = i;
			break;
		}
	}

	if (reply)
	{

		reply->value = value;
		reply->success = true;
		try {
			reply->completed(reply);
		}catch(...)
		{
			DebugOut(DebugOut::Error)<<"failed to call reply completed callback"<<endl;
		}

		removeOne(&propertyReplyList, reply);
	}
	else
	{
		if(oldValueMap.find(property) != oldValueMap.end())
		{
			AbstractPropertyType* old = oldValueMap[property];

			if((*old) == (*value))
			{
				return;
			}

			delete old;
		}

		oldValueMap[property] = value->copy();

		m_re->updateProperty(value, uuid());
	}
}

void OBD2Source::setSupported(PropertyList list)
{
	m_supportedProperties = list;
	m_re->updateSupported(list,PropertyList(),this);
}
/*void OBD2Source::propertySignal(VehicleProperty::Property property,boost::any value)
{
}
void OBD2Source::checkProperty()
{
}*/
void OBD2Source::setConfiguration(map<string, string> config)
{
	// 	//Config has been passed, let's start stuff up.
	configuration = config;

	//Default values
	std::string port = "/dev/ttyUSB0";
	std::string baud = "115200";
	std::string btadapter = "";
	m_isBluetooth = false;

	//Try to load config
	//printf("OBD2Source::setConfiguration\n");
	for (map<string,string>::iterator i=configuration.begin();i!=configuration.end();i++)
	{
		//printf("Incoming setting: %s:%s\n",(*i).first.c_str(),(*i).second.c_str());
		DebugOut(5) << __SMALLFILE__ <<":"<< __LINE__ << "Incoming setting:" << (*i).first << ":" << (*i).second << endl;
		if ((*i).first == "device")
		{
			port = (*i).second;
		}
		else if ((*i).first == "baud")
		{
			if((*i).second != "")
				baud = (*i).second;
		}

		else if ((*i).first == "bluetoothAdapter")
		{
			btadapter = (*i).second;
		}
	}

	if(port.find(":") != string::npos)
	{
		m_btDeviceAddress = port;
		m_btAdapterAddress = btadapter;
		m_isBluetooth = true;
		///TODO: bluetooth!!
		/*DebugOut()<<"bluetooth device?"<<endl;
	BluetoothDevice bt;

	std::string tempPort = bt.getDeviceForAddress(port, btadapter);
	if(tempPort != "")
	{
	  DebugOut(3)<<"Using bluetooth device \""<<port<<"\" bound to: "<<tempPort<<endl;
	  port = tempPort;
	}
	else
	{
	  DebugOut(0)<<"Device Error"<<endl;
	  ///Don't throw here.
	  //throw std::runtime_error("Device Error");
	}*/
	}

	//connect(obd, port, baud);
	CommandRequest *req = new CommandRequest();
	req->req = "setportandbaud";
	req->arglist.push_back(port);
	req->arglist.push_back(baud);
	g_async_queue_push(commandQueue,req);

	m_port = port;
	m_baud = baud;
	m_gThread = g_thread_new("mythread",(GThreadFunc)&threadLoop,this);
	//g_idle_add(updateProperties, this);
	g_timeout_add(5,updateProperties,this);
}

OBD2Source::OBD2Source(AbstractRoutingEngine *re, map<string, string> config)
	: AbstractSource(re, config),obd2Connected(Obd2Connected,false)
{
	bool success = VehicleProperty::registerProperty(Obd2Connected,[](){ return new Obd2ConnectType(Obd2Connected,false); });

	if(!success)
	{
		///ERROR!
	}

	clientConnected = false;
	m_re = re;

	m_threadLive = true;
	Obd2Amb obd2amb;
	obd = new obdLib();

	for(auto itr = obd2amb.supportedPidsList.begin(); itr != obd2amb.supportedPidsList.end(); itr++)
	{
		m_supportedProperties.push_back((*itr)->property);
	}

	m_supportedProperties.push_back(Obd2Connected);

	re->updateSupported(supported(), PropertyList(), this);

	statusQueue = g_async_queue_new();
	commandQueue = g_async_queue_new();
	subscriptionAddQueue = g_async_queue_new();
	subscriptionRemoveQueue = g_async_queue_new();
	responseQueue = g_async_queue_new();
	singleShotQueue = g_async_queue_new();

	setConfiguration(config);
}
OBD2Source::~OBD2Source()
{
	DebugOut() << "OBD2Source Destructor called!!!"<<endl;
	m_threadLive = false;
	g_thread_join(m_gThread);
}

PropertyList OBD2Source::supported()
{
	return m_supportedProperties;
}

int OBD2Source::supportedOperations()
{
	return Get | Set;
}

extern "C" AbstractSource * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new OBD2Source(routingengine, config);

}
const string OBD2Source::uuid()
{
	return "f77af740-f1f8-11e1-aff1-0800200c9a66";
}
void OBD2Source::subscribeToPropertyChanges(VehicleProperty::Property property)
{
	if (property == VehicleProperty::VIN)
	{
		DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "VIN subscription requested... but there's no point!"<<endl;
	}
	else if (property == VehicleProperty::WMI)
	{
		DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "WMI subscription requested... but there's no point!"<<endl;
	}
	else
	{
		if(!contains(m_supportedProperties, property))
		{
			DebugOut(0)<<"obd plugin does not support: "<<property<<endl;
			return;
		}


		ObdPid *pid = obd2AmbInstance->createPidforProperty(property);

		if(!pid)
		{
			DebugOut(DebugOut::Warning) << "We don't support this property: " << property <<endl;
			return;
		}

		//If the pid is currently in the blacklist map, erase it. This allows for applications
		//to "un-blacklist" a pid by re-subscribing to it.
		if (m_blacklistPidCountMap.find(pid->pid) != m_blacklistPidCountMap.end())
		{
			m_blacklistPidCountMap.erase(m_blacklistPidCountMap.find(pid->pid));
		}


		g_async_queue_push(subscriptionAddQueue, pid);
		CommandRequest *req = new CommandRequest();
		req->req = "connectifnot";
		g_async_queue_push(commandQueue,req);
	}
}


void OBD2Source::unsubscribeToPropertyChanges(VehicleProperty::Property property)
{
	if(!contains(m_supportedProperties, property))
	{
		DebugOut(0)<<"obd plugin does not support: "<<property<<endl;
		return;
	}

	ObdPid *pid = obd2AmbInstance->createPidforProperty(property);
	g_async_queue_push(subscriptionRemoveQueue,pid);
}


void OBD2Source::getPropertyAsync(AsyncPropertyReply *reply)
{
	DebugOut(5) << __SMALLFILE__ <<":"<< __LINE__ << "getPropertyAsync requested for " << reply->property << endl;

	VehicleProperty::Property property = reply->property;


	if(!contains(m_supportedProperties, property))
	{
		DebugOut(0)<<"obd plugin does not support: "<<property<<endl;
		return;
	}

	if(reply->property == Obd2Connected)
	{
		reply->success = true;
		reply->value = &obd2Connected;
		reply->completed(reply);
		return;
	}

	propertyReplyList.push_back(reply);
	reply->timedout = [this](AsyncPropertyReply* reply)
	{
		DebugOut() << "removing "<< reply->property << " from propertyReplyList" << endl;
		removeOne(&propertyReplyList, reply);
	};

	ObdPid* requ = obd2AmbInstance->createPidforProperty(property);
	g_async_queue_push(singleShotQueue,requ);
	CommandRequest *req = new CommandRequest();
	req->req = "connectifnot";
	g_async_queue_push(commandQueue,req);
}

AsyncPropertyReply *OBD2Source::setProperty(AsyncSetPropertyRequest request )
{
	AsyncPropertyReply* reply = new AsyncPropertyReply (request);

	if(request.property == Obd2Connected)
	{
		propertyReplyList.push_back(reply);
		reply->success = true;

		if(request.value->value<bool>() == true)
		{
			CommandRequest *req = new CommandRequest();
			req->req = "connectifnot";
			g_async_queue_push(commandQueue,req);
		}
		else
		{
			CommandRequest *req = new CommandRequest();
			req->req = "disconnect";
			g_async_queue_push(commandQueue,req);
		}

	}
	else
	{
		reply->success = false;
		try
		{
			reply->completed(reply);
		}
		catch (...)
		{

		}
	}


	return reply;
}
