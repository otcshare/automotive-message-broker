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

class AbstractProperty;

using namespace std;

class AbstractDBusInterface
{

public:
	AbstractDBusInterface(string interfaceName, string objectPath, GDBusConnection* connection);

	
	void setDBusConnection(GDBusConnection* connection)
	{
		mConnection = connection;
	}

	void registerObject();
	void unregisterObject();
	
	void addProperty(AbstractProperty* property);
	virtual void updateValue(AbstractProperty* property);
	
	static list<string> implementedProperties() { return mimplementedProperties; }

	static AbstractDBusInterface *getInterfaceForObject(std::string object);

	static list<AbstractDBusInterface*> interfaces();

	std::string interfaceName() { return mInterfaceName; }

	bool implementsProperty(std::string property);

	std::string objectPath() { return mObjectPath; }

	bool isSupported() { return supported; }

	double time() { return mTime; }

protected:

	void startRegistration();
	
	static GVariant *getProperty(GDBusConnection * connection, const gchar * sender, const gchar *objectPath,
								 const gchar *interfaceName, const gchar * propertyName, GError** error,
								 gpointer userData);
	static gboolean setProperty(GDBusConnection * connection, const gchar * sender, const gchar *objectPath,
								const gchar *interfaceName, const gchar * propertyName, GVariant *value,
								GError** error, gpointer userData);
    
	virtual void setProperty(string propertyName, GVariant * value);
	virtual GVariant * getProperty(string propertyName);
	
	unordered_map<string, AbstractProperty*> properties;

	bool supported;
	double mTime;

private:
	string mInterfaceName;
	string mObjectPath;
	string introspectionXml;
	GDBusConnection * mConnection;
	static unordered_map<string, AbstractDBusInterface*> interfaceMap;
	static list<string> mimplementedProperties;
	guint regId;
};

#endif // ABSTRACTDBUSINTERFACE_H
