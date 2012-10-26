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
#include "bluetooth.hpp"


#define __SMALLFILE__ std::string(__FILE__).substr(std::string(__FILE__).rfind("/")+1)
AbstractRoutingEngine *m_re;

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
void threadLoop(gpointer data)
{
	GAsyncQueue *privCommandQueue = g_async_queue_ref(((OBD2Source*)data)->commandQueue);
	GAsyncQueue *privResponseQueue = g_async_queue_ref(((OBD2Source*)data)->responseQueue);
	GAsyncQueue *privSingleShotQueue = g_async_queue_ref(((OBD2Source*)data)->singleShotQueue);
	GAsyncQueue *privSubscriptionAddQueue = g_async_queue_ref(((OBD2Source*)data)->subscriptionAddQueue);
	GAsyncQueue *privSubscriptionRemoveQueue = g_async_queue_ref(((OBD2Source*)data)->subscriptionRemoveQueue);
	obdLib *obd = new obdLib();

	obd->setCommsCallback([](const char* mssg, void* data) { DebugOut(6)<<mssg<<endl; },NULL);
	obd->setDebugCallback([](const char* mssg, void* data, obdLib::DebugLevel debugLevel) { DebugOut(debugLevel)<<mssg<<endl; },NULL);
	
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
			//printf("Got request!\n");
			DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Got single shot request!"<<endl;
			ObdRequest *req = (ObdRequest*)query;
			repeatReqList.push_back(req->req);
			delete req;
		}
		query = g_async_queue_try_pop(privSubscriptionAddQueue);
		if (query != nullptr)
		{

			ObdRequest *req = (ObdRequest*)query;
			DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Got subscription request for "<<req->req<<endl;
			reqList.push_back(req->req);
			delete req;
		}
		query = g_async_queue_try_pop(privCommandQueue);
		if (query != nullptr)
		{
			ObdRequest *req = (ObdRequest*)query;
			//commandMap[req->req] = req->arg;
			//printf("Command: %s\n",req->req.c_str());
			DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Command:" << req->req << endl;
			if (req->req == "connect")
			{
				//printf("First: %s\nSecond: %s\n",req->arg.substr(0,req->arg.find(':')).c_str(),req->arg.substr(req->arg.find(':')+1).c_str());
				std::string port = req->arg.substr(0,req->arg.find(':'));
				int baud = boost::lexical_cast<int>(req->arg.substr(req->arg.find(':')+1));
				obd->openPort(port.c_str(),baud);
				
				obd->sendObdRequestString("ATZ\r",4,&replyVector,500,3);
				for (unsigned int i=0;i<replyVector.size();i++)
				{
					reply += replyVector[i];
				}
				if (reply.find("ELM") == -1)
				{
					//No reply found
					//printf("Error!\n");
					DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Error resetting ELM\n";
				}
				else
				{
					//printf("Reply to reset: %s\n",reply.c_str());
				}
				if (!sendElmCommand(obd,"ATSP0"))
				{
					//printf("Error sending echo\n");
					DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Error setting auto protocol"<<endl;
				}
				if (!sendElmCommand(obd,"ATE0"))
				{
					//printf("Error sending echo\n");
					DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Error turning off echo"<<endl;
				}
				if (!sendElmCommand(obd,"ATH0"))
				{
					//printf("Error sending headers off\n");
					DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Error turning off headers"<<endl;
				}
				if (!sendElmCommand(obd,"ATL0"))
				{
					//printf("Error turning linefeeds off\n");
					DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Error turning off linefeeds"<<endl;
				}
			}
			delete req;
		}
		query = g_async_queue_try_pop(privSubscriptionRemoveQueue);
		if (query != nullptr)
		{
			DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Got unsubscription request\n";
			ObdRequest *req = (ObdRequest*)query;
			for (std::list<std::string>::iterator i=reqList.begin();i!= reqList.end();i++)
			{
				if ((*i) == req->req)
				{
					reqList.erase(i);
					i--;
				}
			}
			//reqList.push_back(req->req);
			delete req;
		}
		
		for (std::list<std::string>::iterator i=reqList.begin();i!= reqList.end();i++)
		{
			repeatReqList.push_back(*i);
		}
		for (std::list<std::string>::iterator i=repeatReqList.begin();i!= repeatReqList.end();i++)
		{
			//printf("Req: %s\n",(*i).c_str());
			if ((*i) == "ATRV\r")
			{
				//printf("Requesting voltage...\n");
				if (!obd->sendObdRequestString((*i).c_str(),(*i).length(),&replyVector))
				{
					//printf("Unable to request voltage!!!\n");
					DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Unable to request voltage!\n";
					continue;
				}
				std::string replystring = "";
				for (int j=0;j<replyVector.size();j++)
				{
					replystring += replyVector[j];
				}
				//printf("Voltage reply: %s\n",replystring.c_str());
				replystring.substr(0,replystring.find("V"));
				ObdReply *rep = new ObdReply();
				rep->req = "ATRV\r";
				rep->reply = replystring;
				g_async_queue_push(privResponseQueue,rep);
			}
			if (!obd->sendObdRequest((*i).c_str(),(*i).length(),&replyVector))
			{
				//printf("Error sending obd2 request\n");
				DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Error sending OBD2 request\n";
				continue;
			}
			//printf("Reply: %i %i\n",replyVector[0],replyVector[1]);
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
				else if (replyVector[1] == 0x05)
				{
					int temp = replyVector[2] - 40;
					ObdReply *rep = new ObdReply();
					rep->req = "05";
					rep->reply = boost::lexical_cast<string>(temp);
					g_async_queue_push(privResponseQueue,rep);
				}
				else if (replyVector[1] == 0x10)
				{
					double maf = ((replyVector[2] << 8) + replyVector[3]) / 100.0;
					ObdReply *rep = new ObdReply();
					rep->req = "10";
					rep->reply = boost::lexical_cast<string>(maf);
					g_async_queue_push(privResponseQueue,rep);
				}
				else
				{
					//printf("Unknown response type: %i\n",replyVector[1]);
					DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Unknown response type" << replyVector[1] << "\n";
				}
			}
			else if (replyVector[0] == 0x49)
			{
				/*
				49 02 01 00 00 00 31 
				49 02 02 47 31 4A 43 
				49 02 03 35 34 34 34 
				49 02 04 52 37 32 35 
				49 02 05 32 33 36 37 
				*/
				//VIN number reply
				string vinstring;
				for (int j=0;j<replyVector.size();j++)
				{
					if(replyVector[j] == 0x49 && replyVector[j+1] == 0x02)
					{
						//We're at a reply header
						j+=3;
					}
					if (replyVector[j] != 0x00)
					{
						vinstring += (char)replyVector[j];
						//printf("VIN: %i %c\n",replyVector[j],replyVector[j]);
					}
				}
				ObdReply *rep = new ObdReply();
				rep->req = "0902";
				rep->reply = vinstring;
				g_async_queue_push(privResponseQueue,rep);
				//printf("VIN Number: %i %s\n",replyVector.size(),vinstring.c_str());
				
			}
			
			//DebugOut()<<"Reply: "<<replyVector[2]<<" "<<replyVector[3]<<endl;
		}
		if(!reqList.size()) usleep(10000);
		repeatReqList.clear();
	}
	
}

