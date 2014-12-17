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
  "    <method name='SupportsProperty'>"
  "      <arg type='s' name='objectName' direction='in'/>"
  "      <arg type='s' name='propertyName' direction='in'/>"
  "      <arg type='b' name='response' direction='out'/>"
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

	if(method == "FindObject")
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

		bool hasItems = false;

		for(auto itr = interfaces.begin(); itr != interfaces.end(); itr++)
		{
			AbstractDBusInterface* t = *itr;
			if(!t->isSupported())
				continue;

			hasItems = true;

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

		if(hasItems)
			g_dbus_method_invocation_return_value(invocation, g_variant_new("(ao)",&params));
		else
			g_dbus_method_invocation_return_dbus_error(invocation,"org.automotive.Manager.ObjectNotFound", "Property not found");

	}

	else if(method == "FindObjectForZone")
	{
		gchar* arg;
		int zone;

		g_variant_get(parameters,"(si)", &arg, &zone);

		std::string propertyToFind = arg;

		if(propertyToFind == "")
		{
			g_dbus_method_invocation_return_error(invocation,G_DBUS_ERROR, G_DBUS_ERROR_INVALID_ARGS, "Invalid argument.");
			return;
		}

		std::list<AbstractDBusInterface*> interfaces = AbstractDBusInterface::getObjectsForProperty(propertyToFind);

		if(!interfaces.size())
		{
			g_dbus_method_invocation_return_dbus_error(invocation, "org.automotive.Manager.ObjectNotFound", "Property not found");
			return;
		}



		for(auto itr = interfaces.begin(); itr != interfaces.end(); itr++)
		{
			AbstractDBusInterface* t = *itr;

			if(t->zone() == (Zone::Type)zone)
			{
				if(!t->isRegistered())
					t->registerObject();

				std::list<std::string> processes = manager->subscribedProcesses[t];

				if(!contains(processes,sender))
				{
					DebugOut()<<"Referencing "<<t->objectPath()<<" with sender: "<<sender<<endl;
					manager->subscribedProcesses[t].push_back(sender);
				}

				g_dbus_method_invocation_return_value(invocation,g_variant_new("(o)", t->objectPath().c_str()));
				return;
			}
		}

		g_dbus_method_invocation_return_dbus_error(invocation,"org.automotive.Manager.InvalidZone", "zone not found");
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
		std::vector<std::string> supportedProperties = AbstractDBusInterface::supportedInterfaces();

		if(!supportedProperties.size())
		{
			g_dbus_method_invocation_return_dbus_error(invocation,"org.automotive.Manager.Error", "No supported objects");
			return;
		}

		std::sort(supportedProperties.begin(), supportedProperties.end());
		auto itr = std::unique(supportedProperties.begin(), supportedProperties.end());

		supportedProperties.erase(itr, supportedProperties.end());

		GVariantBuilder builder;
		g_variant_builder_init(&builder, G_VARIANT_TYPE_ARRAY);

		for(auto objectName  : supportedProperties)
		{
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
				if(!t->isRegistered())
					t->registerObject();

				std::list<std::string> processes = manager->subscribedProcesses[t];

				if(!contains(processes,sender))
				{
					DebugOut()<<"Referencing "<<t->objectPath()<<" with sender: "<<sender<<endl;
					manager->subscribedProcesses[t].push_back(sender);
				}

				g_dbus_method_invocation_return_value(invocation,g_variant_new("(o)", t->objectPath().c_str()));
				return;
			}
		}

		g_dbus_method_invocation_return_dbus_error(invocation,"org.automotive.Manager.ObjectNotFound", "Property not found");
	}
	else if(method == "SupportsProperty")
	{
		gchar* objectName;
		gchar* propertyToFindStr;

		g_variant_get(parameters,"(ss)",&objectName, &propertyToFindStr);

		auto objectNamePtr = amb::make_super(objectName);
		auto propertyToFindStrPtr = amb::make_super(propertyToFindStr);

		DebugOut(6) << "Checking " << objectNamePtr.get() << " for member: " << propertyToFindStrPtr.get() << endl;

		std::list<AbstractDBusInterface*> interfaces = AbstractDBusInterface::getObjectsForProperty(objectNamePtr.get());

		for(auto i : interfaces)
		{
			if(i->hasPropertyDBus(propertyToFindStrPtr.get()))
			{
				DebugOut(6) << "member " << propertyToFindStrPtr.get() << " of " << objectNamePtr.get() << " was found!!" << endl;
				g_dbus_method_invocation_return_value(invocation, g_variant_new("(b)", true));
				return;
			}
		}
		DebugOut(6) << "member " << propertyToFindStrPtr.get() << " of " << objectNamePtr.get() << " was not found." << endl;
		g_dbus_method_invocation_return_value(invocation,g_variant_new("(b)", false));
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

	if(!manager)
	{
		DebugOut(DebugOut::Error)<<"Bad user_data"<<endl;
		return;
	}

	gchar* name=nullptr;
	gchar* newOwner=nullptr;
	gchar* oldOwner = nullptr;
	g_variant_get(parameters,"(sss)",&name, &oldOwner, &newOwner);

	std::list<AbstractDBusInterface*> toRemove;

	if(std::string(newOwner) == "")
	{
		for(auto &i : manager->subscribedProcesses)
		{
			AbstractDBusInterface* iface = i.first;

			for(auto itr = i.second.begin(); itr != i.second.end(); itr++)
			{
				std::string n = *itr;
				if(n == name)
				{
					DebugOut()<<"unreferencing "<<n<<" from the subscription of "<<iface->objectPath()<<endl;
					itr = manager->subscribedProcesses[iface].erase(itr);
					DebugOut()<<"new ref count: "<<manager->subscribedProcesses[iface].size()<<endl;
				}
			}

			if(manager->subscribedProcesses[iface].empty())
			{
				DebugOut()<<"No more subscribers.  Unregistering: "<<iface->objectPath()<<endl;
				///Defer removal to not screw up the list
				toRemove.push_back(iface);
				iface->unregisterObject();
			}
		}

		for(auto & i : toRemove)
		{
			manager->subscribedProcesses.erase(i);
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
