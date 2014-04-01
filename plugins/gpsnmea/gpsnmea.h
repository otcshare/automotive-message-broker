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

#ifndef GPSNMEAPLUGIN_H
#define GPSNMEAPLUGIN_H

#include <abstractsource.h>
#include <string>

#include "abstractio.hpp"

using namespace std;

class Location;

class GpsNmeaSource: public AbstractSource
{

public:
	GpsNmeaSource(AbstractRoutingEngine* re, map<string, string> config);
	~GpsNmeaSource();
	
	const string uuid();
	void getPropertyAsync(AsyncPropertyReply *reply);
	void getRangePropertyAsync(AsyncRangePropertyReply *reply);
	AsyncPropertyReply * setProperty(AsyncSetPropertyRequest request);
	void subscribeToPropertyChanges(VehicleProperty::Property property);
	void unsubscribeToPropertyChanges(VehicleProperty::Property property);
	PropertyList supported();

	int supportedOperations();
	
	void supportedChanged(const PropertyList &) {}
	
	PropertyInfo getPropertyInfo(VehicleProperty::Property property)
	{
		if(propertyInfoMap.find(property) != propertyInfoMap.end())
			return propertyInfoMap[property];

		return PropertyInfo::invalid();
	}

	void canHasData();

private:

	bool tryParse(std::string data);

	void addPropertySupport(VehicleProperty::Property property, Zone::Type zone);

	bool checksum(string sentence);

	std::map<VehicleProperty::Property, PropertyInfo> propertyInfoMap;

	PropertyList mRequests;
	PropertyList mSupported;

	AbstractIo* device;
	Location *location;

	std::string mUuid;

	std::string buffer;
};

#endif // EXAMPLEPLUGIN_H