static int updateProperties(/*gpointer retval,*/ gpointer data)
{

	OBD2Source* src = (OBD2Source*)data;
	
	while(gpointer retval = g_async_queue_try_pop(src->responseQueue))
	{
		ObdReply *reply = (ObdReply*)retval;
		if (reply->req == "05")
		{
			VehicleProperty::EngineCoolantTemperatureType speed(reply->reply);
			src->updateProperty(VehicleProperty::EngineCoolantTemperature,&speed);
		}
		else if (reply->req == "0C")
		{
			VehicleProperty::EngineSpeedType speed(reply->reply);
			src->updateProperty(VehicleProperty::EngineSpeed,&speed); 
		}
		else if (reply->req == "0D")
		{
			VehicleProperty::VehicleSpeedType speed(reply->reply);
			src->updateProperty(VehicleProperty::VehicleSpeed,&speed);
		}
		else if (reply->req == "10")
		{
			VehicleProperty::MassAirFlowType mass(reply->reply);
			src->updateProperty(VehicleProperty::MassAirFlow,&mass);
		}
		else if (reply->req == "ATRV\r")
		{
			VehicleProperty::BatteryVoltageType volts(reply->reply);
			src->updateProperty(VehicleProperty::BatteryVoltage,&volts);
			
		}
		else if (reply->req == "0902")
		{
			//VIN number and WMI
			VehicleProperty::VINType vin(reply->reply);
			src->updateProperty(VehicleProperty::VIN,&vin);
			VehicleProperty::WMIType wmi(reply->reply.substr(0,3));
			src->updateProperty(VehicleProperty::WMI,&wmi);
		}
		else if (reply->req == "5C")
		{
			VehicleProperty::EngineCoolantTemperatureType ect(reply->reply);
			src->updateProperty(VehicleProperty::EngineCoolantTemperature,&ect);
		}
		else if (reply->req == "46")
		{
			VehicleProperty::InteriorTemperatureType temp(reply->reply);
			src->updateProperty(VehicleProperty::InteriorTemperature,&temp);
		}
		//5C -- engine oil temp
		//46 interior temp
		delete reply;
	}

	return true;
}
void OBD2Source::updateProperty(VehicleProperty::Property property,AbstractPropertyType* value)
{
	//m_re->updateProperty(property,&value);
	m_re->updateProperty(property,value); 
	if (propertyReplyMap.find(property) != propertyReplyMap.end())
	{
		propertyReplyMap[property]->value = value;
		propertyReplyMap[property]->completed(propertyReplyMap[property]);
		propertyReplyMap.erase(property);
	}
}
void OBD2Source::mafValue(double maf)
{
	VehicleProperty::VehicleSpeedType emaf(maf);
	m_re->updateProperty(VehicleProperty::MassAirFlow,&emaf);
}
void OBD2Source::engineCoolantTemp(int temp)
{
	VehicleProperty::VehicleSpeedType etemp(temp);
	m_re->updateProperty(VehicleProperty::EngineCoolantTemperature,&etemp);
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
	
	//Try to load config
	//printf("OBD2Source::setConfiguration\n");
	for (map<string,string>::iterator i=configuration.begin();i!=configuration.end();i++)
	{
		//printf("Incoming setting: %s:%s\n",(*i).first.c_str(),(*i).second.c_str());
		DebugOut(5) << __SMALLFILE__ <<":"<< __LINE__ << "Incoming setting:" << (*i).first << ":" << (*i).second << "\n";
		if ((*i).first == "device")
		{
			port = (*i).second;
		}
		else if ((*i).first == "baud")
		{
			baud = (*i).second;
		}

		else if ((*i).first == "bluetoothAdapter")
		{
			btadapter = (*i).second;
		}
	}

	if(port.find(":") != string::npos)
	{
		///TODO: bluetooth!!
		DebugOut()<<"bluetooth device?"<<endl;
		ObdBluetoothDevice bt;

		std::string tempPort = bt.getDeviceForAddress(port, btadapter);
		if(tempPort != "")
		{
			DebugOut(3)<<"Using bluetooth device \""<<port<<"\" bound to: "<<tempPort<<endl;
			port = tempPort;
		}
		else throw std::runtime_error("Device Error");
	}

	ObdRequest *requ = new ObdRequest();
	requ->req = "connect";
	requ->arg = port + ":" + baud;
	g_async_queue_push(commandQueue,requ);
}

