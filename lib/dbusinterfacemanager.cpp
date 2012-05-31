/*
    Copyright 2012 Kevron Rees <email>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/


#include "dbusinterfacemanager.h"
#include "runningstatusinterface.h"

#include <string>

using namespace std;

static void
on_bus_acquired (GDBusConnection *connection, const gchar *name, gpointer user_data)
{
	RunningStatusInterface::iface->registerObject(connection);
}

static void
on_name_acquired (GDBusConnection *connection, const gchar *name, gpointer user_data)
{
}

static void
on_name_lost (GDBusConnection *connection, const gchar *name, gpointer user_data)
{
}



DBusInterfaceManager::DBusInterfaceManager()
{
	g_type_init();

	ownerId = g_bus_own_name(G_BUS_TYPE_SESSION,
					DBusServiceName,
					G_BUS_NAME_OWNER_FLAGS_NONE,
					on_bus_acquired,
					on_name_acquired,
					on_name_lost,
					this,
					NULL);

	///TODO: instantiate other interfaces here!
}

DBusInterfaceManager::~DBusInterfaceManager()
{
	g_bus_unown_name(ownerId);
}


