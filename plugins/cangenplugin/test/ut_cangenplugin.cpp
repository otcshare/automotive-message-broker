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

#include "logger.h"

#include <ltdl.h>
#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include <listplusplus.h>
#define private protected
#include "cangenplugin.h"
#undef private
#include "mockabstractroutingengine.h"
#include "mockcanbus.h"

typedef void* create_t(AbstractRoutingEngine*, std::map<std::string, std::string>);

class Loader{
public:
	Loader()
	{
		if(lt_dlinit())
		{
			cerr<<"error initializing libtool: "<<__FILE__<<" - "<<__FUNCTION__<<":"<<__LINE__<<" "<<lt_dlerror()<<endl;
			throw std::runtime_error("Error initializing libtool. aborting");
		}
	}

	~Loader()
	{
		auto handle = openHandles.begin();
		while(handle != openHandles.end())
			lt_dlclose(*handle++);
		lt_dlexit();
	}

	template<class T>
	T loadPlugin(std::string pluginName, AbstractRoutingEngine* routingEngine, std::map<std::string, std::string> config)
	{
		DebugOut()<<"Loading plugin: "<<pluginName<<endl;

		lt_dlhandle handle = lt_dlopenext(pluginName.c_str());

		if(!handle)
		{
			cerr<<"error opening plugin: "<<pluginName<<" in "<<__FILE__<<" - "<<__FUNCTION__<<":"<<__LINE__<<" "<<lt_dlerror()<<endl;
			return nullptr;
		}

		openHandles.push_back(handle);
		create_t* f_create = (create_t *)lt_dlsym(handle, "create");

		//mErrorString = lt_dlerror();

		if(f_create)
		{
			void* obj = f_create(routingEngine, config);
			return static_cast<T>( obj );
		}

		return nullptr;
	}
private:
	std::vector<lt_dlhandle> openHandles;
};

class TestCANGenPluginImpl : public CANGenPlugin
{
public:
	TestCANGenPluginImpl(AbstractRoutingEngine* re, map<string, string> config, AbstractSource& parent) :
		CANGenPlugin(re, config, parent)
	{
		mock().actualCall("CANGenPlugin::CANGenPlugin");
	}

	virtual ~TestCANGenPluginImpl()
	{
		mock().actualCall("CANGenPlugin::~CANGenPlugin");
	}

	void TestAddPropertyToMappingTable(const char* source, const int can_id, const char* name, const Zone::Type& zone)
	{
		std::unique_ptr<json_object, decltype(&json_object_put)> prop(json_object_new_object(), &json_object_put);
		json_object_object_add(prop.get(), "can_id", json_object_new_int(can_id));
		json_object_object_add(prop.get(), "name", json_object_new_string(name));
		json_object_object_add(prop.get(), "zone", json_object_new_int(static_cast<int>(zone)));
		mappingTable.addProperty(source, prop.get());
	}

	inline bool TestSendValue(const string& interface, AbstractPropertyType* value){ return sendValue(interface, value); }
	inline void TestSetCanBusInterface(const string& interface){
		interfaces[interface] = std::shared_ptr<CANBus>(new MockCANBus(*this));
	}
	inline int getCanId(const std::string& source, const Zone::Type& zone, const VehicleProperty::Property& name){
		return mappingTable.getCanId(source, zone, name);
	}
	inline bool TestSetAndGetValue(const std::string& property, const std::string& value, int zone, const std::string& interface, const std::string& transactionId)
	{
		setValue(nullptr, property, value, zone, interface, transactionId);

		getValue(nullptr, property, zone, transactionId);
	}
};

class TestCANGenPlugin : public AmbPlugin<TestCANGenPluginImpl>
{
public:
	TestCANGenPlugin(AbstractRoutingEngine* re, map<string, string> config) :
		AmbPlugin<TestCANGenPluginImpl>(re, config)
	{
		mock().actualCall("AmbPlugin::AmbPlugin");
	}

	virtual ~TestCANGenPlugin()
	{
		mock().actualCall("AmbPlugin::~AmbPlugin");
	}

	inline TestCANGenPluginImpl* getImplementation() { return dynamic_cast<TestCANGenPluginImpl*>(d.get()); }
};

TEST_GROUP(CANGenPlugin)
{
	MockAbstractRoutingEngine *routingEngine;
	std::map<std::string, std::string> config;
	TestCANGenPlugin *plugin;

	void setup()
	{
		DebugOut::setDebugThreshhold(0);
		VehicleProperty::factory();
		routingEngine = new MockAbstractRoutingEngine();
		plugin = nullptr;
		mock().expectOneCall("AmbPlugin::AmbPlugin");
		mock().expectOneCall("CANGenPlugin::CANGenPlugin");
		//mock().expectOneCall("CANBus::start").andReturnValue(1);
		mock().expectOneCall("AbstractRoutingEngine::setSupported");
		plugin = new TestCANGenPlugin(routingEngine, config);
	}
	void teardown()
	{
		//mock().expectOneCall("CANBus::stop");
		mock().expectOneCall("CANGenPlugin::~CANGenPlugin");
		mock().expectOneCall("AmbPlugin::~AmbPlugin");
		if(plugin != nullptr) {
			delete plugin;
			plugin = nullptr;
		}
		VehicleProperty::shutdown();

		delete routingEngine;
		routingEngine = nullptr;

		mock().checkExpectations();
		mock().clear();
	}
};

