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

#include "listplusplus.h"
#include "automotivemanager.h"
#include <unordered_set>

#include <dbusexport.h>
#include <ambplugin.h>

///properties:
#include "runningstatus.h"
#include "custompropertyinterface.h"
#include "uncategorizedproperty.h"
#include "environmentproperties.h"
#include "vehicleinfo.h"
#include "maintenance.h"
#include "parking.h"
#include "drivingsafety.h"
#include "personalization.h"

extern "C" void create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	auto plugin = new AmbPlugin<DBusInterfaceManager>(routingengine, config);
	plugin->init();
}

static void
on_bus_acquired (GDBusConnection *connection, const gchar *name, gpointer user_data)
{
	DBusInterfaceManager* iface = static_cast<DBusInterfaceManager*>(user_data);

	iface->connection = std::shared_ptr<GDBusConnection>(connection, [=](auto conn){
		amb::traits<GDBusConnection>::delete_functor functor;
		functor(conn);
	});

	iface->setValue(iface->dbusConnected, true);

	new AutomotiveManager(connection);

	iface->registerTypes();
}

static void
on_name_acquired (GDBusConnection *connection, const gchar *name, gpointer user_data)
{

}

static void
on_name_lost (GDBusConnection *connection, const gchar *name, gpointer user_data)
{
	DebugOut(DebugOut::Error)<<"DBus: Lost bus name"<<endl;

	DBusInterfaceManager* iface = static_cast<DBusInterfaceManager*>(user_data);

	iface->setValue(iface->dbusConnected, false);

	if(!connection){
		DebugOut(DebugOut::Error)<<"DBus: Connection could not be established."<<endl;
		throw std::runtime_error("Could not establish DBus connection.");
	}
}



DBusInterfaceManager::DBusInterfaceManager(AbstractRoutingEngine * engine, std::map<std::string,std::string> config, AbstractSource & parent)
	:AmbPluginImpl(engine, config, parent), connection(nullptr)
{
	dbusConnected = addPropertySupport(Zone::None, []() { return new BasicPropertyType<bool>(DBusConnected, false); });

	ownerId = g_bus_own_name(G_BUS_TYPE_SYSTEM,
							 DBusServiceName,
							 G_BUS_NAME_OWNER_FLAGS_NONE,
							 on_bus_acquired,
							 on_name_acquired,
							 on_name_lost,
							 this,
							 nullptr);

}

DBusInterfaceManager::~DBusInterfaceManager()
{
	g_bus_unown_name(ownerId);
}

void DBusInterfaceManager::supportedChanged(const PropertyList &supportedProperties)
{
	DebugOut()<<"supported Properties: "<<supportedProperties.size()<<endl;
	if(!connection)
	{
		return;
	}

	registerTypes();
}

