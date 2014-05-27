#include "bluetooth5.h"
#include "debugout.h"
#include <gio/gio.h>
#include <string>

static const gchar introspection_xml[] =
  "<node>"
  "  <interface name='org.bluez.Profile1'>"
  "    <method name='Release'>"
  "    </method>"
  "    <method name='NewConnection'>"
  "      <arg type='o' name='device' direction='in'/>"
  "      <arg type='h' name='fd' direction='in'/>"
  "      <arg type='a{sv}' name='fd_properties' direction='in'/>"
  "    </method>"
  "    <method name='RequestDisconnection'>"
  "      <arg type='o' name='device' direction='in'/>"
  "    </method>"
  "  </interface>"
  "</node>";

static void handleMethodCall(GDBusConnection       *connection,
							 const gchar           *sender,
							 const gchar           *object_path,
							 const gchar           *interface_name,
							 const gchar           *method_name,
							 GVariant              *parameters,
							 GDBusMethodInvocation *invocation,
							 gpointer               user_data)
{

	Bluetooth5* manager = static_cast<Bluetooth5*>(user_data);

	std::string method = method_name;

	if(method == "Release")
	{

	}
	else if(method == "NewConnection")
	{
		DebugOut()<<"NewConnection() called"<<endl;

		gchar* device;
		int fd;
		GVariantIter* iter;

		g_variant_get(parameters,"(oha{sv})", &device, &fd, &iter);

		manager->connected(fd);
	}
	else if(method == "RequestDisconnection")
	{
		DebugOut()<<"disconnection."<<endl;
	}
	else
	{
		g_dbus_method_invocation_return_error(invocation,G_DBUS_ERROR,G_DBUS_ERROR_UNKNOWN_METHOD, "Unknown method.");
	}

	/// return nothing:
	g_dbus_method_invocation_return_value(invocation, nullptr);
}

static GVariant* getProperty(GDBusConnection* connection, const gchar* sender, const gchar* objectPath, const gchar* interfaceName, const gchar* propertyName, GError** error, gpointer userData)
{
	return NULL;
}

static gboolean setProperty(GDBusConnection * connection, const gchar * sender, const gchar *objectPath,
							const gchar *interfaceName, const gchar * propertyName, GVariant *value,
							GError** error, gpointer userData)
{
	return false;
}

static const GDBusInterfaceVTable interfaceVTable =
{
	handleMethodCall,
	getProperty,
	setProperty
};


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

	g_variant_get(objectMap, "(a{oa{sa{sv}}})",&iter);

	while(g_variant_iter_next(iter, "(oa{sa{sv}})",&objPath, &level2Dict))
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

std::string findDevice(std::string addy, std::string adapterPath="")
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

	g_variant_get(objectMap, "(a{oa{sa{sv}}})",&iter);

	while(g_variant_iter_next(iter, "{oa{sa{sv}}}",&objPath, &level2Dict))
	{
		char * interfaceName;
		GVariantIter* innerDict;
		while(g_variant_iter_next(level2Dict, "{sa{sv}}", &interfaceName, &innerDict))
		{
			if(std::string(interfaceName) == "org.bluez.Device1")
			{
				char* propertyName;
				GVariant* value;

				while(objectPath == "" && g_variant_iter_next(innerDict,"{sv}", &propertyName, &value))
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

Bluetooth5::Bluetooth5()
{
	GError* error = NULL;

	GDBusNodeInfo* introspection = g_dbus_node_info_new_for_xml(introspection_xml, &error);

	if(error)
	{
		DebugOut(DebugOut::Error)<<"in instrospection xml: "<<error->message<<endl;
		g_error_free(error);
		throw -1;
	}

	GDBusInterfaceInfo* mInterfaceInfo = g_dbus_node_info_lookup_interface(introspection, "org.bluez.Profile1");

	GDBusConnection *mConnection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, nullptr, &error);

	if(error)
	{
		DebugOut(DebugOut::Error)<<"getting system bus: "<<error->message<<endl;
		g_error_free(error);
		throw -1;
	}

	int regId = g_dbus_connection_register_object(mConnection, "/org/bluez/spp", mInterfaceInfo, &interfaceVTable, this, NULL, &error);
	g_dbus_node_info_unref(introspection);

	if(error)
	{
		g_error_free(error);
		throw -1;
	}

	GVariantBuilder builder;
	g_variant_builder_init(&builder, G_VARIANT_TYPE_DICTIONARY);

	g_variant_builder_add(&builder, "{sv}", "Name", g_variant_new("s","AMB spp client"));
	g_variant_builder_add(&builder, "{sv}", "Role", g_variant_new("s","client"));
	g_variant_builder_add(&builder, "{sv}", "AutoConnect", g_variant_new("b",true));

	g_dbus_connection_call_sync(mConnection, "org.bluez", "/org/bluez", "org.bluez.ProfileManager1", "RegisterProfile", g_variant_new("(osa{sv})", "/org/bluez/spp", "00001101-0000-1000-8000-00805F9B34FB",
																																 &builder), nullptr, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, &error);

	if(error)
	{
		DebugOut(DebugOut::Error)<<"RegisterProfile failed: "<<error->message<<endl;
		throw -1;
	}
}

void Bluetooth5::getDeviceForAddress(std::string address, ConnectedCallback connectedCallback)
{
	mConnected = connectedCallback;

	std::string devicePath = findDevice(address);

	GDBusProxy * deviceProxy = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM,G_DBUS_PROXY_FLAGS_NONE,NULL,
															  "org.bluez", devicePath.c_str(), "org.bluez.Device1", nullptr, nullptr);

	GError* error = nullptr;

	g_dbus_proxy_call_sync(deviceProxy,"Connect", nullptr/*g_variant_new("(s)", "00001101-0000-1000-8000-00805F9B34FB")*/,G_DBUS_CALL_FLAGS_NONE,-1, nullptr, &error);

	if(error)
	{
		DebugOut(DebugOut::Error)<<"error trying to connect profile: "<<error->message<<endl;
		g_error_free(error);
		g_object_unref(deviceProxy);
		return;
	}



}

void Bluetooth5::connected(int fd)
{
	try
	{
		mConnected(fd);
	}
	catch(...)
	{

	}
}
