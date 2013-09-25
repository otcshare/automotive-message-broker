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

#ifndef WHEELPLUGIN_H
#define WHEELPLUGIN_H

#include <abstractsource.h>
#include <string>

using namespace std;

class WheelPrivate;

class WheelSourcePlugin: public AbstractSource
{

public:
	WheelSourcePlugin(AbstractRoutingEngine* re, map<string, string> config);
	~WheelSourcePlugin();
	
	const string uuid();
	void getPropertyAsync(AsyncPropertyReply *reply);
	void getRangePropertyAsync(AsyncRangePropertyReply *reply){}
	AsyncPropertyReply * setProperty(AsyncSetPropertyRequest request);
	void subscribeToPropertyChanges(VehicleProperty::Property property);
	void unsubscribeToPropertyChanges(VehicleProperty::Property property);
	PropertyList supported();

	int supportedOperations();
	
	void supportedChanged(PropertyList) {}

	map<string, string> getConfiguration() { return configuration; }

	friend class WheelPrivate;	
	
private:

	WheelPrivate *mWheel;
	PropertySet mRequests;
};

#endif // WHEELPLUGIN_H
