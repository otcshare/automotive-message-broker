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

#include <abstractroutingengine.h>
#include <debugout.h>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <gio/gio.h>
#include <listplusplus.h>

#include "varianttype.h"
#include "dbussignaller.h"

static DBusSignaller* signaller = nullptr;

unordered_map<string, AbstractDBusInterface*> AbstractDBusInterface::objectMap;
PropertyList AbstractDBusInterface::mimplementedProperties;

const uint getPid(const char *owner)
{
	GError* error = nullptr;
	GDBusProxy* dbus = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM, G_DBUS_PROXY_FLAGS_NONE, NULL,
													 "org.freedesktop.DBus",
													 "/",
													 "org.freedesktop.DBus",
													 NULL,
													 &error);

	if(error)
	{
		throw std::runtime_error(error->message);
	}

	error = nullptr;

	GVariant* pid = g_dbus_proxy_call_sync(dbus, "GetConnectionUnixProcessID", g_variant_new("(s)", owner), G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);

	if(error)
	{
		DebugOut(DebugOut::Error)<< error->message << endl;
	}

	uint thePid=0;

	g_variant_get(pid,"(u)",&thePid);

	return thePid;
}

void AbstractDBusInterface::handleMyMethodCall(GDBusConnection       *connection,
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

	if(DebugOut::getDebugThreshhold() >= 6)
	{
		DebugOut(6)<<"DBus method call from: "<<sender<< " pid: " <<getPid(sender)<< " interface: "<<interface_name<<" method: "<<method<<endl;
		DebugOut(6)<<"DBus method call path: "<<object_path<<endl;
	}

	g_assert(iface);

	if(std::string(interface_name) == "org.freedesktop.DBus.Properties")
	{
		if(method == "Get")
		{
			gchar* propertyName = nullptr;
			gchar* ifaceName = nullptr;
			g_variant_get(parameters, "(ss)", &ifaceName, &propertyName);

			DebugOut(6) << "Parameter signature: " << g_variant_get_type_string(parameters) << endl;
//			DebugOut(6) << "Get property " << propertyName << " for interface " << ifaceName << endl;

			GError* error = nullptr;
			auto value = AbstractDBusInterface::getProperty(connection, sender, object_path, ifaceName, propertyName, &error, iface);
			amb::make_super(error);

			if(!value)
			{
				g_dbus_method_invocation_return_dbus_error(invocation, std::string(std::string(ifaceName)+".PropertyNotFound").c_str(), "Property not found in interface");
			}

			g_dbus_method_invocation_return_value(invocation, g_variant_new("(v)", value));
			return;
		}
		else if(method == "GetAll")
		{
			gchar* ifaceName = nullptr;
			g_variant_get(parameters, "(s)", &ifaceName);

			GVariantBuilder builder;
			g_variant_builder_init(&builder, G_VARIANT_TYPE("a{sv}"));

			auto propertyMap = iface->getProperties();

			for(auto itr : propertyMap)
			{
				auto prop = itr.second;
				GError* error = nullptr;
				auto value = AbstractDBusInterface::getProperty(connection, sender, object_path, ifaceName, prop->name().c_str(), &error, iface);
				amb::make_super(error);
				g_variant_builder_add(&builder, "{sv}", prop->name().c_str(), g_variant_new("v", value));
				//sequence
				auto sequence = AbstractDBusInterface::getProperty(connection, sender, object_path, ifaceName, std::string(prop->name()+"Sequence").c_str(), &error, iface);
				g_variant_builder_add(&builder, "{sv}", std::string(prop->name()+"Sequence").c_str(), g_variant_new("v", sequence));

				auto quality = AbstractDBusInterface::getProperty(connection, sender, object_path, ifaceName, std::string(prop->name()+"ValueQuality").c_str(), &error, iface);
				g_variant_builder_add(&builder, "{sv}", std::string(prop->name()+"ValueQuality").c_str(), g_variant_new("v", quality));

				auto freq = AbstractDBusInterface::getProperty(connection, sender, object_path, ifaceName, std::string(prop->name()+"UpdateFrequency").c_str(), &error, iface);
				g_variant_builder_add(&builder, "{sv}", std::string(prop->name()+"UpdateFrequency").c_str(), g_variant_new("v", freq));
			}

			auto time = AbstractDBusInterface::getProperty(connection, sender, object_path, ifaceName, "Time", nullptr, iface);
			auto zone = AbstractDBusInterface::getProperty(connection, sender, object_path, ifaceName, "Zone", nullptr, iface);
			g_variant_builder_add(&builder, "{sv}", "Time", g_variant_new("v", time));
			g_variant_builder_add(&builder, "{sv}", "Zone", g_variant_new("v", zone));

			g_dbus_method_invocation_return_value(invocation, g_variant_new("(a{sv})", &builder));
			return;
		}
		else if(method == "Set")
		{
			gchar* ifaceName = nullptr;
			gchar* propName = nullptr;
			GVariant* value;
			g_variant_get(parameters, "(ssv)", &ifaceName, &propName, &value);

			AbstractDBusInterface::setProperty(connection, sender, object_path, ifaceName, propName, value, nullptr, iface,
											   [&invocation, &ifaceName](bool success, AsyncPropertyReply::Error error) {
				if(success)
				{
					g_dbus_method_invocation_return_value(invocation, nullptr);
				}
				else
				{
					g_dbus_method_invocation_return_dbus_error(invocation, ifaceName, AsyncPropertyReply::errorToStr(error).c_str());
				}
			});
			return;
		}
	}
	else if(method == "GetHistory")
	{
		double beginTime = 0;
		double endTime = 0;

		g_variant_get(parameters, "(dd)", &beginTime, &endTime);

		auto propertyMap = iface->getProperties();

		PropertyList propertyList;

		for(auto itr = propertyMap.begin(); itr != propertyMap.end(); itr++)
		{
			VariantType* prop = (*itr).second;

			if(!contains(propertyList, prop->ambPropertyName()))
				propertyList.push_back(prop->ambPropertyName());
		}

		std::string ifaceName = iface->interfaceName();

		AsyncRangePropertyRequest request;

		request.properties = propertyList;
		request.timeBegin = beginTime;
		request.timeEnd = endTime;
		request.zone = iface->zone();
		//request.sourceUuid = iface->source();

		request.completed = [&invocation,&ifaceName](AsyncRangePropertyReply* r)
		{
			auto reply = amb::make_unique(r);
			if(!reply->success)
			{
				stringstream str;
				str<<"Error during request: "<<AsyncPropertyReply::errorToStr(reply->error);
				ifaceName += ".Error";
				g_dbus_method_invocation_return_dbus_error(invocation, ifaceName.c_str(), str.str().c_str());
				return;
			}

			if(!reply->values.size())
			{
				ifaceName += ".Error";
				g_dbus_method_invocation_return_dbus_error(invocation, ifaceName.c_str(), "No results");
				return;
			}

			GVariantBuilder builder;
			g_variant_builder_init(&builder, G_VARIANT_TYPE("a{svd}"));


			for(auto itr = reply->values.begin(); itr != reply->values.end(); itr++)
			{
				AbstractPropertyType* value = *itr;

				g_variant_builder_add(&builder, "{svd}", value->name.c_str(), g_variant_ref(value->toVariant()),value->timestamp);
			}

			g_dbus_method_invocation_return_value(invocation,g_variant_new("(a{svd})",&builder));
		};

		iface->re->getRangePropertyAsync(request);

		return;
	}

	g_dbus_method_invocation_return_error(invocation,G_DBUS_ERROR,G_DBUS_ERROR_UNKNOWN_METHOD, "Unknown method.");
}

