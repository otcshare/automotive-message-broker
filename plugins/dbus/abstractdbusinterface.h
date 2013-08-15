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

#ifndef ABSTRACTDBUSINTERFACE_H
#define ABSTRACTDBUSINTERFACE_H

#include <string>
#include <functional>
#include <unordered_map>
#include <list>
#include <gio/gio.h>
#include <boost/any.hpp>
#include <nullptr.h>
#include "abstractpropertytype.h"
#include <abstractroutingengine.h>

class AbstractProperty;

class AbstractDBusInterface
{

public:
	AbstractDBusInterface(std::string interfaceName, std::string objectName, GDBusConnection* connection);

	virtual ~AbstractDBusInterface();
	
	void setDBusConnection(GDBusConnection* connection)
	{
		mConnection = connection;
	}

	void registerObject();
	void unregisterObject();
	
	void addProperty(AbstractProperty* property);
	virtual void updateValue(AbstractProperty* property);
	
	static std::list<std::string> implementedProperties() { return mimplementedProperties; }

	static std::list<AbstractDBusInterface *> getObjectsForProperty(std::string property);

	static list<AbstractDBusInterface*> interfaces();

	std::string interfaceName() { return mInterfaceName; }

	bool implementsProperty(std::string property);

	std::string objectPath() { return mObjectPath; }

	bool isSupported() { return supported; }

	double time() { return mTime; }

	AbstractRoutingEngine* re;

	void setObjectPath(std::string op)
	{
		if(objectMap.find(op) != objectMap.end())
			objectMap.erase(op);

		mObjectPath = op;
		objectMap[mObjectPath] = this;
	}

	std::string objectName() { return mPropertyName; }

	Zone::Type zone() { return zoneFilter; }

	std::unordered_map<std::string, AbstractProperty*> getProperties() { return properties; }

protected:

	void startRegistration();
	
	static GVariant *getProperty(GDBusConnection * connection, const gchar * sender, const gchar *objectPath,
								 const gchar *interfaceName, const gchar * propertyName, GError** error,
								 gpointer userData);
	static gboolean setProperty(GDBusConnection * connection, const gchar * sender, const gchar *objectPath,
								const gchar *interfaceName, const gchar * propertyName, GVariant *value,
								GError** error, gpointer userData);
    
	virtual void setProperty(std::string propertyName, GVariant * value);
	virtual GVariant * getProperty(std::string propertyName);
	
	std::unordered_map<std::string, AbstractProperty*> properties;

	Zone::Type zoneFilter;


	bool supported;
	double mTime;

private:
	std::string mInterfaceName;
	std::string mObjectPath;
	std::string mPropertyName;
	std::string introspectionXml;
	GDBusConnection * mConnection;
	static std::unordered_map<std::string, AbstractDBusInterface*> objectMap;
	static std::list<std::string> mimplementedProperties;
	guint regId;
};

#endif // ABSTRACTDBUSINTERFACE_H
