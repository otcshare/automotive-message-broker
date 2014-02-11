#include "bluetooth5.h"
#include "debugout.h"
#include <gio/gio.h>
#include <string>

std::string findAdapterPath(std::string adapterAddy)
{
	std::string adapterObjectPath;

	GError * error = nullptr;
	GDBusProxy * managerProxy = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM, G_DBUS_PROXY_FLAGS_NONE, NULL,
															  "org.bluez",
															  "/",
															  "org.freedesktop.DBus.ObjectManager",
															  nullptr,&error);

	if(error)
	{
		DebugOut(DebugOut::Error)<<"Could not create ObjectManager proxy for Bluez: "<<error->message<<endl;
		g_error_free(error);
		return "";
	}

	GVariant * objectMap = g_dbus_proxy_call_sync(managerProxy, "GetManagedObjects",nullptr, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);

	if(error)
	{
		DebugOut(DebugOut::Error)<<"Failed call to GetManagedObjects: "<<error->message<<endl;
		g_object_unref(managerProxy);
		g_error_free(error);
		return "";
	}

	GVariantIter* iter;
	char* objPath;
	GVariantIter* level2Dict;

	g_variant_get(objectMap, "a{oa{sa{sv}}}",&iter);

	while(g_variant_iter_next(iter, "oa{sa{sv}}}",&objPath, &level2Dict))
	{
		char * interfaceName;
		GVariantIter* innerDict;
		while(g_variant_iter_next(level2Dict, "sa{sv}", &interfaceName, &innerDict))
		{
			if(std::string(interfaceName) == "org.bluez.Adapter1")
			{
				char* propertyName;
				GVariant* value;

				while(adapterObjectPath == "" && g_variant_iter_next(innerDict,"sv", &propertyName, &value))
				{
					if(std::string(propertyName) == "Address")
					{
						char* addy;
						g_variant_get(value,"s",&addy);

						if(adapterAddy == "" || addy && std::string(addy) == adapterAddy)
						{
							adapterObjectPath = objPath;
						}

						g_free(addy);
					}
					g_free(propertyName);
					g_variant_unref(value);
				}
			}
			g_free(interfaceName);
			g_variant_iter_free(innerDict);
		}
		g_free(objPath);
		g_variant_iter_free(level2Dict);
	}
	g_variant_iter_free(iter);

	return adapterObjectPath;
}

std::string findDevice(std::string addy, std::string adapterPath)
{
	std::string objectPath;

	GError * error = nullptr;
	GDBusProxy * managerProxy = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM, G_DBUS_PROXY_FLAGS_NONE, NULL,
															  "org.bluez",
															  "/",
															  "org.freedesktop.DBus.ObjectManager",
															  nullptr,&error);

	if(error)
	{
		DebugOut(DebugOut::Error)<<"Could not create ObjectManager proxy for Bluez: "<<error->message<<endl;
		g_error_free(error);
		return "";
	}

	GVariant * objectMap = g_dbus_proxy_call_sync(managerProxy, "GetManagedObjects",nullptr, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);

	if(error)
	{
		DebugOut(DebugOut::Error)<<"Failed call to GetManagedObjects: "<<error->message<<endl;
		g_object_unref(managerProxy);
		g_error_free(error);
		return "";
	}

	GVariantIter* iter;
	char* objPath;
	GVariantIter* level2Dict;

	g_variant_get(objectMap, "a{oa{sa{sv}}}",&iter);

	while(g_variant_iter_next(iter, "oa{sa{sv}}}",&objPath, &level2Dict))
	{
		char * interfaceName;
		GVariantIter* innerDict;
		while(g_variant_iter_next(level2Dict, "sa{sv}", &interfaceName, &innerDict))
		{
			if(std::string(interfaceName) == "org.bluez.Device1")
			{
				char* propertyName;
				GVariant* value;

				while(objectPath == "" && g_variant_iter_next(innerDict,"sv", &propertyName, &value))
				{
					if(std::string(propertyName) == "Address")
					{
						char* addy;
						g_variant_get(value,"s",&addy);

						if(adapterPath == "" || addy && std::string(addy) == adapterPath)
						{
							objectPath = objPath;
						}

						g_free(addy);
					}
					///TODO: filter only devices that have the specified adapter
					g_free(propertyName);
					g_variant_unref(value);
				}
			}
			g_free(interfaceName);
			g_variant_iter_free(innerDict);
		}
		g_free(objPath);
		g_variant_iter_free(level2Dict);
	}
	g_variant_iter_free(iter);

	return objectPath;
}

std::string Bluetooth5::getDeviceForAddress(std::string address, std::string adapterAddy)
{
	std::string result;

	std::string adapterPath = findAdapterPath(adapterAddy);

	if(adapterPath == "")
	{
		DebugOut(DebugOut::Error)<<"Adapter not found ("<<adapterAddy<<")"<<endl;
		return "";
	}

	std::string devicePath = findDevice(address, adapterPath);



	return result;
}
