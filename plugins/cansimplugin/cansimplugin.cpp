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

#include <boost/assert.hpp>
#include <glib.h>
#include <deque>

#include <vehicleproperty.h>
#include <listplusplus.h>

#include <logger.h>
#include <ambplugin.h>

#include "cansimplugin.h"

static const char* DEFAULT_CAN_IF_NAME = "vcan0";

//----------------------------------------------------------------------------
// CANSimPlugin
//----------------------------------------------------------------------------

// library exported function for plugin loader
extern "C" AbstractSource* create(AbstractRoutingEngine* routingengine, std::map<std::string, std::string> config)
{
#ifndef UNIT_TESTS
	DEBUG_CONF("cansimplugin",
		CUtil::Logger::file_off|CUtil::Logger::screen_on,
		CUtil::Logger::EInfo, CUtil::Logger::EInfo
	);
#endif
	std::unique_ptr< AmbPlugin<CANSimPlugin> > plugin(new AmbPlugin<CANSimPlugin>(routingengine, config));
	plugin->init();
	return plugin.release();
}

//----------------------------------------------------------------------------
// CANSimPlugin
//----------------------------------------------------------------------------

const VehicleProperty::Property MappingTable = "MappingTable";

//
// IVIPOC signals
//
const VehicleProperty::Property BatteryStatus = "BatteryStatus";
PROPERTYTYPEBASIC(BatteryStatus, uint16_t)
const VehicleProperty::Property FullBatteryRange = "FullBatteryRange";
PROPERTYTYPEBASIC(FullBatteryRange, uint16_t)
const VehicleProperty::Property Weather = "Weather";
PROPERTYTYPEBASIC(Weather, uint16_t)
const VehicleProperty::Property AvgKW = "AvgKW";
PROPERTYTYPEBASIC(AvgKW, double)

#define ADDPROPERTY(property, default_value, zone_value) \
			addPropertySupport(zone_value, [](){ \
				return new property ## Type(default_value); \
			})

CANSimPlugin::CANSimPlugin(AbstractRoutingEngine* re, const map<string, string>& config, AbstractSource& parent) :
	AmbPluginImpl(re, config, parent)
{
	auto it = config.find("interfaces");
	if(it != config.end() && it->second.length()){
		std::unique_ptr<json_object, decltype(&json_object_put)> rootobject(json_tokener_parse(it->second.c_str()), &json_object_put);
		if(rootobject){
			g_assert(json_object_get_type(rootobject.get())==json_type_array);
			array_list *ifacelist = json_object_get_array(rootobject.get());
			if (ifacelist) {
				for(int i=0; i < array_list_length(ifacelist); ++i)
				{
					json_object* obj = (json_object*)array_list_get_idx(ifacelist,i);
					const char* str = obj ? json_object_get_string(obj) : nullptr;
					if(str){
						interfaces[str] = std::shared_ptr<CANBus>(new CANBus(*static_cast<CANObserver*>(this)));
					}
				}
			}
		}
	}
	// Default interface if none has been configured.
	if(interfaces.empty()){
		interfaces[DEFAULT_CAN_IF_NAME] = std::shared_ptr<CANBus>(new CANBus(*static_cast<CANObserver*>(this)));
	}

	addPropertySupport(
		Zone::None,
		[](){
			StringPropertyType *s = new StringPropertyType(MappingTable, "");
			s->zone = Zone::None;
			return s;
		}
	);

	//
	// IVIPOC signals
	//
	ADDPROPERTY(VehicleProperty::ChildLockStatus, false, Zone::None);
	ADDPROPERTY(VehicleProperty::LightHead, false, Zone::None);
	ADDPROPERTY(VehicleProperty::LightParking, false, Zone::None);
	ADDPROPERTY(VehicleProperty::AirConditioning, false, Zone::None);
	ADDPROPERTY(BatteryStatus, 58, Zone::None);
	ADDPROPERTY(FullBatteryRange, 350, Zone::None);
	ADDPROPERTY(VehicleProperty::ExteriorTemperature, 74, Zone::None);
	ADDPROPERTY(VehicleProperty::InteriorTemperature, 68, Zone::None);
	ADDPROPERTY(VehicleProperty::FrontWheelRadius, 0, Zone::None);
	ADDPROPERTY(Weather, 1, Zone::None);
	ADDPROPERTY(AvgKW, 28, Zone::None);
	ADDPROPERTY(VehicleProperty::VehicleSpeed, 65, Zone::None);
	ADDPROPERTY(VehicleProperty::Odometer, 75126, Zone::None);
	ADDPROPERTY(VehicleProperty::TransmissionShiftPosition, Transmission::Drive, Zone::None);
	ADDPROPERTY(VehicleProperty::NightMode, false, Zone::None);
	ADDPROPERTY(VehicleProperty::ExteriorBrightness, 1000, Zone::None);
	// HVAC
	ADDPROPERTY(VehicleProperty::LightHazard, false, Zone::None);
	ADDPROPERTY(VehicleProperty::SeatHeater, 0, Zone::FrontLeft);
	ADDPROPERTY(VehicleProperty::SeatHeater, 0, Zone::FrontRight);
	ADDPROPERTY(VehicleProperty::AirRecirculation, false, Zone::None);
	ADDPROPERTY(VehicleProperty::AirflowDirection, HVAC::Front, Zone::None);
	ADDPROPERTY(VehicleProperty::FanSpeed, 200, Zone::None);
	ADDPROPERTY(VehicleProperty::TargetTemperature, 68, Zone::Left);
	ADDPROPERTY(VehicleProperty::TargetTemperature, 68, Zone::Right);

	ADDPROPERTY(VehicleProperty::Defrost, false, Zone::Front);
	ADDPROPERTY(VehicleProperty::Defrost, false, Zone::Rear);

	ADDPROPERTY(VehicleProperty::VehiclePowerMode, Power::Run, Zone::None);
	// TirePresure
	ADDPROPERTY(VehicleProperty::TirePressure, 2.3, Zone::FrontLeft);
	ADDPROPERTY(VehicleProperty::TirePressure, 2.3, Zone::FrontRight);
	ADDPROPERTY(VehicleProperty::TirePressure, 2.4, Zone::RearLeft);
	ADDPROPERTY(VehicleProperty::TirePressure, 2.4, Zone::RearRight);
}