AbstractDBusInterface::AbstractDBusInterface(string interfaceName, string objectName,
											 GDBusConnection* connection)
	: mInterfaceName(interfaceName), mConnection(connection), mPropertyName(objectName), supported(false), zoneFilter(Zone::None), mTime(0), regId(0)
{
	startRegistration();

	mObjectPath = "/" + objectName;
}

AbstractDBusInterface::~AbstractDBusInterface()
{
	unregisterObject();

	PropertyList impl = implementedProperties();

	for(auto itr = impl.begin(); itr != impl.end(); itr++)
	{
		if(properties.find(*itr) != properties.end())
		{
			// Deleted in ~DBusSink()
			//delete properties[*itr];
			properties.erase(*itr);
		}
	}

	objectMap.erase(mObjectPath);
}

void AbstractDBusInterface::addProperty(VariantType * property)
{
	string nameToLower = property->name();
	boost::algorithm::to_lower<string>(nameToLower);

	string access;

	if(property->access() == VariantType::Read)
		access = "read";
	else if(property->access() == VariantType::Write)
		access = "write";
	else if(property->access() == VariantType::ReadWrite)
		access = "readwrite";
	else throw -1; //FIXME: don't throw

	std::string pn = property->name();

	///see which properties are supported:
	introspectionXml +=
			"<property type='"+ string(property->signature()) + "' name='"+ pn +"' access='"+access+"' />"
			"<property type='i' name='" + pn + "Sequence' access='read' />"
			"<property type='i' name='" + pn + "ValueQuality' access='read' />"
			"<property type='i' name='" + pn + "UpdateFrequency' access='read' />";

	properties[pn] = property;

	if(!contains(mimplementedProperties, property->ambPropertyName()))
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

	if(!boost::algorithm::ends_with(introspectionXml,"</node>"))
	{
		introspectionXml += "</interface>"
				"</node>";
	}

	GError* error=NULL;

	GDBusNodeInfo* introspection = g_dbus_node_info_new_for_xml(introspectionXml.c_str(), &error);

	if(!introspection || error)
	{

		DebugOut(DebugOut::Error)<<"Error in "<<__FILE__<<" - "<<__FUNCTION__<<":"<<__LINE__<<endl;
		DebugOut(DebugOut::Error)<<error->message<<endl;
		DebugOut(DebugOut::Error)<<"probably bad xml:"<<endl;
		DebugOut(DebugOut::Error)<<introspectionXml<<endl;

		g_error_free(error);

		return;
	}

	GDBusInterfaceInfo* mInterfaceInfo = g_dbus_node_info_lookup_interface(introspection, mInterfaceName.c_str());


	const GDBusInterfaceVTable vtable = { handleMyMethodCall, nullptr, nullptr };

	GError* error2=NULL;

	DebugOut()<<"registering DBus path: "<<mObjectPath<<endl;

	regId = g_dbus_connection_register_object(mConnection, mObjectPath.c_str(), mInterfaceInfo, &vtable, this, NULL, &error2);
	g_dbus_node_info_unref(introspection);
	if(error2)
	{
		DebugOut(DebugOut::Error)<<error2->message<<endl;
		g_error_free(error2);
	}

	if(regId == 0)
	{
		DebugOut(DebugOut::Error)<<"We failed to register on DBus"<<endl;
	}
}

