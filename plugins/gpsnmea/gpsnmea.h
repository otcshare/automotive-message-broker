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

#include <abstractio.hpp>
#include <ambpluginimpl.h>

using namespace std;

class Location;
class Bluetooth5;
class BluetoothDevice;

class GpsNmeaSource: public AmbPluginImpl
{

public:
	GpsNmeaSource(AbstractRoutingEngine* re, map<string, string> config, AbstractSource &parent);
	~GpsNmeaSource();

	const string uuid() const;

	void canHasData();

	void test();

	int supportedOperations() const;

	void init();

private:
	bool tryParse(std::string data);

	bool checksum(string sentence);

	PropertyList mRequests;
	PropertyList mSupported;

	AbstractIo* device;
	Location *location;

	std::string mUuid;

	std::string buffer;

	std::shared_ptr<AbstractPropertyType> rawNmea;

#ifdef USE_BLUEZ5
	Bluetooth5 * bt;
#else
	BluetoothDevice *bt;
#endif
};

#endif // EXAMPLEPLUGIN_H
