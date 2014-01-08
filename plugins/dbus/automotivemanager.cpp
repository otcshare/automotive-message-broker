#include "automotivemanager.h"
#include "abstractdbusinterface.h"
#include "listplusplus.h"

static const gchar introspection_xml[] =
  "<node>"
  "  <interface name='org.automotive.Manager'>"
  "    <method name='FindObject'>"
  "      <arg type='s' name='searchstring' direction='in'/>"
  "      <arg type='ao' name='response' direction='out'/>"
  "    </method>"
  "    <method name='FindObjectForZone'>"
  "      <arg type='s' name='searchstring' direction='in'/>"
  "      <arg type='i' name='zone' direction='in'/>"
  "      <arg type='o' name='response' direction='out'/>"
  "    </method>"
  "    <method name='FindObjectForSourceZone'>"
  "      <arg type='s' name='searchstring' direction='in'/>"
  "      <arg type='s' name='source' direction='in'/>"
  "      <arg type='i' name='zone' direction='in'/>"
  "      <arg type='o' name='response' direction='out'/>"
  "    </method>"
  "    <method name='List'>"
  "      <arg type='as' name='response' direction='out'/>"
  "    </method>"
  "    <method name='ZonesForObjectName'>"
  "      <arg type='s' name='searchstring' direction='in'/>"
  "      <arg type='ai' name='response' direction='out'/>"
  "    </method>"
  "    <method name='SourcesForObjectName'>"
  "      <arg type='s' name='searchstring' direction='in'/>"
  "      <arg type='as' name='response' direction='out'/>"
  "    </method>"
  "    <method name='findProperty'>"
  "      <arg type='s' name='searchstring' direction='in'/>"
  "      <arg type='o' name='response' direction='out'/>"
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

	AutomotiveManager* manager = static_cast<AutomotiveManager*>(user_data);

	std::string method = method_name;

	uint pid = getPid(sender);

	if(DebugOut::getDebugThreshhold() >= 6)
	{
		DebugOut(6)<<"DBus method call from: "<<sender<< " pid: " <<pid<< " interface: "<<interface_name<<" method: "<<method<<endl;
		DebugOut(6)<<"DBus method call path: "<<object_path<<endl;
	}

	if(method == "findProperty")
	{
		DebugOut(DebugOut::Warning)<<"org.automotive.Manager.findProperty() is deprecated.  Use org.automotive.Manager.FindObject() instead."<<endl;

		gchar* arg;

		g_variant_get(parameters,"(s)",&arg);

		std::string objectToFind = arg;

		if(objectToFind == "")
		{
			g_dbus_method_invocation_return_error(invocation,G_DBUS_ERROR,G_DBUS_ERROR_INVALID_ARGS, "Invalid argument.");
			return;
		}

		std::list<AbstractDBusInterface*> interfaces = AbstractDBusInterface::getObjectsForProperty(objectToFind);

		if(!interfaces.size())
		{
			g_dbus_method_invocation_return_dbus_error(invocation,"org.automotive.Manager.PropertyNotFound", "Object not found");
			return;
		}

		auto itr = interfaces.begin();

		g_dbus_method_invocation_return_value(invocation, g_variant_new("(o)",(*itr)->objectPath().c_str()));
		///TODO: we might need to clean up stuff there (like var)
	}

	else if(method == "FindObject")
	{
		gchar* arg;

		g_variant_get(parameters,"(s)",&arg);

		std::string objectToFind = arg;

		if(objectToFind == "")
		{
			g_dbus_method_invocation_return_error(invocation,G_DBUS_ERROR,G_DBUS_ERROR_INVALID_ARGS, "Invalid argument.");
			return;
		}

		std::list<AbstractDBusInterface*> interfaces = AbstractDBusInterface::getObjectsForProperty(objectToFind);

		if(!interfaces.size())
		{
			g_dbus_method_invocation_return_dbus_error(invocation,"org.automotive.Manager.ObjectNotFound", "Object not found");
			return;
		}

		GVariantBuilder params;
		g_variant_builder_init(&params, G_VARIANT_TYPE_ARRAY);

		for(auto itr = interfaces.begin(); itr != interfaces.end(); itr++)
		{
			AbstractDBusInterface* t = *itr;
			if(!t->isSupported())
				continue;

			if(!t->isRegistered())
				t->registerObject();

			std::list<std::string> processes = manager->subscribedProcesses[t];

			if(!contains(processes,sender))
			{
				DebugOut()<<"Referencing "<<t->objectPath()<<" with sender: "<<sender<<endl;
				manager->subscribedProcesses[t].push_back(sender);
			}

			GVariant *newvar = g_variant_new("o",t->objectPath().c_str());
			g_variant_builder_add_value(&params, newvar);
		}

		//GVariant* var =  g_variant_builder_end(&params);

		g_dbus_method_invocation_return_value(invocation, g_variant_new("(ao)",&params));
		///TODO: we might need to clean up stuff there (like var)
	}

	else if(method == "FindObjectForZone")
	{
		gchar* arg;
		int zone;

		g_variant_get(parameters,"(si)", &arg, &zone);

		std::string propertyToFind = arg;

		if(propertyToFind == "")
		{
			g_dbus_method_invocation_return_error(invocation,G_DBUS_ERROR,G_DBUS_ERROR_INVALID_ARGS, "Invalid argument.");
			return;
		}

		std::list<AbstractDBusInterface*> interfaces = AbstractDBusInterface::getObjectsForProperty(propertyToFind);

		if(!interfaces.size())
		{
			g_dbus_method_invocation_return_dbus_error(invocation,"org.automotive.Manager.ObjectNotFound", "Property not found");
			return;
		}



		for(auto itr = interfaces.begin(); itr != interfaces.end(); itr++)
		{
			AbstractDBusInterface* t = *itr;
			if(t->zone() == (Zone::Type)zone)
			{
				g_dbus_method_invocation_return_value(invocation,g_variant_new("(o)", t->objectPath().c_str()));
				return;
			}
		}

		g_dbus_method_invocation_return_dbus_error(invocation,"org.automotive.Manager.ObjectNotFound", "Property not found");
	}

	else if (method == "ZonesForObjectName")
	{
		gchar* arg;

		g_variant_get(parameters,"(s)",&arg);

		std::string propertyToFind = arg;

		if(propertyToFind == "")
		{
			g_dbus_method_invocation_return_error(invocation, G_DBUS_ERROR, G_DBUS_ERROR_INVALID_ARGS, "Invalid argument.");
			return;
		}

		std::list<AbstractDBusInterface*> interfaces = AbstractDBusInterface::getObjectsForProperty(propertyToFind);

		if(!interfaces.size())
		{
			g_dbus_method_invocation_return_dbus_error(invocation,"org.automotive.Manager.ObjectNotFound", "Property not found");
			return;
		}

		GVariantBuilder params;
		g_variant_builder_init(&params, G_VARIANT_TYPE_ARRAY);

		for(auto itr = interfaces.begin(); itr != interfaces.end(); itr++)
		{
			AbstractDBusInterface* t = *itr;
			GVariant *newvar = g_variant_new("i",t->zone());
			g_variant_builder_add_value(&params, newvar);

		}

		g_dbus_method_invocation_return_value(invocation,g_variant_new("(ai)",&params));
	}
	
	else if(method == "List")
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

			std::string objectName = (*itr)->objectName();

			g_variant_builder_add(&builder, "s", objectName.c_str());
		}


		g_dbus_method_invocation_return_value(invocation,g_variant_new("(as)",&builder));
	}
	else if(method == "SourcesForObjectName")
	{
		gchar* arg;

		g_variant_get(parameters,"(s)",&arg);

		std::string propertyToFind = arg;

		if(propertyToFind == "")
		{
			g_dbus_method_invocation_return_error(invocation, G_DBUS_ERROR, G_DBUS_ERROR_INVALID_ARGS, "Invalid argument.");
			return;
		}

		std::list<AbstractDBusInterface*> interfaces = AbstractDBusInterface::getObjectsForProperty(propertyToFind);

		if(!interfaces.size())
		{
			g_dbus_method_invocation_return_dbus_error(invocation,"org.automotive.Manager.ObjectNotFound", "Property not found");
			return;
		}

		GVariantBuilder params;
		g_variant_builder_init(&params, G_VARIANT_TYPE_ARRAY);

		for(auto itr = interfaces.begin(); itr != interfaces.end(); itr++)
		{
			AbstractDBusInterface* t = *itr;
			string source = t->source();
			boost::algorithm::erase_all(source, "-");
			GVariant *newvar = g_variant_new("s", source.c_str());
			g_variant_builder_add_value(&params, newvar);

		}

		g_dbus_method_invocation_return_value(invocation,g_variant_new("(as)",&params));
	}
	else if(method == "FindObjectForSourceZone")
	{
		gchar* arg;
		gchar* arg2;
		int zone;

		g_variant_get(parameters,"(ssi)", &arg, &arg2, &zone);

		std::string propertyToFind = arg;
		std::string source = arg2;

		if(propertyToFind == "" || source == "")
		{
			g_dbus_method_invocation_return_error(invocation,G_DBUS_ERROR,G_DBUS_ERROR_INVALID_ARGS, "Invalid argument.");
			return;
		}

		std::list<AbstractDBusInterface*> interfaces = AbstractDBusInterface::getObjectsForProperty(propertyToFind);

		if(!interfaces.size())
		{
			g_dbus_method_invocation_return_dbus_error(invocation,"org.automotive.Manager.ObjectNotFound", "Property not found");
			return;
		}

		for(auto itr = interfaces.begin(); itr != interfaces.end(); itr++)
		{
			AbstractDBusInterface* t = *itr;
			string targetSource = t->source();
			boost::algorithm::erase_all(targetSource, "-");
			if(t->zone() == (Zone::Type)zone && source == targetSource)
			{
				g_dbus_method_invocation_return_value(invocation,g_variant_new("(o)", t->objectPath().c_str()));
				return;
			}
		}

		g_dbus_method_invocation_return_dbus_error(invocation,"org.automotive.Manager.ObjectNotFound", "Property not found");
	}
	else
	{
		g_dbus_method_invocation_return_error(invocation,G_DBUS_ERROR,G_DBUS_ERROR_UNKNOWN_METHOD, "Unknown method.");
	}
}

