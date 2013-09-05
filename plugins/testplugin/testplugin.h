
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
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <glib.h>




class TestPlugin : public AbstractSource
{

public:
	TestPlugin(AbstractRoutingEngine* re, map<string, string> config);
	~TestPlugin();
	const string uuid();
	int portHandle;
	void getPropertyAsync(AsyncPropertyReply *reply);
	void getRangePropertyAsync(AsyncRangePropertyReply *reply){}
	AsyncPropertyReply * setProperty(AsyncSetPropertyRequest request);
	void subscribeToPropertyChanges(VehicleProperty::Property property);
	void unsubscribeToPropertyChanges(VehicleProperty::Property property);
	PropertyList supported();

	int supportedOperations();

	void setSupported(PropertyList list);
	void supportedChanged(PropertyList) {}
	void setConfiguration(map<string, string> config);
	//void randomizeProperties();
	void updateProperty(VehicleProperty::Property property,AbstractPropertyType *value);

private:
	PropertyList m_supportedProperties;
};

#endif // OBD2SOURCE_H