OBD2Source::OBD2Source(AbstractRoutingEngine *re, map<string, string> config) : AbstractSource(re, config)
{
	clientConnected = false;
	m_re = re;  

	Obd2Amb obd2amb;

	for(auto itr = obd2amb.propertyPidMap.begin(); itr != obd2amb.propertyPidMap.end(); itr++)
	{
		m_supportedProperties.push_back((*itr).first);
	}

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

	setConfiguration(config);

	//AsyncQueueWatcher * watcher = new AsyncQueueWatcher(responseQueue, (AsyncQueueWatcherCallback) updateProperties, this);

	//g_timeout_add(1,updateProperties, this);
	g_idle_add(updateProperties, this);
	//g_timeout_add(1000,calcCPS,NULL);

}

PropertyList OBD2Source::supported()
{
	return m_supportedProperties;
}
extern "C" AbstractSource * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new OBD2Source(routingengine, config);
	
}
string OBD2Source::uuid()
{
	return "f77af740-f1f8-11e1-aff1-0800200c9a66";
}
void OBD2Source::subscribeToPropertyChanges(VehicleProperty::Property property)
{
	/*//printf("Subscribed to property: %s\n",property.c_str());
	if (property == VehicleProperty::EngineSpeed)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "010C1\r";
		g_async_queue_push(subscriptionAddQueue,requ);
	}
	else if (property == VehicleProperty::MassAirFlow)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "01101\r";
		g_async_queue_push(subscriptionAddQueue,requ);
	}
	else if (property == VehicleProperty::VehicleSpeed)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "010D1\r";
		g_async_queue_push(subscriptionAddQueue,requ);
	}
	else if (property == VehicleProperty::EngineCoolantTemperature)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "01051\r";
		g_async_queue_push(subscriptionAddQueue,requ);
	}
	else if (property == VehicleProperty::VIN)
	{
		DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "VIN subscription requested... but there's no point!\n";
	}
	else if (property == VehicleProperty::WMI)
	{
		DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "WMI subscription requested... but there's no point!\n";
	}
	else if (property == VehicleProperty::EngineOilTemperature)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "015C1\r";
		g_async_queue_push(subscriptionAddQueue,requ);
	}
	else if (property == VehicleProperty::InteriorTemperature)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "01461\r";
		g_async_queue_push(subscriptionAddQueue,requ);
	}
	else if (property == VehicleProperty::BatteryVoltage)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "ATRV\r";
		g_async_queue_push(subscriptionAddQueue,requ);
	}
	/*m_supportedProperties.push_back(VehicleProperty::VIN);
	m_supportedProperties.push_back(VehicleProperty::WMI);
	m_supportedProperties.push_back(VehicleProperty::EngineOilTemperature);
	m_supportedProperties.push_back(VehicleProperty::InteriorTemperature);
	m_supportedProperties.push_back(VehicleProperty::BatteryVoltage);*/
	/*else
	{
		//printf("Unsupported property: %s\n",property.c_str());
		DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Unsupported property requested:" << property << "\n";
	}*/

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
		if(!ListPlusPlus<VehicleProperty::Property>(&m_supportedProperties).contains(property))
		{
			DebugOut(0)<<"obd plugin does not support: "<<property<<endl;
			return;
		}

		Obd2Amb obd2amb;
		ObdRequest *requ = new ObdRequest();
		requ->req = obd2amb.propertyPidMap[property];
		g_async_queue_push(subscriptionAddQueue,requ);
	}
}


