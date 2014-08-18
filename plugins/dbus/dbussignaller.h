#ifndef _DBUSSIGNALLER_H_
#define _DBUSSIGNALLER_H_

#include <gio/gio.h>

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#include "debugout.h"
#include "abstractproperty.h"
#include "superptr.hpp"
#include "listplusplus.h"

class DBusSignal
{
public:
	DBusSignal():connection(nullptr), property(nullptr){}
	DBusSignal(GDBusConnection* conn, const std::string & objPath, const std::string & iface, const std::string & sigName, AbstractProperty* var)
		: connection(conn), objectPath(objPath), interface(iface), signalName(sigName), property(var)
	{

	}

	~DBusSignal()
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

		if(queue.find(objPath) != queue.end())
		{
			bool isFound = false;
			for(auto i : queue[objPath])
			{
				if(*i == *signal)
				{
					isFound = true;
					break;
				}
			}

			if(!isFound)
				queue[objPath].push_back(signal);
		}
		else
		{
			queue[objPath].push_back(signal);
		}
	}

private:

	DBusSignaller() { }

	DBusSignaller(int timeout)
	{
		g_timeout_add(timeout,[](gpointer userData)
		{
			std::unordered_map<std::string, std::vector<DBusSignal*>> *q = static_cast<std::unordered_map<std::string, std::vector<DBusSignal*>>*>(userData);
			std::unordered_map<std::string, std::vector<DBusSignal*>> queue = *q;

			for(auto itr : queue)
			{
				std::string objectPath;
				GDBusConnection* connection;
				std::string interfaceName;
				std::string signalName;

				std::unordered_map<std::string, GVariant*> variantMap;

				for(auto s : itr.second)
				{
					std::unique_ptr<DBusSignal> signal(s);
					objectPath = signal->objectPath;
					connection = signal->connection;
					interfaceName = signal->interface;
					signalName = signal->signalName;

					AbstractProperty* property = signal->property;

					auto val = g_variant_ref(property->toGVariant());
					std::string sequenceName = property->name() + "Sequence";

					variantMap[property->name()] = val;
					variantMap[sequenceName] = g_variant_new("i", property->sequence());
					variantMap["Time"] = g_variant_new("d", property->timestamp());
					variantMap["Zone"] = g_variant_new("i", property->value()->zone);
				}

				GVariantBuilder builder;
				g_variant_builder_init(&builder, G_VARIANT_TYPE_DICTIONARY);

				for(auto sv : variantMap)
				{
					/// Send PropertiesChanged signal
					auto key = sv.first;
					auto value = sv.second;

					g_variant_builder_add(&builder, "{sv}", key.c_str(), value);
				}

				GError* error = nullptr;

				g_dbus_connection_emit_signal(connection, NULL, objectPath.c_str(),
											  "org.freedesktop.DBus.Properties",
											  signalName.c_str(),
											  g_variant_new("(sa{sv}as)", interfaceName.c_str(), &builder, NULL),
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

	std::unordered_map<std::string, std::vector<DBusSignal*>> queue;

	static DBusSignaller * singleton;
};

#endif
