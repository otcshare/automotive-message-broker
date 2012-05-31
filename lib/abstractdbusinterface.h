/*
    Copyright 2012 Kevron Rees <email>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/


#ifndef ABSTRACTDBUSINTERFACE_H
#define ABSTRACTDBUSINTERFACE_H

#include <string>
#include <functional>
#include <unordered_map>
#include <gio/gio.h>
#include <boost/any.hpp>

#include "abstractproperty.h"

using namespace std;

class AbstractDBusInterface
{

public:
	AbstractDBusInterface(string interfaceName, string objectPath);

	
	void registerObject(GDBusConnection* connection);
	
	void addProperty(AbstractProperty* property);
	virtual void updateValue(AbstractProperty* property);
	
protected:
	
	static GVariant *getProperty(GDBusConnection * connection, const gchar * sender, const gchar *objectPath, const gchar *interfaceName, const gchar * propertyName, GError** error, gpointer userData);
	static gboolean setProperty(GDBusConnection * connection, const gchar * sender, const gchar *objectPath, const gchar *interfaceName, const gchar * propertyName, GVariant *value, GError** error, gpointer userData);
    
	virtual void setProperty(string propertyName, GVariant * value) = 0;
	virtual GVariant * getProperty(string propertyName) = 0;
	
	unordered_map<string, AbstractProperty*> properties;

private:
	string mInterfaceName;
	string mObjectPath;
	string introspectionXml;
	GDBusConnection * mConnection;
	static unordered_map<string, AbstractDBusInterface*> interfaceMap;
};

#endif // ABSTRACTDBUSINTERFACE_H
