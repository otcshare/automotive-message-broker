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
#include <boost/algorithm/string/predicate.hpp>
#include <gio/gio.h>

#include "listplusplus.h"
#include "abstractproperty.h"

unordered_map<string, AbstractDBusInterface*> AbstractDBusInterface::interfaceMap;
list<string> AbstractDBusInterface::mimplementedProperties;


static void handleMyMethodCall(GDBusConnection       *connection,
							 const gchar           *sender,
							 const gchar           *object_path,
							 const gchar           *interface_name,
							 const gchar           *method_name,
							 GVariant              *parameters,
							 GDBusMethodInvocation *invocation,
							 gpointer               user_data)
{

	std::string method = method_name;
	AbstractDBusInterface* iface = static_cast<AbstractDBusInterface*>(user_data);

	g_assert(iface);

	if(boost::algorithm::starts_with(method,"get"))
	{
		std::string propertyName = method.substr(3);
		auto propertyMap = iface->getProperties();
		if(propertyMap.find(propertyName) == propertyMap.end())
		{
			g_dbus_method_invocation_return_error(invocation, G_DBUS_ERROR, G_DBUS_ERROR_UNKNOWN_METHOD, "Unknown method.");
			return;
		}

		AbstractProperty* property = propertyMap[propertyName];

		GError *error = NULL;

		GVariant **params = g_new(GVariant*,4);
		GVariant *val = g_variant_ref(property->value()->toVariant());
		params[0] = g_variant_new("v",val);
		params[1] = g_variant_new("d",property->timestamp());
		params[2] = g_variant_new("i",property->value()->sequence);
		params[3] = g_variant_new("i",property->updateFrequency());

		GVariant *tuple_variant = g_variant_new_tuple(params,4);

		g_dbus_method_invocation_return_value(invocation, tuple_variant);

		g_free(params);
		g_variant_unref(val);

		if(error)
		{
			DebugOut(DebugOut::Error)<<error->message<<endl;
			g_error_free(error);
		}
		return;
	}

	g_dbus_method_invocation_return_error(invocation,G_DBUS_ERROR,G_DBUS_ERROR_UNKNOWN_METHOD, "Unknown method.");
}

AbstractDBusInterface::AbstractDBusInterface(string interfaceName, string objectName,
											 GDBusConnection* connection)
	: mInterfaceName(interfaceName), mConnection(connection), mPropertyName(objectName), supported(false), zoneFilter(Zone::None), mTime(0)
{
	startRegistration();

	mObjectPath = "/" + objectName;
}

AbstractDBusInterface::~AbstractDBusInterface()
{
	unregisterObject();

	list<std::string> impl = implementedProperties();

	for(auto itr = impl.begin(); itr != impl.end(); itr++)
	{
		if(properties.find(*itr) != properties.end())
		{
			delete properties[*itr];
			properties.erase(*itr);
		}
	}

	interfaceMap.erase(mInterfaceName);

}

void AbstractDBusInterface::addProperty(AbstractProperty* property)
{
	string nameToLower = property->name();
	boost::algorithm::to_lower<string>(nameToLower);
	
	string access;

	if(property->access() == AbstractProperty::Read)
		access = "read";
	else if(property->access() == AbstractProperty::Write)
		access = "write";
	else if(property->access() == AbstractProperty::ReadWrite)
		access = "readwrite";
	else throw -1; //FIXME: don't throw

	std::string pn = property->name();

	///see which properties are supported:
	introspectionXml +=
			"<property type='"+ property->signature() + "' name='"+ pn +"' access='"+access+"' />"
			"<method name='get" + pn + "'>"
			"	<arg type='v' direction='out' name='value' />"
			"	<arg type='d' direction='out' name='timestamp' />"
			"	<arg type='i' direction='out' name='sequence' />"
			"   <arg type='i' direction='out' name='updateFrequency' />"
			"</method>"
			"<signal name='" + pn + "Changed' >"
			"	<arg type='v' name='" + nameToLower + "' direction='out' />"
			"	<arg type='d' name='timestamp' direction='out' />"
			"</signal>";
	
	properties[property->name()] = property;

	if(!ListPlusPlus<string>(&mimplementedProperties).contains(property->ambPropertyName()))
	{
		std::string pname = property->ambPropertyName();
		mimplementedProperties.push_back(pname);
	}
}

void AbstractDBusInterface::registerObject()
{
	if(!mConnection)
	{
		throw std::runtime_error("forgot to call setDBusConnection on AbstractDBusInterface");
	}
	
	if(introspectionXml.empty())
	{
		cerr<<"no interface to export: "<<mInterfaceName<<endl;
		throw -1;
	}

	introspectionXml += "</interface>";
	introspectionXml += "</node>";
	
	GError* error=NULL;

	GDBusNodeInfo* introspection = g_dbus_node_info_new_for_xml(introspectionXml.c_str(), &error);
	
	if(!introspection || error)
	{

		DebugOut(0)<<"Error in "<<__FILE__<<" - "<<__FUNCTION__<<":"<<__LINE__<<endl;
		DebugOut(0)<<error->message<<endl;
		DebugOut(0)<<"probably bad xml:"<<endl;
		DebugOut(0)<<introspectionXml<<endl;

		g_error_free(error);

		return;
	}

	GDBusInterfaceInfo* mInterfaceInfo = g_dbus_node_info_lookup_interface(introspection, mInterfaceName.c_str());


	const GDBusInterfaceVTable vtable = { handleMyMethodCall, AbstractDBusInterface::getProperty, AbstractDBusInterface::setProperty };

	GError* error2=NULL;

	regId = g_dbus_connection_register_object(mConnection, mObjectPath.c_str(), mInterfaceInfo, &vtable, this, NULL, &error2);
	
	if(error2) throw -1;
	
	g_assert(regId > 0);
}