void DBusInterfaceManager::registerTypes()
{
	/// properties:
	auto re = routingEngine;

	auto exporter = amb::Exporter::instance();

	exporter->connection = connection;

	exporter->exportProperty<AccelerationProperty>(re);
	exporter->exportProperty<VehicleSpeedProperty>(re);
	exporter->exportProperty<TireProperty>(re);
	exporter->exportProperty<EngineSpeedProperty>(re);
	exporter->exportProperty<VehiclePowerModeProperty>(re);
	exporter->exportProperty<TripMeterProperty>(re);
	exporter->exportProperty<TransmissionProperty>(re);
	exporter->exportProperty<CruiseControlProperty>(re);
	exporter->exportProperty<BrakeOperation>(re);
	exporter->exportProperty<LightStatusProperty>(re);
	exporter->exportProperty<HornProperty>(re);
	exporter->exportProperty<FuelProperty>(re);
	exporter->exportProperty<EngineOilProperty>(re);
	exporter->exportProperty<ExteriorBrightnessProperty>(re);
	exporter->exportProperty<Temperature>(re);
	exporter->exportProperty<RainSensor>(re);
	exporter->exportProperty<ClimateControlProperty>(re);
	exporter->exportProperty<WindowStatusProperty>(re);
	exporter->exportProperty<DefrostProperty>(re);
	exporter->exportProperty<Sunroof>(re);
	exporter->exportProperty<ConvertibleRoof>(re);
	exporter->exportProperty<VehicleId>(re);
	exporter->exportProperty<VehicleTypeProperty>(re);
	exporter->exportProperty<FuelInfoProperty>(re);
	exporter->exportProperty<SizeProperty>(re);
	exporter->exportProperty<DoorsProperty>(re);
	exporter->exportProperty<WheelInformationProperty>(re);
	exporter->exportProperty<OdometerProperty>(re);
	exporter->exportProperty<FluidProperty>(re);
	exporter->exportProperty<BatteryStatusProperty>(re);
	exporter->exportProperty<ParkingBrakeProperty>(re);
	exporter->exportProperty<HazardLightProperty>(re);
	exporter->exportProperty<LocationProperty>(re);
	exporter->exportProperty<AntilockBrakingSystemProperty>(re);
	exporter->exportProperty<TractionControlSystemProperty>(re);
	exporter->exportProperty<VehicleTopSpeedLimitProperty>(re);
	exporter->exportProperty<AirbagStatusProperty>(re);
	exporter->exportProperty<DoorStatusProperty>(re);
	exporter->exportProperty<SeatBeltStatusProperty>(re);
	exporter->exportProperty<OccupantStatusProperty>(re);
	exporter->exportProperty<ObstacleDistanceProperty>(re);
	exporter->exportProperty<SteeringWheelPositionProperty>(re);
	exporter->exportProperty<SteeringWheel>(re);
	exporter->exportProperty<ThrottlePosition>(re);
	exporter->exportProperty<EngineCoolant>(re);
	exporter->exportProperty<NightMode>(re);
	exporter->exportProperty<DrivingMode>(re);
	exporter->exportProperty<PowertrainTorque>(re);
	exporter->exportProperty<AcceleratorPedalPosition>(re);
	exporter->exportProperty<Chime>(re);
	exporter->exportProperty<WheelTick>(re);
	exporter->exportProperty<IgnitionTime>(re);
	exporter->exportProperty<YawRate>(re);
	exporter->exportProperty<TransmissionClutch>(re);
	exporter->exportProperty<TransmissionOil>(re);
	exporter->exportProperty<BrakeMaintenance>(re);
	exporter->exportProperty<WasherFluid>(re);
	exporter->exportProperty<MalfunctionIndicator>(re);
	exporter->exportProperty<Diagnostics>(re);
	exporter->exportProperty<MirrorProperty>(re);
	exporter->exportProperty<SeatAdjustment>(re);
	exporter->exportProperty<DriveMode>(re);
	exporter->exportProperty<VehicleSound>(re);
	exporter->exportProperty<ElectronicStabilityControl>(re);
	exporter->exportProperty<ChildSafetyLock>(re);
	exporter->exportProperty<SeatProperty>(re);
	exporter->exportProperty<DoorProperty>(re);
	exporter->exportProperty<WindshieldWiperStatus>(re);
	exporter->exportProperty<SideWindowStatusProperty>(re);
	exporter->exportProperty<AtmosphericPressure>(re);
	exporter->exportProperty<LaneDepartureStatus>(re);
	exporter->exportProperty<AlarmStatus>(re);

	PropertyList list = VehicleProperty::customProperties();
	PropertyList implemented = AbstractDBusInterface::implementedProperties();

	for (auto prop : list)
	{
		if(!contains(implemented, prop))
		{
			exporter->exportProperty<CustomPropertyInterface>(prop, re);
		}
	}

	/// Create objects for unimplemented properties:

	implemented = AbstractDBusInterface::implementedProperties();

	PropertyList capabilitiesList = VehicleProperty::capabilities();

	for (auto itr = capabilitiesList.begin(); itr != capabilitiesList.end(); itr++)
	{
		VehicleProperty::Property prop = *itr;

		if(!contains(implemented, prop))
		{
			exporter->exportProperty<UncategorizedPropertyInterface>(prop, re);
		}
	}
}
