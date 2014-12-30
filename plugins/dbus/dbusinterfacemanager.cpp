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

#include <dbusexport.hpp>

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
	new DBusInterfaceManager(routingengine, config);
}

static void
on_bus_acquired (GDBusConnection *connection, const gchar *name, gpointer user_data)
{
	DBusInterfaceManager* iface = static_cast<DBusInterfaceManager*>(user_data);

	iface->connection = connection;

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

	if(!connection){
		DebugOut(DebugOut::Error)<<"DBus: Connection could not be established."<<endl;
		throw std::runtime_error("Could not establish DBus connection.");
	}
}



DBusInterfaceManager::DBusInterfaceManager(AbstractRoutingEngine* engine,std::map<std::string,std::string> config)
	:AbstractSink(engine,config),re(engine), connection(nullptr)
{
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
	amb::exportProperty<AccelerationProperty>(re, connection);
	amb::exportProperty<VehicleSpeedProperty>(re, connection);
	amb::exportProperty<TireProperty>(re, connection);
	amb::exportProperty<EngineSpeedProperty>(re, connection);
	amb::exportProperty<VehiclePowerModeProperty>(re, connection);
	amb::exportProperty<TripMeterProperty>(re, connection);
	amb::exportProperty<TransmissionProperty>(re, connection);
	amb::exportProperty<CruiseControlProperty>(re, connection);
	amb::exportProperty<BrakeOperation>(re, connection);
	amb::exportProperty<LightStatusProperty>(re, connection);
	amb::exportProperty<HornProperty>(re, connection);
	amb::exportProperty<FuelProperty>(re, connection);
	amb::exportProperty<EngineOilProperty>(re, connection);
	amb::exportProperty<ExteriorBrightnessProperty>(re, connection);
	amb::exportProperty<Temperature>(re, connection);
	amb::exportProperty<RainSensor>(re, connection);
	amb::exportProperty<ClimateControlProperty>(re, connection);
	amb::exportProperty<WindowStatusProperty>(re, connection);
	amb::exportProperty<DefrostProperty>(re, connection);
	amb::exportProperty<Sunroof>(re, connection);
	amb::exportProperty<ConvertibleRoof>(re, connection);
	amb::exportProperty<VehicleId>(re, connection);
	amb::exportProperty<VehicleTypeProperty>(re, connection);
	amb::exportProperty<FuelInfoProperty>(re, connection);
	amb::exportProperty<SizeProperty>(re, connection);
	amb::exportProperty<DoorsProperty>(re, connection);
	amb::exportProperty<WheelInformationProperty>(re, connection);
	amb::exportProperty<OdometerProperty>(re, connection);
	amb::exportProperty<FluidProperty>(re, connection);
	amb::exportProperty<BatteryStatusProperty>(re, connection);
	amb::exportProperty<ParkingBrakeProperty>(re, connection);
	amb::exportProperty<HazardLightProperty>(re, connection);
	amb::exportProperty<LocationProperty>(re, connection);
	amb::exportProperty<AntilockBrakingSystemProperty>(re, connection);
	amb::exportProperty<TractionControlSystemProperty>(re, connection);
	amb::exportProperty<VehicleTopSpeedLimitProperty>(re, connection);
	amb::exportProperty<AirbagStatusProperty>(re, connection);
	amb::exportProperty<DoorStatusProperty>(re, connection);
	amb::exportProperty<SeatBeltStatusProperty>(re, connection);
	amb::exportProperty<OccupantStatusProperty>(re, connection);
	amb::exportProperty<ObstacleDistanceProperty>(re, connection);
	amb::exportProperty<SteeringWheelPositionProperty>(re, connection);
	amb::exportProperty<SteeringWheel>(re, connection);
	amb::exportProperty<ThrottlePosition>(re, connection);
	amb::exportProperty<EngineCoolant>(re, connection);
	amb::exportProperty<NightMode>(re, connection);
	amb::exportProperty<DrivingMode>(re, connection);
	amb::exportProperty<PowertrainTorque>(re, connection);
	amb::exportProperty<AcceleratorPedalPosition>(re, connection);
	amb::exportProperty<Chime>(re, connection);
	amb::exportProperty<WheelTick>(re, connection);
	amb::exportProperty<IgnitionTime>(re, connection);
	amb::exportProperty<YawRate>(re, connection);
	amb::exportProperty<TransmissionClutch>(re, connection);
	amb::exportProperty<TransmissionOil>(re, connection);
	amb::exportProperty<BrakeMaintenance>(re, connection);
	amb::exportProperty<WasherFluid>(re, connection);
	amb::exportProperty<MalfunctionIndicator>(re, connection);
	amb::exportProperty<Diagnostics>(re, connection);
	amb::exportProperty<MirrorProperty>(re, connection);
	amb::exportProperty<SeatAdjustment>(re, connection);
	amb::exportProperty<DriveMode>(re, connection);
	amb::exportProperty<VehicleSound>(re, connection);
	amb::exportProperty<ElectronicStabilityControl>(re, connection);
	amb::exportProperty<ChildSafetyLock>(re, connection);
	amb::exportProperty<SeatProperty>(re, connection);
	amb::exportProperty<DoorProperty>(re, connection);
	amb::exportProperty<WindshieldWiperStatus>(re, connection);
	amb::exportProperty<SideWindowStatusProperty>(re, connection);
	amb::exportProperty<AtmosphericPressure>(re, connection);
	amb::exportProperty<LaneDepartureStatus>(re, connection);
	amb::exportProperty<AlarmStatus>(re, connection);

	PropertyList list = VehicleProperty::customProperties();
	PropertyList implemented = AbstractDBusInterface::implementedProperties();

	for (auto prop : list)
	{
		if(!contains(implemented, prop))
		{
			amb::exportProperty<CustomPropertyInterface>(prop, re, connection);
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
			amb::exportProperty<UncategorizedPropertyInterface>(prop, re, connection);
		}
	}
}
