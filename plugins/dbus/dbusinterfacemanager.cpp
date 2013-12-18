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

///properties:
#include "runningstatus.h"
#include "custompropertyinterface.h"
#include "uncategorizedproperty.h"
#include "environmentproperties.h"
#include "vehicleinfo.h"
#include "maintenance.h"
#include "parking.h"
#include "drivingsafety.h"

using namespace std;


std::map<std::string, std::map<Zone::Type, bool> > getUniqueSourcesList(AbstractRoutingEngine *re, std::list<VehicleProperty::Property> implementedProperties)
{
	std::map<std::string, std::map<Zone::Type, bool>> uniqueSourcesList;

	for(auto itr = implementedProperties.begin(); itr != implementedProperties.end(); itr++)
	{
		VehicleProperty::Property property = *itr;
		std::list<std::string> sources = re->getSourcesForProperty(property);

		for(auto itr2 = sources.begin(); itr2 != sources.end(); itr2++)
		{
			std::string source = *itr2;

			PropertyInfo info = re->getPropertyInfo(property,source);

			std::map<Zone::Type, bool> uniqueZoneList;

			if(uniqueSourcesList.find(source) != uniqueSourcesList.end())
			{
				uniqueZoneList = uniqueSourcesList[source];
			}

			std::list<Zone::Type> zoneList = info.zones();

			if(!zoneList.size())
			{
				uniqueZoneList[Zone::None] = true;
			}

			for(auto zoneItr = zoneList.begin(); zoneItr != zoneList.end(); zoneItr++)
			{
				uniqueZoneList[*zoneItr] = true;
			}

			uniqueSourcesList[source] = uniqueZoneList;
		}
	}

	return uniqueSourcesList;
}

template <typename T>
void exportProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
{
	T* t = new T(re, connection);

	/// check if we need more than one instance:

	std::list<VehicleProperty::Property> implementedProperties = t->wantsProperties();

	std::map<std::string, std::map<Zone::Type, bool> > uniqueSourcesList = getUniqueSourcesList(re, implementedProperties);

	delete t;

	for(auto itr = uniqueSourcesList.begin(); itr != uniqueSourcesList.end(); itr++)
	{
		std::map<Zone::Type, bool> zones = (*itr).second;

		std::string source = (*itr).first;

		std::string objectPath = "/" + source;

		boost::algorithm::erase_all(objectPath, "-");

		for(auto zoneItr = zones.begin(); zoneItr != zones.end(); zoneItr++)
		{
			Zone::Type zone = (*zoneItr).first;
			T* t = new T(re, connection);
			std::stringstream fullobjectPath;
			fullobjectPath<< objectPath << "/" << zone << "/" <<t->objectName();
			t->setObjectPath(fullobjectPath.str());
			t->setSourceFilter(source);
			t->setZoneFilter(zone);
			t->supportedChanged(re->supported());
		}

	}
}

template <typename T>
void exportProperty(VehicleProperty::Property prop, AbstractRoutingEngine *re, GDBusConnection *connection)
{
	T* t = new T(prop, re, connection);

	/// check if we need more than one instance:

	std::list<VehicleProperty::Property> implementedProperties = t->wantsProperties();

	std::map<std::string, std::map<Zone::Type, bool> > uniqueSourcesList = getUniqueSourcesList(re, implementedProperties);

	delete t;

	for(auto itr = uniqueSourcesList.begin(); itr != uniqueSourcesList.end(); itr++)
	{
		std::map<Zone::Type, bool> zones = (*itr).second;

		std::string source = (*itr).first;

		std::string objectPath = "/" + source;

		boost::algorithm::erase_all(objectPath, "-");

		for(auto zoneItr = zones.begin(); zoneItr != zones.end(); zoneItr++)
		{
			Zone::Type zone = (*zoneItr).first;
			T* t = new T(prop, re, connection);
			std::stringstream fullobjectPath;
			fullobjectPath<< objectPath << "/" << zone << "/" <<t->objectName();
			t->setObjectPath(fullobjectPath.str());
			t->setSourceFilter(source);
			t->setZoneFilter(zone);
			t->supportedChanged(re->supported());
		}

	}
}