static void signalCallback( GDBusConnection *connection,
																												 const gchar *sender_name,
																												 const gchar *object_path,
																												 const gchar *interface_name,
																												 const gchar *signal_name,
																												 GVariant *parameters,
																												 gpointer user_data)
{
	AutomotiveManager* manager = static_cast<AutomotiveManager*>(user_data);

	gchar* name=nullptr;
	gchar* newOwner=nullptr;
	gchar* oldOwner = nullptr;
	g_variant_get(parameters,"(sss)",&name, &oldOwner, &newOwner);

	if(std::string(newOwner) == "")
	{
		for(auto i : manager->subscribedProcesses)
		{
			AbstractDBusInterface* iface = i.first;
			for(auto n : i.second)
			{
				if(n == name)
				{
					DebugOut()<<"unreferencing "<<n<<" from the subscription of "<<iface->objectPath()<<endl;
					manager->subscribedProcesses[iface].remove(n);
				}
			}

			if(manager->subscribedProcesses[iface].empty())
			{
				DebugOut()<<"No more subscribers.  Unregistering."<<endl;
				iface->unregisterObject();
			}
		}
	}

	g_free(name);
	g_free(newOwner);
	g_free(oldOwner);
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

	regId = g_dbus_connection_register_object(mConnection, "/", mInterfaceInfo, &interfaceVTable, this, NULL, &error);
	g_dbus_node_info_unref(introspection);

	if(error){
		g_error_free(error);
		throw -1;
	}

	g_assert(regId > 0);

	g_dbus_connection_signal_subscribe(g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL,NULL), "org.freedesktop.DBus", "org.freedesktop.DBus",
																					   "NameOwnerChanged", "/org/freedesktop/DBus", NULL, G_DBUS_SIGNAL_FLAGS_NONE,
																					   signalCallback, this, NULL);
}

AutomotiveManager::~AutomotiveManager()
{
	g_dbus_connection_unregister_object(mConnection, regId);
}
