#ifndef _DBUSSIGNALLER_H_
#define _DBUSSIGNALLER_H_

#include <gio/gio.h>
#include <map>
#include <string>

#include "debugout.h"
#include "abstractproperty.h"

class DBusSignaller
{
public:

	static DBusSignaller* factory(int timeout)
	{
		if(!singleton)
			return singleton = new DBusSignaller(timeout);
		return singleton;
	}

	void fireSignal(GDBusConnection* conn, std::string objPath, std::string iface, std::string sigName, AbstractProperty* prop)
	{
		DBusSignal signal(conn, objPath, iface, sigName, prop);

		queue[objPath] = signal;
	}

	class DBusSignal
	{
	public:
		DBusSignal():connection(nullptr), property(nullptr){}
		DBusSignal(GDBusConnection* conn, std::string objPath, std::string iface, std::string sigName, AbstractProperty* var)
			: connection(conn), objectPath(objPath), interface(iface), signalName(sigName), property(var)
		{

		}

		GDBusConnection* connection;
		std::string objectPath;
		std::string interface;
		std::string signalName;
		AbstractProperty* property;
	};

private:

	DBusSignaller() { }

	DBusSignaller(int timeout)
	{
		g_timeout_add(timeout,[](gpointer userData)
		{
			std::map<std::string, DBusSignal> *q = static_cast<std::map<std::string, DBusSignal>*>(userData);
			std::map<std::string, DBusSignal> queue = *q;

			for(auto sig : queue)
			{
				DBusSignal signal = sig.second;

				GError* error = nullptr;

				AbstractProperty* property = signal.property;

				GVariant* val = g_variant_ref(property->toGVariant());

				/// Send PropertiesChanged signal

				GVariantBuilder builder;
				g_variant_builder_init(&builder, G_VARIANT_TYPE_DICTIONARY);

				g_variant_builder_add(&builder, "{sv}", property->name().c_str(), val);
				g_variant_builder_add(&builder, "{sv}", std::string(property->name() + "Sequence").c_str(), g_variant_new("i", property->sequence()));
				g_variant_builder_add(&builder, "{sv}", "Time", g_variant_new("d", property->timestamp()) );

				g_dbus_connection_emit_signal(signal.connection, NULL, signal.objectPath.c_str(), signal.interface.c_str(), signal.signalName.c_str(), g_variant_new("(sa{sv}as)",
																																									 signal.interface.c_str(),
																																									 &builder, NULL), &error);

				if(error)
				{
					DebugOut(DebugOut::Error)<<error->message<<std::endl;
				}

				queue.erase(signal.objectPath);

			}

			*q = queue;

			return 1;

		},&queue);
	}

	std::map<std::string, DBusSignal> queue;

	static DBusSignaller * singleton;
};

#endif
