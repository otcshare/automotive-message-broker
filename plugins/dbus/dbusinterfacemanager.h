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


#ifndef DBUSINTERFACEMANAGER_H
#define DBUSINTERFACEMANAGER_H

#include "ambpluginimpl.h"
#include "gio/gio.h"
#include <memory>

class AbstractRoutingEngine;

class DBusInterfaceManager: public AmbPluginImpl
{

public:
	DBusInterfaceManager(AbstractRoutingEngine* re, std::map<string, string> config, AbstractSource & parent);
	~DBusInterfaceManager();

	/// From AbstractSink:
	virtual const string uuid() const { return "DBusInterfaceManager"; }
	virtual void supportedChanged(const PropertyList & supportedProperties);
    
	std::shared_ptr<GDBusConnection> connection;
	std::shared_ptr<AbstractPropertyType> dbusConnected;

	void registerTypes();

private:

	unsigned int ownerId;



};

#endif // DBUSINTERFACEMANAGER_H