void OBD2Source::unsubscribeToPropertyChanges(VehicleProperty::Property property)
{
	//
	/*if (property == VehicleProperty::EngineSpeed)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "010C1\r";
		g_async_queue_push(subscriptionRemoveQueue,requ);
	}
	else if (property == VehicleProperty::MassAirFlow)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "01101\r";
		g_async_queue_push(subscriptionRemoveQueue,requ);
	}
	else if (property == VehicleProperty::VehicleSpeed)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "010D1\r";
		g_async_queue_push(subscriptionRemoveQueue,requ);
	}
	else if (property == VehicleProperty::EngineCoolantTemperature)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "01051\r";
		g_async_queue_push(subscriptionRemoveQueue,requ);
	}
	else if (property == VehicleProperty::VIN)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "0902\r";
		g_async_queue_push(subscriptionRemoveQueue,requ);
	}
	else if (property == VehicleProperty::WMI)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "0902\r";
		g_async_queue_push(subscriptionRemoveQueue,requ);
	}
	else if (property == VehicleProperty::EngineOilTemperature)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "015C1\r";
		g_async_queue_push(subscriptionRemoveQueue,requ);
	}
	else if (property == VehicleProperty::InteriorTemperature)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "01461\r";
		g_async_queue_push(subscriptionRemoveQueue,requ);
	}
	else if (property == VehicleProperty::BatteryVoltage)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "ATRV\r";
		g_async_queue_push(subscriptionRemoveQueue,requ);
	}
	*/

	if(!ListPlusPlus<VehicleProperty::Property>(&m_supportedProperties).contains(property))
	{
		DebugOut(0)<<"obd plugin does not support: "<<property<<endl;
		return;
	}

	Obd2Amb obd2amb;
	ObdRequest *requ = new ObdRequest();
	requ->req = obd2amb.propertyPidMap[property];
	g_async_queue_push(subscriptionRemoveQueue,requ);
}


