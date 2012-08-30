
/*
Copyright (C) 2012 Intel Corporation

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


#ifndef OBD2SOURCE_H
#define OBD2SOURCE_H



#include <abstractsource.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include "obdlib.h"
#include <glib/gasyncqueue.h>


class ObdRequest
{
public:
  std::string req;
};

class ObdReply
{
public:
  std::string req;
  std::string reply;
};



class OBD2Source : public AbstractSource
{

public:
	OBD2Source(AbstractRoutingEngine* re);
	string uuid();
	int portHandle;
	void getPropertyAsync(AsyncPropertyReply *reply);
	void setProperty(VehicleProperty::Property, AbstractPropertyType*);
	void subscribeToPropertyChanges(VehicleProperty::Property property);
	void unsubscribeToPropertyChanges(VehicleProperty::Property property);
	PropertyList supported();
	PropertyList queuedRequests;
	bool clientConnected;
	PropertyList activeRequests;
	void engineSpeed(double speed);
	void vehicleSpeed(int speed);
	PropertyList removeRequests;
	void setSupported(PropertyList list);
	void propertyChanged(VehicleProperty::Property property, AbstractPropertyType* value, string uuid) {}
	void supportedChanged(PropertyList) {}
	GAsyncQueue* commandQueue;
	GAsyncQueue* subscriptionAddQueue;
	GAsyncQueue* subscriptionRemoveQueue;
	GAsyncQueue* singleShotQueue;
	GAsyncQueue* responseQueue;
	//void randomizeProperties();
private:
	PropertyList m_supportedProperties;
	GMutex *threadQueueMutex;
	

};

#endif // OBD2SOURCE_H
