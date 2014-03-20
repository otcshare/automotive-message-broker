#ifndef _DBUSSIGNALLER_H_
#define _DBUSSIGNALLER_H_

#include <gio/gio.h>
#include <unordered_set>
#include <string>

#include "debugout.h"
#include "abstractproperty.h"

class DBusSignal;

namespace std {
  template <>
  struct hash<DBusSignal>
  {
	typedef DBusSignal   argument_type;
	typedef std::size_t  result_type;

	result_type operator()(const DBusSignal & t) const
	{
	  std::size_t val { 0 };
	  return val;
	}
  };
}

class DBusSignal
{
public:
	DBusSignal():connection(nullptr), property(nullptr){}
	DBusSignal(GDBusConnection* conn, std::string objPath, std::string iface, std::string sigName, AbstractProperty* var)
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

	void fireSignal(GDBusConnection* conn, std::string objPath, std::string iface, std::string sigName, AbstractProperty* prop)
	{
		DBusSignal signal(conn, objPath, iface, sigName, prop);

		queue.insert(signal);
	}

private:

	DBusSignaller() { }

	DBusSignaller(int timeout)
	{
		g_timeout_add(timeout,[](gpointer userData)
		{
			std::unordered_set<DBusSignal> *q = static_cast<std::unordered_set< DBusSignal>*>(userData);
			std::unordered_set<DBusSignal> queue = *q;

			for(auto s : queue)
			{
				DBusSignal signal = s;

				GError* error = nullptr;

				AbstractProperty* property = signal.property;

				GVariant* val = g_variant_ref(property->toGVariant());

				/// Send PropertiesChanged signal

				GVariantBuilder builder;
				g_variant_builder_init(&builder, G_VARIANT_TYPE_DICTIONARY);

				g_variant_builder_add(&builder, "{sv}", property->name().c_str(), val);
				g_variant_builder_add(&builder, "{sv}", std::string(property->name() + "Sequence").c_str(), g_variant_new("i", property->sequence()));
				g_variant_builder_add(&builder, "{sv}", "Time", g_variant_new("d", property->timestamp()) );
				g_variant_builder_add(&builder, "{sv}", "Zone", g_variant_new("i", property->value()->zone) );

				g_dbus_connection_emit_signal(signal.connection, NULL, signal.objectPath.c_str(), "org.freedesktop.DBus.Properties", signal.signalName.c_str(), g_variant_new("(sa{sv}as)",
																																									 signal.interface.c_str(),
																																						 &builder, NULL), &error);

				if(error)
				{
					DebugOut(DebugOut::Error)<<error->message<<std::endl;
				}

				queue.erase(signal);

			}

			*q = queue;

			return 1;

		},&queue);
	}

	std::unordered_set<DBusSignal> queue;

	static DBusSignaller * singleton;
};

#endif