void OBD2Source::getPropertyAsync(AsyncPropertyReply *reply)
{
	propertyReplyMap[reply->property] = reply;
	VehicleProperty::Property property = reply->property;

	//TODO: There is a much better way to do this, but for now it's hardcoded.
	/*if (property == VehicleProperty::EngineSpeed)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "010C\r";
		g_async_queue_push(singleShotQueue,requ);
	}
	else if (property == VehicleProperty::MassAirFlow)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "0110\r";
		g_async_queue_push(singleShotQueue,requ);
	}
	else if (property == VehicleProperty::VehicleSpeed)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "010D\r";
		g_async_queue_push(singleShotQueue,requ);
	}
	else if (property == VehicleProperty::EngineCoolantTemperature)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "0105\r";
		g_async_queue_push(singleShotQueue,requ);
	}
	else if (property == VehicleProperty::VIN)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "0902\r";
		g_async_queue_push(singleShotQueue,requ);
	}
	else if (property == VehicleProperty::WMI)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "0902\r";
		g_async_queue_push(singleShotQueue,requ);
	}
	else if (property == VehicleProperty::EngineOilTemperature)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "015C\r";
		g_async_queue_push(singleShotQueue,requ);
	}
	else if (property == VehicleProperty::InteriorTemperature)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "0146\r";
		g_async_queue_push(singleShotQueue,requ);
	}
	else if (property == VehicleProperty::BatteryVoltage)
	{
		ObdRequest *requ = new ObdRequest();
		requ->req = "ATRV\r";
		g_async_queue_push(singleShotQueue,requ);
	}
	*/

	///Here's a better way:

	if(!ListPlusPlus<VehicleProperty::Property>(&m_supportedProperties).contains(property))
	{
		DebugOut(0)<<"obd plugin does not support: "<<property<<endl;
		return;
	}

	Obd2Amb obd2amb;
	ObdRequest *requ = new ObdRequest();
	requ->req = obd2amb.propertyPidMap[property];
	g_async_queue_push(singleShotQueue,requ);
}

AsyncPropertyReply *OBD2Source::setProperty(AsyncSetPropertyRequest request )
{
	AsyncPropertyReply* reply = new AsyncPropertyReply (request);
	reply->success = false;
	try
	{
		reply->completed(reply);
	}
	catch (...)
	{

	}
	return reply;
}