CANSimPlugin::~CANSimPlugin()
{
	for(auto it = interfaces.begin(); it != interfaces.end(); ++it){
		it->second->stop();
	}
}

void CANSimPlugin::init()
{
	AmbPluginImpl::init();
	for(auto it = interfaces.begin(); it != interfaces.end(); ++it){
		it->second->start(it->first.c_str());
	}
}

void CANSimPlugin::supportedChanged(const PropertyList& supportedProperties)
{
	PropertyList s = const_cast<PropertyList&>(supportedProperties);
	removeOne(&s, MappingTable);// CANSimPlugin has own copy of the PropertyList from AmbPlugin
	createMappingTable(supportedProperties);
}

int CANSimPlugin::supportedOperations() const
{
	return AbstractSource::Get | AbstractSource::Set;
}

void CANSimPlugin::createMappingTable(const PropertyList& /*supported*/)
{
	//
	// Local helper classes
	//
	class JsonObject : public std::unique_ptr<json_object, decltype(&json_object_put)>
	{
	public:
		JsonObject(json_object* object) : std::unique_ptr<json_object, decltype(&json_object_put)>(object, &json_object_put) {}
	};

	class PROPERTY{
	public:
		PROPERTY(const VehicleProperty::Property& propertyName, const Zone::Type& z, int canId) :
			name(propertyName),
			zone(z),
			can_id(canId)
		{
		}
		PROPERTY(const PROPERTY& other) = delete;
		PROPERTY& operator=(const PROPERTY& other) = delete;
		PROPERTY(PROPERTY&& other) = default;
		PROPERTY& operator=(PROPERTY&& other) = default;
		JsonObject toJson()
		{
			JsonObject prop(json_object_new_object());
			json_object_object_add(prop.get(), "can_id", json_object_new_int(can_id));
			json_object_object_add(prop.get(), "name", json_object_new_string(name.c_str()));
			json_object_object_add(prop.get(), "zone", json_object_new_int(static_cast<int>(zone)));
			return prop;
		}
	private:
		std::string name;
		Zone::Type zone;
		int can_id;
	};

	//
	PropertyList allProperties(VehicleProperty::capabilities());

	removeOne(&allProperties, MappingTable);

	//
	// Create mapping table in JSON format
	//
	map< std::string, std::deque<PROPERTY> > table;
	PropertyList addedProperties;
	PropertyList removedProperties;
	std::map< canid_t, std::tuple< std::string, VehicleProperty::Property, Zone::Type> > newMappingTable;
	int can_id = 10; // Let's have a space for a special messages. Just in case .... in the future.
	for(PropertyList::const_iterator propIt = allProperties.begin(); propIt != allProperties.end(); ++propIt)
	{
		VehicleProperty::Property propertyName(*propIt);

		std::list<std::string> sources(routingEngine->sourcesForProperty(propertyName));
		size_t size = sources.size();

		bool IAmTheSource = contains(sources, uuid());

		if(size == 0 || size == 1 && IAmTheSource) {
			if( size == 0 ){
				// I'm the source from now
				ZonePropertyType& zonePropType = properties[propertyName];
				std::shared_ptr<AbstractPropertyType> value(VehicleProperty::getPropertyTypeForPropertyNameValue(propertyName));
				if(value){
					value->zone = Zone::None;
					zonePropType.insert(make_pair(Zone::None, value));
					addedProperties.push_back(propertyName);
				}
				else{
					properties.erase(propertyName);
				}
			}
			std::string source(uuid());
			//PropertyInfo info(routingEngine->getPropertyInfo(propertyName,source));
			//Zone::ZoneList zones(info.zones());
			//if(zones.empty())
			Zone::ZoneList zones;
			{
				for(int i = 0; i< 10; ++i){
					Zone::Type zone(Zone::None);
					if(i)
						zone = static_cast<Zone::Type>(1 << i);
					zones.push_back(zone);
				}
				zones.push_back(Zone::FrontRight);
				zones.push_back(Zone::FrontLeft);
				zones.push_back(Zone::MiddleRight);
				zones.push_back(Zone::MiddleLeft);
				zones.push_back(Zone::RearRight);
				zones.push_back(Zone::RearLeft);
			}
			for( auto z=zones.begin(); z != zones.end(); ++z ){
				table[source].push_back(PROPERTY(propertyName, *z, can_id));
				newMappingTable[can_id++] = make_tuple(source, propertyName, *z);
			}
		}
		else if(IAmTheSource){
			// I'm the source, and there is another source
			properties.erase(propertyName);// I don't need to simulate it anymore
			removedProperties.push_back(propertyName);
		}
	}

	if(addedProperties.size() || removedProperties.size()) {
		JsonObject sources(json_object_new_array());
		for(auto it = table.begin(); it != table.end(); ++it) {
			// one source object:
			JsonObject source(json_object_new_object());
			JsonObject description(json_object_new_object());
			json_object_object_add(description.get(), "guid", json_object_new_string(it->first.c_str()));
			json_object_object_add(source.get(), "source", description.release());
			// signals:
			JsonObject sigs(json_object_new_array());
			for(auto signalIt = it->second.begin(); signalIt != it->second.end(); ++signalIt) {
				json_object_array_add(sigs.get(), signalIt->toJson().release());
			}
			// add signals into source
			json_object_object_add(source.get(), "signals", sigs.release());
			// add one source into sources array
			json_object_array_add(sources.get(), source.release());
		}
		// result json:
		JsonObject result(json_object_new_object());
		json_object_object_add(result.get(), "sources", sources.release());

		std::string mappingTableValue(json_object_to_json_string(result.get()));

		std::replace(mappingTableValue.begin(), mappingTableValue.end(), '"', '\'');// replace all " to '
		auto tableProperty = properties[MappingTable][Zone::None];
		if(tableProperty){
			// we have a new MappingTable
			mappingTable.swap(newMappingTable);
			tableProperty->setValue(mappingTableValue);
			routingEngine->updateProperty(tableProperty.get(), uuid());
		}

		routingEngine->updateSupported(addedProperties, removedProperties, &source);
	}
}

