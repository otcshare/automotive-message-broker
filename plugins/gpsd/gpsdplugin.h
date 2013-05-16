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

#ifndef GPSDPLUGIN_H
#define GPSDPLUGIN_H

#include <abstractsource.h>
#include <string>

#include <gps.h>

using namespace std;

class GpsdPlugin: public AbstractSource
{

public:

	class Shared
	{
	public:
		Shared(GpsdPlugin* p)
			:oldspeed(0), oldalt(0), oldlat(0), oldlon(0), oldheading(0), parent(p)
		{

		}

		struct gps_data_t gps;
		std::string oldspeed;
		std::string oldalt;
		std::string oldlat;
		std::string oldlon;
		std::string oldheading;
		GpsdPlugin* parent;
	};

	GpsdPlugin(AbstractRoutingEngine* re, map<string, string> config);
	~GpsdPlugin();
	string uuid();
	void getPropertyAsync(AsyncPropertyReply *reply);
	void getRangePropertyAsync(AsyncRangePropertyReply *reply);
	AsyncPropertyReply * setProperty(AsyncSetPropertyRequest request);
	void subscribeToPropertyChanges(VehicleProperty::Property property);
	void unsubscribeToPropertyChanges(VehicleProperty::Property property);
	PropertyList supported();

	int supportedOperations();
	
	void propertyChanged(VehicleProperty::Property property, AbstractPropertyType* value, string uuid) {}
	void supportedChanged(PropertyList) {}
	
	void updateProperty();


private:	
	uint lastLux;
	std::string device;
	std::list<AsyncPropertyReply*> replyQueue;
	Shared* shared;

	bool updateVelocity;
};




#endif // EXAMPLEPLUGIN_H
