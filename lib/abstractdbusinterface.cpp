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


#include "abstractdbusinterface.h"
#include <boost/algorithm/string.hpp>
#include <gio/gio.h>

unordered_map<string, AbstractDBusInterface*> AbstractDBusInterface::interfaceMap;

AbstractDBusInterface::AbstractDBusInterface(string interfaceName, string objectPath)
: mInterfaceName(interfaceName), mObjectPath(objectPath)
{
	interfaceMap[interfaceName] = this;
	introspectionXml ="<node>" ;
	introspectionXml += "<interface name='"+ interfaceName + "' >";
}

void AbstractDBusInterface::addProperty(string name, string signature)
{
	string nameToLower = name;
	boost::algorithm::to_lower<string>(nameToLower);
	
	///see which properties are supported:
	introspectionXml += 	"<property type='"+ signature + "' name='"+name+"' access='read' />"
	"<signal name='" + name + "' >"
	"	<arg type='"+ signature + "' name='" + nameToLower + "' direction='out' />"
	"</signal>";
	
	PropertyNameSignaturePair property;
	property.name = name;
	property.signature = signature;
	
	properties[name] = property;
}

void AbstractDBusInterface::addProperty(string name, string signature, SetterFunc setter)
{
	addProperty(name, signature);
	
	properties[name].setter = setter;
}

void AbstractDBusInterface::registerObject(GDBusConnection* connection)
{
	mConnection = connection;
	
	if(introspectionXml.empty())
	{
		cerr<<"no interface to export: "<<mInterfaceName<<endl;
		return;
	}

	introspectionXml += "</interface>";
	introspectionXml += "</node>";
	
	GError* error=NULL;

	GDBusNodeInfo* introspection = g_dbus_node_info_new_for_xml(introspectionXml.c_str(), &error);
	
	if(!introspection)
	{
		cerr<<"Error in "<<__FILE__<<" - "<<__FUNCTION__<<":"<<__LINE__<<endl;
		cerr<<error->message<<endl;
		cerr<<"probably bad xml:"<<endl;
		cerr<<introspectionXml<<endl;
		return;
	}

	GDBusInterfaceInfo* mInterfaceInfo = g_dbus_node_info_lookup_interface(introspection, mInterfaceName.c_str());
	
	const GDBusInterfaceVTable vtable = {NULL, AbstractDBusInterface::getProperty, AbstractDBusInterface::setProperty};	
	
	guint regId = g_dbus_connection_register_object(connection, mObjectPath.c_str(), mInterfaceInfo, &vtable, NULL, NULL, NULL);
	
	g_assert(regId > 0);
}

void AbstractDBusInterface::updateValue(string name, boost::any value)
{

}

GVariant* AbstractDBusInterface::getProperty(GDBusConnection* connection, const gchar* sender, const gchar* objectPath, const gchar* interfaceName, const gchar* propertyName, GError** error, gpointer userData)
{
	if(interfaceMap.count(interfaceName))
		return interfaceMap[interfaceName]->getProperty(propertyName);
	return nullptr;
}

gboolean AbstractDBusInterface::setProperty(GDBusConnection* connection, const gchar* sender, const gchar* objectPath, const gchar* interfaceName, const gchar* propertyName, GVariant* value, GError** error, gpointer userData)
{
	if(interfaceMap.count(interfaceName))
	{
		interfaceMap[interfaceName]->setProperty(propertyName, value);
		return true;
	}

	return false;
}