// from CANObserver
void CANSimPlugin::errorOccured(CANObserver::CANError error)
{
	(void) error;
	LOG_INFO( "CANSimPlugin::errorOccured() not implemented "<< std::endl );
}

void CANSimPlugin::standardFrameReceived(const can_frame& frame)
{
	(void) frame;
	LOG_INFO( "CANSimPlugin::standardFrameReceived() not implemented "<< std::endl );
}

void CANSimPlugin::extendedFrameReceived(const can_frame& frame)
{
	LOG_INFO("CANSimPlugin::extendedFrameReceived()");
	printFrame(frame);

	auto it = mappingTable.find(frame.can_id);
	if( it == mappingTable.end()){
		LOG_WARNING("can_id not found");
		return;
	}

	std::string source(std::get<0>(it->second));
	VehicleProperty::Property name(std::get<1>(it->second));
	Zone::Type zone(std::get<2>(it->second));
	AbstractPropertyType* value = findPropertyType(name, zone);
	if(!value)
		return;

	std::unique_ptr<GVariant, decltype(&g_variant_unref)> v(value->toVariant(), &g_variant_unref);
	std::unique_ptr<GVariant, decltype(&g_variant_unref)> v_untrusted(
		g_variant_new_from_data( g_variant_get_type(v.release()), frame.data, frame.can_dlc, FALSE, nullptr, nullptr),
		&g_variant_unref
	);
	std::unique_ptr<GVariant, decltype(&g_variant_unref)> v_normal(g_variant_get_normal_form(v_untrusted.release()), &g_variant_unref);
	if(g_variant_is_normal_form(v_normal.get())) {
		value->fromVariant(v_normal.get());
		routingEngine->updateProperty(value, source);
	}
	else{
		LOG_ERROR("Can't convert value from CAN to GVariant");
	}
}

void CANSimPlugin::errorFrameReceived(const can_frame& frame)
{
	LOG_INFO("CANSimPlugin::errorFrameReceived()");
	printFrame(frame);
}

void CANSimPlugin::remoteTransmissionRequest(const can_frame& frame)
{
	(void) frame;
	LOG_INFO( "CANSimPlugin::remoteTransmissionRequest() not implemented "<< std::endl );
}

void CANSimPlugin::printFrame(const can_frame& frame) const
{
	LOG_INFO( "CANSimPlugin::printFrame can_id: " << std::hex << frame.can_id << std::dec << endl );
	LOG_INFO( "CANSimPlugin::printFrame can_dlc: " << int(frame.can_dlc) << endl );

	std::stringstream ss;
	for(int i=0; i<frame.can_dlc; ++i){
		ss << " " << std::hex << (int)(frame.data[i]);
	}
	ss << std::dec;

	LOG_INFO( "CANSimPlugin::printFrame can data" << ss.str() << endl );
}