TEST(CANGenPlugin, create)
{

	//mock().expectOneCall("CANSimPlugin::CANSimPlugin");
	Loader ldr;

	mock().expectOneCall("AbstractRoutingEngine::setSupported");
	AbstractSource* myplugin = ldr.loadPlugin<AbstractSource*>("./build/cangenplugin.so", routingEngine, config);
	CHECK( myplugin != nullptr);
	delete myplugin;
}

TEST(CANGenPlugin, uuid)
{
	std::string uuid = plugin->uuid();
	//CHECK_EQUAL_C_STRING("3f43e231-11ec-4782-9b5a-3dbcc5221eeb", uuid.c_str());
	CHECK(uuid.length() > 0);
}

TEST(CANGenPlugin, propertyChanged)
{
	plugin->propertyChanged(nullptr);

	std::unique_ptr<AbstractPropertyType> value(VehicleProperty::getPropertyTypeForPropertyNameValue(VehicleProperty::VehicleSpeed, "55"));
	value->zone = Zone::None;
	value->sourceUuid = plugin->uuid();

	plugin->propertyChanged(value.get());

	value = std::unique_ptr<AbstractPropertyType>(new StringPropertyType("MappingTable", "{}"));
	plugin->propertyChanged(value.get());

}

TEST(CANGenPlugin, parseMappingTable)
{
	std::unique_ptr<AbstractPropertyType> value(new StringPropertyType("MappingTable", "{"));
	plugin->propertyChanged(value.get());

	value = std::unique_ptr<AbstractPropertyType>(new StringPropertyType("MappingTable", "{ "
			"'sources': [ { 'source': { 'guid': '3f43e231-11ec-4782-9b5a-3dbcc5221eeb' }, "
			"'signals': [ "
				"{ 'can_id': 10, 'name': 'SeatHeater', 'zone': 0 }, "
				"{ 'can_id': 11, 'name': 'SeatHeater', 'zone': 2 }, "
				"{ 'can_id': 12, 'name': 'SeatHeater', 'zone': 4 }, "
				"{ 'can_id': 13, 'name': 'SeatHeater', 'zone': 8 }, "
				"{ 'can_id': 14, 'name': 'SeatHeater', 'zone': 16 }, "
				"{ 'can_id': 15, 'name': 'SeatHeater', 'zone': 32 }, "
				"{ 'can_id': 16, 'name': 'SeatHeater', 'zone': 64 }, "
				"{ 'can_id': 17, 'name': 'SeatHeater', 'zone': 128 }, "
				"{ 'can_id': 18, 'name': 'SeatHeater', 'zone': 256 }, "
				"{ 'can_id': 19, 'name': 'SeatHeater', 'zone': 512 }, "
				"{ 'can_id': 20, 'name': 'SeatHeater', 'zone': 5 }, "
				"{ 'can_id': 21, 'name': 'SeatHeater', 'zone': 9 }, "
				"{ 'can_id': 22, 'name': 'SeatHeater', 'zone': 6 }, "
				"{ 'can_id': 23, 'name': 'SeatHeater', 'zone': 10 }, "
				"{ 'can_id': 24, 'name': 'SeatHeater', 'zone': 20 }, "
				"{ 'can_id': 25, 'name': 'SeatHeater', 'zone': 24 }"
			" ] } ] }"));
	plugin->propertyChanged(value.get());
	CHECK(20 == plugin->getImplementation()->getCanId(
			std::string("3f43e231-11ec-4782-9b5a-3dbcc5221eeb"),
			Zone::Type(5), VehicleProperty::Property("SeatHeater")));

	value = std::unique_ptr<AbstractPropertyType>(new StringPropertyType("MappingTable", "{ "
				"'signals': [ "
					"{ 'can_id': 10, 'name': 'SeatHeater', 'zone': 0 }, "
					"{ 'can_id': 11, 'name': 'SeatHeater', 'zone': 2 }, "
					"{ 'can_id': 12, 'name': 'SeatHeater', 'zone': 4 }, "
					"{ 'can_id': 13, 'name': 'SeatHeater', 'zone': 8 }, "
					"{ 'can_id': 14, 'name': 'SeatHeater', 'zone': 16 }, "
					"{ 'can_id': 15, 'name': 'SeatHeater', 'zone': 32 }, "
					"{ 'can_id': 16, 'name': 'SeatHeater', 'zone': 64 }, "
					"{ 'can_id': 17, 'name': 'SeatHeater', 'zone': 128 }, "
					"{ 'can_id': 18, 'name': 'SeatHeater', 'zone': 256 }, "
					"{ 'can_id': 19, 'name': 'SeatHeater', 'zone': 512 }, "
					"{ 'can_id': 20, 'name': 'SeatHeater', 'zone': 5 }, "
					"{ 'can_id': 21, 'name': 'SeatHeater', 'zone': 9 }, "
					"{ 'can_id': 22, 'name': 'SeatHeater', 'zone': 6 }, "
					"{ 'can_id': 23, 'name': 'SeatHeater', 'zone': 10 }, "
					"{ 'can_id': 24, 'name': 'SeatHeater', 'zone': 20 }, "
					"{ 'can_id': 25, 'name': 'SeatHeater', 'zone': 24 }"
				" ] } ] }"));
	plugin->propertyChanged(value.get());
	CHECK(20 != plugin->getImplementation()->getCanId(
				std::string("3f43e231-11ec-4782-9b5a-3dbcc5221eeb"),
				Zone::Type(5), VehicleProperty::Property("SeatHeater"))
	);
}

