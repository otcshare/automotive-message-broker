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

#include "abstractdbusinterface.h"
#include "debugout.h"
#include <boost/algorithm/string.hpp>
#include <gio/gio.h>

#include "abstractproperty.h"

unordered_map<string, AbstractDBusInterface*> AbstractDBusInterface::interfaceMap;

AbstractDBusInterface::AbstractDBusInterface(string interfaceName, string objectPath)
: mInterfaceName(interfaceName), mObjectPath(objectPath)
{
	mConnection = nullptr;

	interfaceMap[interfaceName] = this;
	introspectionXml ="<node>" ;
	introspectionXml += "<interface name='"+ interfaceName + "' >";
}

void AbstractDBusInterface::addProperty(AbstractProperty* property)
{
	string nameToLower = property->name();
	boost::algorithm::to_lower<string>(nameToLower);
	
	///see which properties are supported:
	introspectionXml += 	"<property type='"+ property->signature() + "' name='"+ property->name()+"' access='read' />"
	"<signal name='" + property->name() + "' >"
	"	<arg type='"+ property->signature() + "' name='" + nameToLower + "' direction='out' />"
	"</signal>";
	
	properties[property->name()] = property;
}

void AbstractDBusInterface::registerObject(GDBusConnection* connection)
{
	mConnection = connection;
	
	if(introspectionXml.empty())
	{
		cerr<<"no interface to export: "<<mInterfaceName<<endl;
		throw -1;
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
	
	const GDBusInterfaceVTable vtable = { NULL, AbstractDBusInterface::getProperty, AbstractDBusInterface::setProperty };
	
	guint regId = g_dbus_connection_register_object(connection, mObjectPath.c_str(), mInterfaceInfo, &vtable, NULL, NULL, &error);
	
	if(error) throw -1;
	
	g_assert(regId > 0);
}

void AbstractDBusInterface::updateValue(AbstractProperty *property)
{
	if(mConnection == nullptr)
	{
		return;
	}

	GError *error = NULL;
	g_dbus_connection_emit_signal(mConnection, "", mObjectPath.c_str(), mInterfaceName.c_str(), property->name().c_str(),property->toGVariant(), &error);

	if(error)
	{
		throw -1;
	}
}

GVariant* AbstractDBusInterface::getProperty(GDBusConnection* connection, const gchar* sender, const gchar* objectPath, const gchar* interfaceName, const gchar* propertyName, GError** error, gpointer userData)
{
	debugOut("setting error to NULL");
	*error = NULL;
	debugOut("Get Property called!");
	if(interfaceMap.count(interfaceName))
	{
		GVariant* value = interfaceMap[interfaceName]->getProperty(propertyName);
		return value;
	}
	debugOut("No interface for" + string(interfaceName));
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

