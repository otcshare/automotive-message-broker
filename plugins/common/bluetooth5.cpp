#include "bluetooth5.h"
#include "superptr.hpp"

#include "debugout.h"
#include <gio/gio.h>
#include <gio/gunixfdlist.h>
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
		gint32 fd;
		GVariantIter* iter;

		DebugOut() << "parameters signature: " << g_variant_get_type_string(parameters) << endl;

		g_variant_get(parameters,"(oha{sv})", &device, &fd, &iter);

		DebugOut() << "device: " << device << endl;

		auto message = g_dbus_method_invocation_get_message(invocation);

		auto fdList = g_dbus_message_get_unix_fd_list(message);

		GError* error = nullptr;

		fd = g_unix_fd_list_get(fdList, 0, &error);

		auto errorPtr = amb::make_super(error);

		if(errorPtr)
		{
			DebugOut(DebugOut::Error) << "Error trying to get fd: " << errorPtr->message << endl;
			return;
		}

		char* propertyName;
		GVariant* value;

		DebugOut() << "trying to see what properties we got with this call" << endl;

		while(g_variant_iter_next(iter,"{sv}", &propertyName, &value))
		{
			auto keyPtr = amb::make_super(propertyName);
			auto valuePtr = amb::make_super(value);
			DebugOut() << "key " << keyPtr.get() << "value signature: " << g_variant_get_type_string(valuePtr.get()) << endl;
		}

		manager->connected_(fd);
	}
	else if(method == "RequestDisconnection")
	{
		DebugOut()<<"disconnection."<<endl;
	}
	else
	{
		g_dbus_method_invocation_return_error(invocation, G_DBUS_ERROR, G_DBUS_ERROR_UNKNOWN_METHOD, "Unknown method.");
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

std::string findDevice(std::string address, std::string adapterPath="")
{
	std::string objectPath;

	GError * proxyError = nullptr;
	auto managerProxy = amb::make_super(g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM, G_DBUS_PROXY_FLAGS_NONE, NULL,
															  "org.bluez",
															  "/",
															  "org.freedesktop.DBus.ObjectManager",
															  nullptr, &proxyError));

	auto proxyErrorPtr = amb::make_super(proxyError);
	if(proxyErrorPtr)
	{
		DebugOut(DebugOut::Error)<<"Could not create ObjectManager proxy for Bluez: "<<proxyErrorPtr->message<<endl;
		return "";
	}

	GError * getManagerObjectError = nullptr;

	auto objectMap = amb::make_super(g_dbus_proxy_call_sync(managerProxy.get(), "GetManagedObjects",nullptr, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &getManagerObjectError));

	auto getManagerObjectErrorPtr = amb::make_super(getManagerObjectError);

	if(getManagerObjectErrorPtr)
	{
		DebugOut(DebugOut::Error)<<"Failed call to GetManagedObjects: "<<getManagerObjectErrorPtr->message<<endl;
		return "";
	}

	GVariantIter* iter;
	char* objPath;
	GVariantIter* level2Dict;

	g_variant_get(objectMap.get(), "(a{oa{sa{sv}}})",&iter);

	auto iterPtr = amb::make_super(iter);


	while(g_variant_iter_next(iter, "{oa{sa{sv}}}",&objPath, &level2Dict))
	{
		auto level2DictPtr = amb::make_super(level2Dict);
		auto objPathPtr = amb::make_super(objPath);

		char * interfaceName;
		GVariantIter* innerDict;
		while(g_variant_iter_next(level2DictPtr.get(), "{sa{sv}}", &interfaceName, &innerDict))
		{
			auto interfaceNamePtr = amb::make_super(interfaceName);
			auto innerDictPtr = amb::make_super(innerDict);
			if(std::string(interfaceNamePtr.get()) == "org.bluez.Device1")
			{
				char* propertyName;
				GVariant* value;

				while(objectPath == "" && g_variant_iter_next(innerDictPtr.get(),"{sv}", &propertyName, &value))
				{
					auto propertyNamePtr = amb::make_super(propertyName);
					auto valuePtr = amb::make_super(value);

					if(std::string(propertyNamePtr.get()) == "Address")
					{
						char* addy;
						g_variant_get(valuePtr.get(),"s",&addy);

						auto addyPtr = amb::make_super(addy);

						if(addyPtr && std::string(addyPtr.get()) == address)
						{
							objectPath = objPathPtr.get();
						}
					}
					///TODO: filter only devices that have the specified adapter
				}
			}
		}
	}

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

	g_dbus_connection_call_sync(mConnection,
								"org.bluez",
								"/org/bluez",
								"org.bluez.ProfileManager1",
								"RegisterProfile",
								g_variant_new("(osa{sv})", "/org/bluez/spp", "00001101-0000-1000-8000-00805F9B34FB", &builder),
								nullptr, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, &error);

	if(error)
	{
		DebugOut(DebugOut::Error)<<"RegisterProfile failed: "<<error->message<<endl;
		throw -1;
	}
}