TEST(CANGenPlugin, errorOccured)
{
	plugin->getImplementation()->errorOccured(static_cast<CANObserver::CANError>(-1));

	CANObserver::CANError error(CANObserver::GENERAL_ERROR);
	plugin->getImplementation()->errorOccured(error);
}

TEST(CANGenPlugin, standardFrameReceived)
{
	can_frame frame;
	memset(&frame, 0, sizeof(frame));
	frame.can_id = 0x00a;
	plugin->getImplementation()->standardFrameReceived(frame);
}

TEST(CANGenPlugin, extendedFrameReceived)
{
	can_frame frame;
	memset(&frame, 0, sizeof(frame));
	plugin->getImplementation()->extendedFrameReceived(frame);
}

TEST(CANGenPlugin, errorFrameReceived)
{
	can_frame frame;
	memset(&frame, 0, sizeof(frame));
	plugin->getImplementation()->errorFrameReceived(frame);
}

TEST(CANGenPlugin, remoteTransmissionRequest)
{
	can_frame frame;
	memset(&frame, 0, sizeof(frame));
	plugin->getImplementation()->remoteTransmissionRequest(frame);
}

TEST(CANGenPlugin, sendValue)
{
	plugin->getImplementation()->TestSetCanBusInterface("vcan98");
	CHECK(plugin->getImplementation()->TestSendValue("vcan98", nullptr) == false);

	std::unique_ptr<AbstractPropertyType> value(VehicleProperty::getPropertyTypeForPropertyNameValue(VehicleProperty::DoorsPerRow, "[2,2]"));
	CHECK(plugin->getImplementation()->TestSendValue("vcan98", value.get()) == false);

	plugin->getImplementation()->TestAddPropertyToMappingTable(plugin->uuid().c_str(), 0xA, VehicleProperty::VehicleSpeed.c_str(), Zone::None);
	value = std::unique_ptr<AbstractPropertyType>(VehicleProperty::getPropertyTypeForPropertyNameValue(VehicleProperty::VehicleSpeed, "55"));
	value->zone = Zone::None;
	value->sourceUuid = plugin->uuid();
	plugin->getImplementation()->TestSetCanBusInterface("vcan98");
	mock().expectOneCall("CANBus::sendExtendedFrame").andReturnValue(0);
	CHECK(plugin->getImplementation()->TestSendValue("vcan98", value.get()) == false);

	plugin->getImplementation()->TestSetCanBusInterface("vcan99");
	mock().expectOneCall("CANBus::sendExtendedFrame").andReturnValue(1);
	CHECK(plugin->getImplementation()->TestSendValue("vcan99", value.get()) == true);
	mock().expectNCalls(2,"CANBus::stop");
}

TEST(CANGenPlugin, SetAndGetValue)
{
	plugin->getImplementation()->TestSetCanBusInterface("vcan0");
	mock().expectOneCall("CANBus::sendExtendedFrame").andReturnValue(1);
	plugin->getImplementation()->TestAddPropertyToMappingTable(CANSimPluginUUID, 0xA, VehicleProperty::VehicleSpeed.c_str(), Zone::None);
	plugin->getImplementation()->TestSetAndGetValue(VehicleProperty::VehicleSpeed, "33", 0, "vcan0", "trans1");
	mock().expectOneCall("CANBus::stop");
}

TEST(CANGenPlugin, dataReceived)
{
	const char *get = R"({"type":"method","name":"get","transactionid":"862bb93d-a302-9a58-baa9-d90265ac843c","data":[{"property":"VehicleSpeed","zone":"0"}]})";
	plugin->getImplementation()->dataReceived(nullptr, get, strlen(get));
	const char *set = R"({"type":"method","name":"set","transactionid":"d5935c94-7b05-fd67-56bc-31c320185035","data":[{"interface":"vcan0","property":"VehicleSpeed","value":"33","zone":"0"}]})";
	plugin->getImplementation()->dataReceived(nullptr, set, strlen(set));
}