void AbstractDBusInterface::unregisterObject()
{
	g_dbus_connection_unregister_object(mConnection, regId);
}

void AbstractDBusInterface::updateValue(AbstractProperty *property)
{
	if(mConnection == nullptr)
	{
		return;
	}

	GError *error = NULL;

	GVariant **params = g_new(GVariant*,2);
	GVariant *val = g_variant_ref(property->toGVariant());
	params[0] = g_variant_new("v",val);
	params[1] = g_variant_new("d",property->timestamp());

	GVariant *tuple_variant = g_variant_new_tuple(params,2);

	g_dbus_connection_emit_signal(mConnection, NULL, mObjectPath.c_str(), mInterfaceName.c_str(), string(property->name() + "Changed").c_str(), tuple_variant, &error);

	if(error)
	{
		DebugOut(DebugOut::Error)<<error->message<<endl;
		g_error_free(error);
	}



	/// Send PropertiesChanged signal

	GVariantBuilder builder;
	g_variant_builder_init(&builder, G_VARIANT_TYPE_DICTIONARY);

	g_variant_builder_add(&builder, "{sv}", property->name().c_str(), val);
	g_variant_builder_add(&builder, "{sv}", "Time", g_variant_new("(d)", mTime) );

	GError *error2 = NULL;

	g_dbus_connection_emit_signal(mConnection, NULL, mObjectPath.c_str(), "org.freedesktop.DBus.Properties", "PropertiesChanged", g_variant_new("(sa{sv}as)",
																																				mInterfaceName.c_str(),
																																				&builder, NULL), &error2);
	g_variant_unref(val);

}

std::list<AbstractDBusInterface *> AbstractDBusInterface::getObjectsForProperty(string object)
{
	std::list<AbstractDBusInterface *> l;
	for(auto itr = interfaceMap.begin(); itr != interfaceMap.end(); itr++)
	{
		AbstractDBusInterface * interface = (*itr).second;
		if(interface->objectName() == object)
			l.push_back(interface);
	}

	return l;
}
list<AbstractDBusInterface *> AbstractDBusInterface::interfaces()
{
	std::list<AbstractDBusInterface*> ifaces;

	for(auto itr = interfaceMap.begin(); itr != interfaceMap.end(); itr++)
	{
		ifaces.push_back((*itr).second);
	}

	return ifaces;
}

bool AbstractDBusInterface::implementsProperty(string property)
{
	for(auto itr = properties.begin(); itr != properties.end(); itr++)
	{
		if((*itr).first == property)
		{
			return true;
		}
	}

	return false;
}

void AbstractDBusInterface::startRegistration()
{
	unregisterObject();
	introspectionXml ="<node>" ;
	introspectionXml +=
			"<interface name='"+ mInterfaceName + "' >"
			"<property type='i' name='Zone' access='read' />"
			"<property type='d' name='Time' access='read' />";
}

GVariant* AbstractDBusInterface::getProperty(GDBusConnection* connection, const gchar* sender, const gchar* objectPath, const gchar* interfaceName, const gchar* propertyName, GError** error, gpointer userData)
{
	std::string pn = propertyName;
	if(pn == "Time")
	{
		double time = interfaceMap[interfaceName]->time();

		GVariant* value = g_variant_new("(d)", time);
		return value;
	}

	if(pn == "Zone")
	{
		Zone::Type zone = interfaceMap[objectPath]->zone();

		GVariant* value = g_variant_new("(i)",(int)zone);
		return value;
	}

	if(interfaceMap.count(objectPath))
	{
		GVariant* value = interfaceMap[objectPath]->getProperty(propertyName);
		return value;
	}


	DebugOut(DebugOut::Error)<<"No interface for" << interfaceName <<endl;
	return nullptr;
}

gboolean AbstractDBusInterface::setProperty(GDBusConnection* connection, const gchar* sender, const gchar* objectPath, const gchar* interfaceName, const gchar* propertyName, GVariant* value, GError** error, gpointer userData)
{
	if(interfaceMap.count(objectPath))
	{
		interfaceMap[objectPath]->setProperty(propertyName, value);
		return true;
	}

	return false;
}

void AbstractDBusInterface::setProperty(string propertyName, GVariant *value)
{
	if(properties.count(propertyName))
	{
		properties[propertyName]->fromGVariant(value);
	}
	else
	{
		throw -1;
	}
}

GVariant *AbstractDBusInterface::getProperty(string propertyName)
{
	if(properties.count(propertyName))
		return properties[propertyName]->toGVariant();
	else
		throw -1;
}

