/*
Copyright (C) 2012 Intel Corporation

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "dbusinterfacemanager.h"
#include "runningstatusinterface.h"

#include <string>

using namespace std;

static void
on_bus_acquired (GDBusConnection *connection, const gchar *name, gpointer user_data)
{
	RunningStatusInterface::iface()->registerObject(connection);
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