void AbstractDBusInterface::unregisterObject()
{
	if(regId)
	{
		DebugOut()<<__FUNCTION__<<endl;
		g_dbus_connection_unregister_object(mConnection, regId);
	}

	regId=0;
}

void AbstractDBusInterface::updateValue(VariantType *property)
{
	if(mConnection == nullptr)
	{
		return;
	}

	if(isRegistered())
		signaller->fireSignal(mConnection, mObjectPath, mInterfaceName, "PropertiesChanged", property);
}

std::list<AbstractDBusInterface *> AbstractDBusInterface::getObjectsForProperty(string object)
{
	std::list<AbstractDBusInterface *> l;
	for(auto itr = objectMap.begin(); itr != objectMap.end(); itr++)
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

	for(auto itr = objectMap.begin(); itr != objectMap.end(); itr++)
	{
		ifaces.push_back((*itr).second);
	}

	return ifaces;
}

std::vector<std::string> AbstractDBusInterface::supportedInterfaces()
{
	std::vector<std::string> ifaces;

	for(auto itr : objectMap)
	{
		if(itr.second->isSupported())
			ifaces.push_back(itr.second->objectName());
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
	//unregisterObject();
	introspectionXml ="<node>" ;
	introspectionXml +=
			"<interface name='org.freedesktop.DBus.Properties'>"
			"<method name='Get'>"
			"   <arg type='s' direction='in' name='interface' />"
			"   <arg type='s' direction='in' name='property' />"
			"   <arg type='v' direction='out' name='value' />"
			"</method>"
			"<method name='Set'>"
			"   <arg type='s' direction='in' name='interface' />"
			"   <arg type='s' direction='in' name='property' />"
			"   <arg type='v' direction='in' name='value' />"
			"</method>"
			"<method name='GetAll'>"
			"   <arg type='s' direction='in' name='interface' />"
			"   <arg type='a{sv}' direction='out' name='interface' />"
			"</method>"
			"</interface>";
	introspectionXml +=
			"<interface name='"+ mInterfaceName + "' >"
			"<property type='i' name='Zone' access='read' />"
			"<property type='d' name='Time' access='read' />"
			"<method name='GetHistory'>"
			"	<arg type='d' direction='in' name='beginTimestamp' />"
			"	<arg type='d' direction='in' name='endTimestamp' />"
			"   <arg type='a(svd)' direction='out' name='result' />"
			"</method>";
}

GVariant* AbstractDBusInterface::getProperty(GDBusConnection* connection, const gchar* sender, const gchar* objectPath, const gchar* interfaceName,
											 const gchar* propertyName, GError** error, gpointer userData)
{
	if(DebugOut::getDebugThreshhold() >= 6)
	{
		DebugOut(6)<<"DBus GetProperty call from: "<<sender<< " pid: " <<getPid(sender)<< " interface: "<<interfaceName<<" property: "<<propertyName<<endl;
		DebugOut(6)<<"DBus GetProperty call path: "<<objectPath<<endl;
	}

	std::string pn = propertyName;
	if(pn == "Time")
	{
		if(objectMap.find(objectPath) == objectMap.end())
		{
			DebugOut(DebugOut::Error)<<objectPath<<" is not a valid object path."<<endl;
			return nullptr;
		}
		double time = objectMap[objectPath]->time();

		GVariant* value = g_variant_new("d", time);
		return value;
	}

	else if(boost::ends_with(pn, "Sequence"))
	{
		AbstractDBusInterface* t = static_cast<AbstractDBusInterface*>(userData);

		int pos = pn.find("Sequence");

		std::string p = pn.substr(0,pos);

		VariantType * theProperty = t->property(p);

		if(!theProperty)
		{
			DebugOut(DebugOut::Error)<<"Invalid Sequence property: "<<p<<endl;
			return nullptr;
		}

		int sequence = theProperty->sequence();

		GVariant* value = g_variant_new("i", sequence);
		return value;
	}

	else if(boost::ends_with(pn, "ValueQuality"))
	{
		AbstractDBusInterface* t = static_cast<AbstractDBusInterface*>(userData);

		int pos = pn.find("ValueQuality");

		std::string p = pn.substr(0, pos);

		VariantType * theProperty = t->property(p);

		if(!theProperty)
		{
			DebugOut(DebugOut::Error)<<"Invalid ValueQuality property: "<<p<<endl;
			return nullptr;
		}

		int quality = theProperty->value()->valueQuality;

		GVariant* value = g_variant_new("i", quality);
		return value;
	}

	else if(boost::ends_with(pn, "UpdateFrequency"))
	{
		AbstractDBusInterface* t = static_cast<AbstractDBusInterface*>(userData);

		int pos = pn.find("UpdateFrequency");

		std::string p = pn.substr(0, pos);

		VariantType * theProperty = t->property(p);

		if(!theProperty)
		{
			DebugOut(DebugOut::Error)<<"Invalid UpdateFrequency property: "<<p<<endl;
			return nullptr;
		}

		int freq = theProperty->updateFrequency();

		GVariant* value = g_variant_new("i", freq);
		return value;
	}

	else if(pn == "Zone")
	{
		if(objectMap.find(objectPath) == objectMap.end())
		{
			DebugOut(DebugOut::Error)<<objectPath<<" is not a valid object path."<<endl;
			return nullptr;
		}

		Zone::Type zone = objectMap[objectPath]->zone();

		GVariant* value = g_variant_new("i",(int)zone);
		return value;
	}

	if(objectMap.count(objectPath))
	{
		GVariant* value = objectMap[objectPath]->getProperty(propertyName);

		DebugOut(6) << "Returning value for: " << propertyName << endl;
		return value;
	}


	DebugOut(DebugOut::Error)<<"No interface for" << interfaceName <<endl;
	return nullptr;
}

gboolean AbstractDBusInterface::setProperty(GDBusConnection* connection, const gchar* sender, const gchar* objectPath, const gchar* interfaceName,
											const gchar* propertyName, GVariant* value, GError** error, gpointer userData,
											std::function<void (bool, AsyncPropertyReply::Error)> callback)
{
	if(DebugOut::getDebugThreshhold() >= 6)
	{
		DebugOut(6)<<"DBus SetProperty call from: "<<sender<< " pid: " <<getPid(sender)<< " interface: "<<interfaceName<<" property: "<<propertyName<<endl;
		DebugOut(6)<<"DBus SetProperty call path: "<<objectPath<<endl;
	}

	if(objectMap.count(objectPath))
	{
		objectMap[objectPath]->setProperty(propertyName, value, callback);
		return true;
	}

	return false;
}

void AbstractDBusInterface::setProperty(string propertyName, GVariant *value, std::function<void (bool, AsyncPropertyReply::Error)> callback)
{
	if(properties.count(propertyName))
	{
		properties[propertyName]->fromVariant(value, callback);
	}
	else if(callback)
	{
		callback(false, AsyncPropertyReply::InvalidOperation);
	}
}

GVariant *AbstractDBusInterface::getProperty(string propertyName)
{
	if(properties.count(propertyName))
		return properties[propertyName]->toVariant();

	return nullptr;
}

void AbstractDBusInterface::setTimeout(int timeout)
{
	if(!signaller)
		signaller = DBusSignaller::factory(timeout);
}