static void
on_bus_acquired (GDBusConnection *connection, const gchar *name, gpointer user_data)
{
	DBusInterfaceManager* iface = static_cast<DBusInterfaceManager*>(user_data);

	iface->connection = connection;

	new AutomotiveManager(connection);

	/// properties:
	exportProperty<AccelerationProperty>(iface->re,connection);
	exportProperty<VehicleSpeedProperty>(iface->re, connection);
	exportProperty<TirePressureProperty>(iface->re, connection);
	exportProperty<EngineSpeedProperty>(iface->re, connection);
	exportProperty<VehiclePowerModeProperty>(iface->re, connection);
	exportProperty<TripMeterProperty>(iface->re, connection);
	exportProperty<TransmissionProperty>(iface->re, connection);
	exportProperty<TireTemperatureProperty>(iface->re, connection);
	exportProperty<CruiseControlProperty>(iface->re, connection);
	exportProperty<WheelBrakeProperty>(iface->re, connection);
	exportProperty<LightStatusProperty>(iface->re, connection);
	exportProperty<HornProperty>(iface->re, connection);
	exportProperty<FuelProperty>(iface->re, connection);
	exportProperty<EngineOilProperty>(iface->re, connection);
	exportProperty<ExteriorBrightnessProperty>(iface->re, connection);
	exportProperty<Temperature>(iface->re, connection);
	exportProperty<RainSensor>(iface->re, connection);
	exportProperty<WindshieldWiper>(iface->re, connection);
	exportProperty<HVACProperty>(iface->re, connection);
	exportProperty<WindowStatusProperty>(iface->re, connection);
	exportProperty<Sunroof>(iface->re, connection);
	exportProperty<ConvertibleRoof>(iface->re, connection);
	exportProperty<VehicleId>(iface->re, connection);
	exportProperty<TransmissionInfoProperty>(iface->re, connection);
	exportProperty<VehicleTypeProperty>(iface->re, connection);
	exportProperty<FuelInfoProperty>(iface->re, connection);
	exportProperty<SizeProperty>(iface->re, connection);
	exportProperty<DoorsProperty>(iface->re, connection);
	exportProperty<WheelInformationProperty>(iface->re, connection);
	exportProperty<OdometerProperty>(iface->re, connection);
	exportProperty<FluidProperty>(iface->re, connection);
	exportProperty<BatteryProperty>(iface->re, connection);
	exportProperty<SecurityAlertProperty>(iface->re, connection);
	exportProperty<ParkingBrakeProperty>(iface->re, connection);
	exportProperty<ParkingLightProperty>(iface->re, connection);
	exportProperty<HazardLightProperty>(iface->re, connection);
	exportProperty<LocationProperty>(iface->re, connection);
	exportProperty<AntilockBrakingSystemProperty>(iface->re, connection);
	exportProperty<TractionControlSystemProperty>(iface->re, connection);
	exportProperty<VehicleTopSpeedLimitProperty>(iface->re, connection);
	exportProperty<AirbagStatusProperty>(iface->re, connection);
	exportProperty<DoorStatusProperty>(iface->re, connection);
	exportProperty<SeatBeltStatusProperty>(iface->re, connection);
	exportProperty<OccupantStatusProperty>(iface->re, connection);
	exportProperty<ObstacleDistanceProperty>(iface->re, connection);

	iface->supportedChanged(iface->re->supported());
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

void DBusInterfaceManager::supportedChanged(PropertyList supportedProperties)
{
	DebugOut()<<"supported Properties: "<<supportedProperties.size()<<endl;
	if(!connection)
	{
		DebugOut(DebugOut::Warning)<<"supportedChanged called before we have a dbus connection"<<endl;
		return;
	}

	PropertyList list = VehicleProperty::customProperties();
	PropertyList implemented = AbstractDBusInterface::implementedProperties();

	for (auto itr = list.begin(); itr != list.end(); itr++)
	{
		VehicleProperty::Property prop = *itr;

		if(!ListPlusPlus<VehicleProperty::Property>(&implemented).contains(prop))
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

		if(!ListPlusPlus<VehicleProperty::Property>(&implemented).contains(prop))
		{
			exportProperty<UncategorizedPropertyInterface>(prop, re, connection);
		}
	}
}


