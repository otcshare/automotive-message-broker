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

#include "listplusplus.h"
#include "abstractproperty.h"

unordered_map<string, AbstractDBusInterface*> AbstractDBusInterface::interfaceMap;
list<string> AbstractDBusInterface::mimplementedProperties;


static void handleMethodCall(GDBusConnection       *connection,
							 const gchar           *sender,
							 const gchar           *object_path,
							 const gchar           *interface_name,
							 const gchar           *method_name,
							 GVariant              *parameters,
							 GDBusMethodInvocation *invocation,
							 gpointer               user_data)
{
	AbstractDBusInterface* iface = static_cast<AbstractDBusInterface*>(user_data);

	std::string method = method_name;
	if(method == "getHistory")
	{
		double beginTime = 0;
		double endTime = 0;

		g_variant_get(parameters, "(dd)", &beginTime, &endTime);

		auto propertyList = iface->getAmbProperties();

//		auto cb = [&invocation](std::list<>){};

		for(auto itr = propertyList.begin(); itr != propertyList.end(); itr++)
		{

		}

		//AsyncRangePropertyRequest
	}

}

AbstractDBusInterface::AbstractDBusInterface(string interfaceName, string op,
											 GDBusConnection* connection)
	: mInterfaceName(interfaceName), mObjectPath(op), mConnection(connection), supported(false), mTime(0)
{
	interfaceMap[interfaceName] = this;
	startRegistration();
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

	///see which properties are supported:
	introspectionXml += 	"<property type='"+ property->signature() + "' name='"+ property->name()+"' access='"+access+"' />"
			"<signal name='" + property->name() + "Changed' >"
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


	const GDBusInterfaceVTable vtable = { handleMethodCall, AbstractDBusInterface::getProperty, AbstractDBusInterface::setProperty };

	regId = g_dbus_connection_register_object(mConnection, mObjectPath.c_str(), mInterfaceInfo, &vtable, this, NULL, &error);
	
	if(error) throw -1;
	
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
	}

	g_free(params);

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

AbstractDBusInterface *AbstractDBusInterface::getInterfaceForObject(string object)
{
	std::string iface = "org.automotive." + object;

	if(interfaceMap.find(iface) != interfaceMap.end())
	{
		return interfaceMap[iface];
	}

	return nullptr;
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

std::list<std::string> AbstractDBusInterface::getAmbProperties()
{
	std::list<std::string> list;

	for(auto itr = properties.begin(); itr != properties.end(); itr++)
	{
		AbstractProperty* prop = (*itr).second;
		list.push_back(prop->ambPropertyName());
	}

	return list;
}

void AbstractDBusInterface::startRegistration()
{
	unregisterObject();
	introspectionXml ="<node>" ;
	introspectionXml += "<interface name='"+ mInterfaceName + "' >"

			"<property type='d' name='Time' access='read' />"
			"<method name='getHistory'>"
			"	<arg type='d' direction='in' name='beginTimestamp' />"
			"	<arg type='d' direction='in' name='endTimestamp' />"
			"</method>";
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

	if(interfaceMap.count(interfaceName))
	{
		GVariant* value = interfaceMap[interfaceName]->getProperty(propertyName);
		return value;

	}

	DebugOut(DebugOut::Error)<<"No interface for" + string(interfaceName)<<endl;
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

