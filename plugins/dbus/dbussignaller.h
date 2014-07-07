#ifndef _DBUSSIGNALLER_H_
#define _DBUSSIGNALLER_H_

#include <gio/gio.h>

#include <vector>
#include <string>
#include <memory>

#include "debugout.h"
#include "abstractproperty.h"
#include "superptr.hpp"

class DBusSignal
{
public:
	DBusSignal():connection(nullptr), property(nullptr){}
	DBusSignal(GDBusConnection* conn, const std::string & objPath, const std::string & iface, const std::string & sigName, AbstractProperty* var)
		: connection(conn), objectPath(objPath), interface(iface), signalName(sigName), property(var)
	{

	}

	bool operator == (const DBusSignal &other) const
	{
		return connection == other.connection &&
				objectPath == other.objectPath &&
				interface == other.interface &&
				signalName == other.signalName &&
				property == other.property;
	}

	GDBusConnection* connection;
	std::string objectPath;
	std::string interface;
	std::string signalName;
	AbstractProperty* property;
};

class DBusSignaller
{
public:

	static DBusSignaller* factory(int timeout)
	{
		if(!singleton)
			return singleton = new DBusSignaller(timeout);
		return singleton;
	}

	void fireSignal(GDBusConnection* conn, const std::string & objPath, const std::string & iface,
					const std::string & sigName, AbstractProperty* prop)
	{
		DBusSignal * signal = new DBusSignal(conn, objPath, iface, sigName, prop);

		bool isFound = false;
		for(auto i : queue)
		{
			if(*i == *signal)
			{
				isFound = true;
				break;
			}
		}

		if(!isFound)
			queue.push_back(signal);
	}

private:

	DBusSignaller() { }

	DBusSignaller(int timeout)
	{
		g_timeout_add(timeout,[](gpointer userData)
		{
			std::vector<DBusSignal*> *q = static_cast<std::vector< DBusSignal*>*>(userData);
			std::vector<DBusSignal*> queue = *q;

			for(auto s : queue)
			{
				std::unique_ptr<DBusSignal> signal(s);

				GError* error = nullptr;

				AbstractProperty* property = signal->property;

				GVariant* val = g_variant_ref(property->toGVariant());

				/// Send PropertiesChanged signal

				GVariantBuilder builder;
				g_variant_builder_init(&builder, G_VARIANT_TYPE_DICTIONARY);

				g_variant_builder_add(&builder, "{sv}", property->name().c_str(), val);
				g_variant_builder_add(&builder, "{sv}", std::string(property->name() + "Sequence").c_str(), g_variant_new("i", property->sequence()));
				g_variant_builder_add(&builder, "{sv}", "Time", g_variant_new("d", property->timestamp()) );
				g_variant_builder_add(&builder, "{sv}", "Zone", g_variant_new("i", property->value()->zone) );

				g_dbus_connection_emit_signal(signal->connection, NULL, signal->objectPath.c_str(),
											  "org.freedesktop.DBus.Properties",
											  signal->signalName.c_str(),
											  g_variant_new("(sa{sv}as)", signal->interface.c_str(), &builder, NULL),
											  &error);

				auto errorPtr = amb::make_super(error);

				if(errorPtr)
				{
					DebugOut(DebugOut::Error)<<errorPtr->message<<std::endl;
				}
			}

			q->clear();

			return 1;

		},&queue);
	}

	std::vector<DBusSignal*> queue;

	static DBusSignaller * singleton;
};

#endif