bool Bluetooth5::setDevice(string address)
{
	mPath = findDevice(address);


	if(mPath == "")
	{
		DebugOut(DebugOut::Error) << "device path not found.  Not paired? " << endl;
		return false;
	}

	return true;
}

void Bluetooth5::getDeviceForAddress(std::string address, ConnectedCallback connectedCallback)
{
	mConnected = connectedCallback;

	if(!setDevice(address))
		return;

	DebugOut() << "Bluetooth device path: " << mPath << endl;

	connect(connectedCallback);
}

void Bluetooth5::connected_(int fd)
{
	try
	{
		mConnected(fd);
	}
	catch(...)
	{
		DebugOut(DebugOut::Error) << "Error calling connected callback" << endl;
	}
}

void Bluetooth5::connect(ConnectedCallback onconnectedCallback)
{
	mConnected = onconnectedCallback;

	GError* error = nullptr;

	auto  deviceProxyPtr = amb::make_super(g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM,G_DBUS_PROXY_FLAGS_NONE,NULL,
															  "org.bluez", mPath.c_str(), "org.bluez.Device1", nullptr, &error));

	auto errorPtr = amb::make_super(error);

	if(errorPtr)
	{
		DebugOut(DebugOut::Error) << "Error getting bluetooth device proxy " << errorPtr->message <<endl;
		return;
	}

	g_dbus_proxy_call(deviceProxyPtr.get(), "Connect", nullptr, G_DBUS_CALL_FLAGS_NONE, -1, nullptr,
					  [](GObject *source_object, GAsyncResult *res, gpointer user_data)
	{

		GError* error = nullptr;

		g_dbus_proxy_call_finish(G_DBUS_PROXY (source_object), res, &error);

		auto errorPtr = amb::make_super(error);

		if(errorPtr)
		{
			DebugOut(DebugOut::Error) << "error trying to connect profile: " << errorPtr->message << endl;
		}
	},
	this);
}

void Bluetooth5::disconnect()
{
	GError* error = nullptr;

	auto  deviceProxyPtr = amb::make_super(g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM,G_DBUS_PROXY_FLAGS_NONE,NULL,
															  "org.bluez", mPath.c_str(), "org.bluez.Device1", nullptr, &error));

	auto errorPtr = amb::make_super(error);

	if(errorPtr)
	{
		DebugOut(DebugOut::Error) << "Error getting bluetooth device proxy " << errorPtr->message <<endl;
		return;
	}

	g_dbus_proxy_call(deviceProxyPtr.get(), "Disconnect", nullptr, G_DBUS_CALL_FLAGS_NONE, -1, nullptr,[](GObject *source_object,
					  GAsyncResult *res, gpointer user_data){
		GError* error = nullptr;

		g_dbus_proxy_call_finish(G_DBUS_PROXY (source_object), res, &error);

		auto errorPtr = amb::make_super(error);

		if(errorPtr)
		{
			DebugOut(DebugOut::Error) << "error trying to disconnect: " << errorPtr->message << endl;
		}
	}, nullptr);
}
