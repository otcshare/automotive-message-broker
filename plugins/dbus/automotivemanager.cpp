#include "automotivemanager.h"
#include "abstractdbusinterface.h"

static const gchar introspection_xml[] =
  "<node>"
  "  <interface name='org.automotive.Manager'>"
  "    <method name='findProperty'>"
  "      <arg type='s' name='searchstring' direction='in'/>"
  "      <arg type='o' name='response' direction='out'/>"
  "    </method>"
  "    <method name='list'>"
  "      <arg type='as' name='response' direction='out'/>"
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

	std::string method = method_name;

	if(method == "findProperty")
	{
		gchar* arg;

		g_variant_get(parameters,"(s)",&arg);

		std::string objectToFind = arg;

		if(objectToFind == "")
		{
			g_dbus_method_invocation_return_error(invocation,G_DBUS_ERROR,G_DBUS_ERROR_INVALID_ARGS, "Invalid argument.");
			return;
		}

		AbstractDBusInterface* interface = AbstractDBusInterface::getInterfaceForObject(objectToFind);

		if(!interface)
		{
			g_dbus_method_invocation_return_dbus_error(invocation,"org.automotive.Manager.ObjectNotFound", "Object not found");
			return;
		}

		if(!interface->isSupported())
		{
			g_dbus_method_invocation_return_dbus_error(invocation,"org.automotive.Manager.ObjectNotSupported", "Object not Supported");
			return;
		}

		g_dbus_method_invocation_return_value(invocation,g_variant_new("(o)",interface->objectPath().c_str()));
	}

	else if(method == "list")
	{
		std::list<AbstractDBusInterface*> list = AbstractDBusInterface::interfaces();

		if(!list.size())
		{
			g_dbus_method_invocation_return_dbus_error(invocation,"org.automotive.Manager.Error", "No supported objects");
			return;
		}

		GVariantBuilder builder;
		g_variant_builder_init(&builder, G_VARIANT_TYPE_ARRAY);


		for(auto itr = list.begin(); itr != list.end(); itr++)
		{
			if(!(*itr)->isSupported())
				continue;

			std::string objectName = (*itr)->interfaceName();

			g_assert(objectName.size() > strlen("org.automotive."));

			objectName = objectName.substr(strlen("org.automotive."));

			g_variant_builder_add(&builder, "s", objectName.c_str());
		}


		g_dbus_method_invocation_return_value(invocation,g_variant_new("(as)",&builder));
	}

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

AutomotiveManager::AutomotiveManager(GDBusConnection *connection)
	:mConnection(connection)
{
	GError* error = NULL;

	GDBusNodeInfo* introspection = g_dbus_node_info_new_for_xml(introspection_xml, &error);
	GDBusInterfaceInfo* mInterfaceInfo = g_dbus_node_info_lookup_interface(introspection, "org.automotive.Manager");

	regId = g_dbus_connection_register_object(mConnection, "/", mInterfaceInfo, &interfaceVTable, NULL, NULL, &error);

	if(error) throw -1;

	g_assert(regId > 0);
}

AutomotiveManager::~AutomotiveManager()
{
	g_dbus_connection_unregister_object(mConnection, regId);
}
