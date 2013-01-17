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


#include <gio/gio.h>
#include <string>

///properties:
#include "accelerationproperty.h"
#include "properties.h"
#include "custompropertyinterface.h"
#include "environmentproperties.h"

#define ConstructProperty(property) \
	new property(iface->re, connection);

using namespace std;

static void
on_bus_acquired (GDBusConnection *connection, const gchar *name, gpointer user_data)
{
	DBusInterfaceManager* iface = static_cast<DBusInterfaceManager*>(user_data);

	AbstractDBusInterface* acceleration = new AccelerationProperty(iface->re, connection);
	AbstractDBusInterface* vehicleSpeed = new VehicleSpeedProperty(iface->re, connection);
	AbstractDBusInterface* tirePressure = new TirePressureProperty(iface->re, connection);
	AbstractDBusInterface* engineSpeed = new EngineSpeedProperty(iface->re, connection);
	ConstructProperty(VehiclePowerModeProperty);
	ConstructProperty(TripMeterProperty);
	ConstructProperty(TransmissionProperty);
	ConstructProperty(TireTemperatureProperty);
	ConstructProperty(CruiseControlProperty);
	ConstructProperty(WheelBrakeProperty);
	ConstructProperty(LightStatusProperty);
	ConstructProperty(HornProperty);
	ConstructProperty(FuelProperty);
	ConstructProperty(EngineOilProperty);
	ConstructProperty(ExteriorBrightnessProperty);

	PropertyList list = VehicleProperty::customProperties();

	for (auto itr = list.begin(); itr != list.end(); itr++)
	{
		VehicleProperty::Property prop = *itr;

		new CustomPropertyInterface(prop,iface->re,connection);
	}


}

static void
on_name_acquired (GDBusConnection *connection, const gchar *name, gpointer user_data)
{
}

static void
on_name_lost (GDBusConnection *connection, const gchar *name, gpointer user_data)
{
}



DBusInterfaceManager::DBusInterfaceManager(AbstractRoutingEngine* engine)
	:re(engine)
{
	g_type_init();

	ownerId = g_bus_own_name(G_BUS_TYPE_SYSTEM,
					DBusServiceName,
					G_BUS_NAME_OWNER_FLAGS_NONE,
					on_bus_acquired,
					on_name_acquired,
					on_name_lost,
					this,
					NULL);

}

DBusInterfaceManager::~DBusInterfaceManager()
{
	g_bus_unown_name(ownerId);
}


