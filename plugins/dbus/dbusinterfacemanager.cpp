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


std::unordered_map<std::string, std::unordered_set<Zone::Type>> getUniqueSourcesList(AbstractRoutingEngine *re, PropertyList implementedProperties)
{
	std::unordered_map<std::string, std::unordered_set<Zone::Type>> uniqueSourcesList;

	for(auto property : implementedProperties)
	{
		std::vector<std::string> sources = re->sourcesForProperty(property);

		for(auto source : sources)
		{
			PropertyInfo info = re->getPropertyInfo(property, source);

			std::unordered_set<Zone::Type> uniqueZoneList;

			if(uniqueSourcesList.count(source))
			{
				uniqueZoneList = uniqueSourcesList[source];
			}

			Zone::ZoneList zoneList = info.zones();

			if(!zoneList.size())
			{
				uniqueZoneList.emplace(Zone::None);
			}

			for(auto zoneItr : zoneList)
			{
				uniqueZoneList.emplace(zoneItr);
			}

			uniqueSourcesList[source] = uniqueZoneList;
		}
	}

	return uniqueSourcesList;
}

template <typename T>
void exportProperty(VehicleProperty::Property prop, AbstractRoutingEngine *re, GDBusConnection *connection)
{
	T* t = new T(prop, re, connection);

	prop = t->objectName();

	/// check if we need more than one instance:

	PropertyList implementedProperties = t->wantsProperties();

	std::unordered_map<std::string, std::unordered_set<Zone::Type> > uniqueSourcesList = getUniqueSourcesList(re, implementedProperties);

	delete t;

	PropertyList supported = re->supported();

	for(auto itr : uniqueSourcesList)
	{
		std::unordered_set<Zone::Type> zones = itr.second;

		std::string source = itr.first;

		std::string objectPath = "/" + source;

		boost::algorithm::erase_all(objectPath, "-");

		for(auto zone : zones)
		{
			T* t = new T(prop, re, connection);
			std::stringstream fullobjectPath;
			fullobjectPath<< objectPath << "/" << zone << "/" <<t->objectName();
			t->setObjectPath(fullobjectPath.str());
			t->setSourceFilter(source);
			t->setZoneFilter(zone);
			t->supportedChanged(supported);
		}

	}
}

template <typename T>
void exportProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
{
	exportProperty<T>("", re, connection);
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
	exportProperty<AccelerationProperty>(re, connection);
	exportProperty<VehicleSpeedProperty>(re, connection);
	exportProperty<TireProperty>(re, connection);
	exportProperty<EngineSpeedProperty>(re, connection);
	exportProperty<VehiclePowerModeProperty>(re, connection);
	exportProperty<TripMeterProperty>(re, connection);
	exportProperty<TransmissionProperty>(re, connection);
	exportProperty<CruiseControlProperty>(re, connection);
	exportProperty<WheelBrakeProperty>(re, connection);
	exportProperty<BrakeOperation>(re, connection);
	exportProperty<LightStatusProperty>(re, connection);
	exportProperty<HornProperty>(re, connection);
	exportProperty<FuelProperty>(re, connection);
	exportProperty<EngineOilProperty>(re, connection);
	exportProperty<ExteriorBrightnessProperty>(re, connection);
	exportProperty<Temperature>(re, connection);
	exportProperty<RainSensor>(re, connection);
	exportProperty<WindshieldWiper>(re, connection);
	exportProperty<HVACProperty>(re, connection);
	exportProperty<ClimateControlProperty>(re, connection);
	exportProperty<WindowStatusProperty>(re, connection);
	exportProperty<DefrostProperty>(re, connection);
	exportProperty<Sunroof>(re, connection);
	exportProperty<ConvertibleRoof>(re, connection);
	exportProperty<VehicleId>(re, connection);
	exportProperty<VehicleTypeProperty>(re, connection);
	exportProperty<FuelInfoProperty>(re, connection);
	exportProperty<SizeProperty>(re, connection);
	exportProperty<DoorsProperty>(re, connection);
	exportProperty<WheelInformationProperty>(re, connection);
	exportProperty<OdometerProperty>(re, connection);
	exportProperty<FluidProperty>(re, connection);
	exportProperty<BatteryProperty>(re, connection);
	exportProperty<BatteryStatusProperty>(re, connection);
	exportProperty<SecurityAlertProperty>(re, connection);
	exportProperty<ParkingBrakeProperty>(re, connection);
	exportProperty<ParkingLightProperty>(re, connection);
	exportProperty<HazardLightProperty>(re, connection);
	exportProperty<LocationProperty>(re, connection);
	exportProperty<AntilockBrakingSystemProperty>(re, connection);
	exportProperty<TractionControlSystemProperty>(re, connection);
	exportProperty<VehicleTopSpeedLimitProperty>(re, connection);
	exportProperty<AirbagStatusProperty>(re, connection);
	exportProperty<DoorStatusProperty>(re, connection);
	exportProperty<SeatBeltStatusProperty>(re, connection);
	exportProperty<OccupantStatusProperty>(re, connection);
	exportProperty<ObstacleDistanceProperty>(re, connection);
	exportProperty<SeatPostionProperty>(re, connection);
	exportProperty<SteeringWheelPositionProperty>(re, connection);
	exportProperty<SteeringWheel>(re, connection);
	exportProperty<MirrorSettingProperty>(re, connection);
	exportProperty<ThrottlePosition>(re, connection);
	exportProperty<EngineCoolant>(re, connection);
	exportProperty<NightMode>(re, connection);
	exportProperty<DrivingMode>(re, connection);
	exportProperty<PowertrainTorque>(re, connection);
	exportProperty<AcceleratorPedalPosition>(re, connection);
	exportProperty<Chime>(re, connection);
	exportProperty<WheelTick>(re, connection);
	exportProperty<IgnitionTime>(re, connection);
	exportProperty<YawRate>(re, connection);
	exportProperty<TransmissionClutch>(re, connection);
	exportProperty<TransmissionOil>(re, connection);
	exportProperty<BrakeMaintenance>(re, connection);
	exportProperty<WasherFluid>(re, connection);
	exportProperty<MalfunctionIndicator>(re, connection);
	exportProperty<Diagnostics>(re, connection);
	exportProperty<MirrorProperty>(re, connection);
	exportProperty<SeatAdjustment>(re, connection);
	exportProperty<DriveMode>(re, connection);
	exportProperty<VehicleSound>(re, connection);
	exportProperty<ElectronicStabilityControl>(re, connection);
	exportProperty<ChildSafetyLock>(re, connection);
	exportProperty<SeatProperty>(re, connection);
	exportProperty<DoorProperty>(re, connection);
	exportProperty<WindshieldWiperStatus>(re, connection);
	exportProperty<SideWindowStatusProperty>(re, connection);
	exportProperty<AtmosphericPressure>(re, connection);
	exportProperty<LaneDepartureStatus>(re, connection);
	exportProperty<AlarmStatus>(re, connection);

	PropertyList list = VehicleProperty::customProperties();
	PropertyList implemented = AbstractDBusInterface::implementedProperties();

	for (auto prop : list)
	{
		if(!contains(implemented, prop))
		{
			exportProperty<CustomPropertyInterface>(prop, re, connection);
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
			exportProperty<UncategorizedPropertyInterface>(prop, re, connection);
		}
	}
}
