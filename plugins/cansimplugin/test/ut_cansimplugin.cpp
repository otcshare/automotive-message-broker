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
#include "cansimplugin.h"
#undef private
#include "mockabstractroutingengine.h"
#include "mockcanbus.h"

const char* mappingTable = "MappingTable";

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

class TestCANSimPluginImpl : public CANSimPlugin
{
public:
    TestCANSimPluginImpl(AbstractRoutingEngine* re, map<string, string> config, AbstractSource& parent) :
        CANSimPlugin(re, config, parent)
    {
        mock().actualCall("CANSimPlugin::CANSimPlugin");

        for(auto it=interfaces.begin(); it != interfaces.end(); ++it){
            interfaces[it->first] = std::shared_ptr<CANBus>(new MockCANBus(*this));
        }
    }

    virtual ~TestCANSimPluginImpl()
    {
        mock().actualCall("CANSimPlugin::~CANSimPlugin");
    }

    std::string TestCreateMappingTable(const PropertyList& supported) {
        supportedChanged(supported);
        AbstractPropertyType* value = findPropertyType("MappingTable", Zone::None);
        if(!value)
            return "";
        return  value->toString();
    }

    inline void TestAddPropertyToMappingTable(const int id, const char* source, const char* propertyName, const Zone::Type& z)
    {
        mappingTable[id] = make_tuple(source, propertyName, z);
        ZonePropertyType& zonePropType = properties[propertyName];
        std::shared_ptr<AbstractPropertyType> value(VehicleProperty::getPropertyTypeForPropertyNameValue(propertyName));
        if(value){
            value->zone = z;
            zonePropType.insert(make_pair(z, value));
        }
        else{
            properties.erase(propertyName);
        }
    }
};

class TestCANSimPlugin : public AmbPlugin<TestCANSimPluginImpl>
{
public:
    TestCANSimPlugin(AbstractRoutingEngine* re, map<string, string> config) :
        AmbPlugin<TestCANSimPluginImpl>(re, config)
    {
        mock().actualCall("AmbPlugin::AmbPlugin");
    }

    virtual ~TestCANSimPlugin()
    {
        mock().actualCall("AmbPlugin::~AmbPlugin");
    }

    inline TestCANSimPluginImpl* getImplementation() { return dynamic_cast<TestCANSimPluginImpl*>(d.get()); }
};

TEST_GROUP(CANSimPlugin)
{
    MockAbstractRoutingEngine *routingEngine;
    std::map<std::string, std::string> config;
    TestCANSimPlugin *plugin;

    void setup()
    {
        DebugOut::setDebugThreshhold(0);
        VehicleProperty::factory();
        routingEngine = new MockAbstractRoutingEngine();
        plugin = nullptr;
        mock().expectOneCall("AmbPlugin::AmbPlugin");
        mock().expectOneCall("CANSimPlugin::CANSimPlugin");
        mock().expectOneCall("CANBus::stop"); // because of the interfaces[it->first] = std::shared_ptr<CANBus>(new MockCANBus(*this)); in TestCANSimPluginImpl
        mock().expectOneCall("AbstractRoutingEngine::setSupported");
        mock().expectNCalls(27, "AbstractRoutingEngine::supported"); // 27 well-known properties in AMB API
        config["interfaces"] = "[\"vcan999\"]";
        plugin = new TestCANSimPlugin(routingEngine, config);
    }
    void teardown()
    {
        //mock().expectOneCall("CANBus::stop");
        mock().expectOneCall("AmbPlugin::~AmbPlugin");
        mock().expectOneCall("CANSimPlugin::~CANSimPlugin");
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

TEST(CANSimPlugin, create)
{

    //mock().expectOneCall("CANSimPlugin::CANSimPlugin");
    Loader ldr;
    config.clear();// create plugin with emtpy interfaces list at least once.
    mock().expectOneCall("AbstractRoutingEngine::setSupported");
    mock().expectNCalls(32, "AbstractRoutingEngine::supported"); // we are calling 32 times addPropertySupport in AMB API
    AbstractSource* myplugin = ldr.loadPlugin<AbstractSource*>("./build/cansimplugin.so", routingEngine, config);
    CHECK( myplugin != nullptr);
    delete myplugin;
}

TEST(CANSimPlugin, getPropertyAsync)
{
    bool replySuccess(false);
    int replyError(-1);
    int replyZone(-1);
    std::string replySignalName("");

    // NULL pointer test
    plugin->getPropertyAsync(nullptr);

    // Invalid request test
    AsyncPropertyRequest requestInvalid;
    requestInvalid.timeout = 0;
    AsyncPropertyReply* reply = new AsyncPropertyReply(requestInvalid);
    plugin->getPropertyAsync(reply);
    delete reply; reply = nullptr;

    requestInvalid.property = "NotExists";
    requestInvalid.completed = [&](AsyncPropertyReply* reply)
        {
            replySuccess = reply->success;
            replyError = reply->error;
            if(reply->value) {
                LOG_TRACE("getPropertyAsync completed, value is: " << reply->value->toString());
                replyZone = reply->value->zone;
                replySignalName = reply->value->name;
            }
            delete reply;
        };

    reply = new AsyncPropertyReply(requestInvalid);
    plugin->getPropertyAsync(reply);

    CHECK(replySuccess == false);
    CHECK(replyError == AsyncPropertyReply::InvalidOperation);

    plugin->getImplementation()->TestAddPropertyToMappingTable(0xa, plugin->uuid().c_str(), VehicleProperty::SeatHeater.c_str(), Zone::FrontLeft);
    AsyncPropertyRequest request;
    request.timeout = 0;
    request.property = VehicleProperty::SeatHeater;
    request.zoneFilter = Zone::FrontLeft;
    request.completed = requestInvalid.completed;

    reply = new AsyncPropertyReply(request);
    plugin->getPropertyAsync(reply);

    CHECK(replySuccess == true);
    CHECK_EQUAL_C_STRING(request.property.c_str(), replySignalName.c_str());
    CHECK_EQUAL_C_INT(request.zoneFilter, replyZone);

}

TEST(CANSimPlugin, getRangePropertyAsync)
{
    bool replySuccess(false);
    int replyError(-1);

    plugin->getRangePropertyAsync(nullptr);

    AsyncRangePropertyRequest requestInvalid;
    AsyncRangePropertyReply* reply = new AsyncRangePropertyReply(requestInvalid);
    plugin->getRangePropertyAsync(reply);
    delete reply; reply = nullptr;

    AsyncRangePropertyRequest request;
    request.properties.push_back(VehicleProperty::SeatHeater);
    request.completed = [&](AsyncRangePropertyReply* reply)
    {
        replySuccess = reply->success;
        replyError = reply->error;
        delete reply;
    };

    reply = new AsyncRangePropertyReply(request);
    plugin->getRangePropertyAsync(reply);

    CHECK(replySuccess == false);
    CHECK(replyError == AsyncPropertyReply::InvalidOperation);
}

TEST(CANSimPlugin, setProperty)
{
    bool replySuccess(false);
    int replyError(-1);
    std::string replySignalName("");

    // Invalid request test
    AsyncSetPropertyRequest requestInvalid;
    requestInvalid.timeout = 0;
    AsyncPropertyReply* reply = plugin->setProperty(requestInvalid);
    delete reply; reply = nullptr;

    requestInvalid.property = "NotExists";
    requestInvalid.completed = [&](AsyncPropertyReply* reply)
        {
            replySuccess = reply->success;
            replyError = reply->error;
            delete reply;
        };

    reply = plugin->setProperty(requestInvalid);

    CHECK(replySuccess == false);
    CHECK(replyError == AsyncPropertyReply::InvalidOperation);

    plugin->getImplementation()->TestAddPropertyToMappingTable(0xa, plugin->uuid().c_str(), VehicleProperty::SeatHeater.c_str(), Zone::FrontLeft);
    mock().expectOneCall("AbstractRoutingEngine::updateProperty");
    AsyncSetPropertyRequest request;
    request.timeout = 0;
    request.property = VehicleProperty::SeatHeater;
    request.zoneFilter = Zone::FrontLeft;
    request.value = VehicleProperty::getPropertyTypeForPropertyNameValue(request.property, "1");
    request.completed = requestInvalid.completed;

    reply = plugin->setProperty(request);
    delete request.value;
    request.value = nullptr;

    CHECK(replySuccess == true);
    CHECK(replyError == AsyncPropertyReply::NoError);
}

TEST(CANSimPlugin, subscribeUnsubscribeToPropertyChanges)
{
    plugin->subscribeToPropertyChanges("");

    // AbstractRoutingEngine::updateProperty is called so many times as many zones signal has
    mock().expectNCalls(1, "AbstractRoutingEngine::updateProperty");
    plugin->subscribeToPropertyChanges(VehicleProperty::VehicleSpeed);// only Zone::None

    // empty implemnetation, does nothing
    plugin->unsubscribeToPropertyChanges("");
    plugin->unsubscribeToPropertyChanges(VehicleProperty::VehicleSpeed);

}

TEST(CANSimPlugin, supported)
{
    PropertyList supported = plugin->supported();
    CHECK_EQUAL_C_INT(29, supported.size());
}

TEST(CANSimPlugin, supportedOperations)
{
    int supported = plugin->supportedOperations();
    CHECK_EQUAL_C_INT(AbstractSource::Get | AbstractSource::Set, supported);
}

TEST(CANSimPlugin, getPropertyInfo)
{
    PropertyInfo pi = plugin->getPropertyInfo(VehicleProperty::EngineSpeed);
    CHECK(!pi.isValid());
    std::list<Zone::Type> zones = pi.zones();
    CHECK(zones.empty());
    pi = plugin->getPropertyInfo(mappingTable);
    CHECK(pi.isValid());
    zones = pi.zones();
    CHECK(ListPlusPlus<Zone::Type>(&zones).contains(Zone::None));
}

TEST(CANSimPlugin, uuid)
{
    std::string uuid = plugin->uuid();
    //CHECK_EQUAL_C_STRING("3f43e231-11ec-4782-9b5a-3dbcc5221eeb", uuid.c_str());
    CHECK(uuid.length() > 0);
}

TEST(CANSimPlugin, BasicPropertyType_fromString_and_toString)
{

    //VehicleProperty::DefrostType *defrost = new VehicleProperty::DefrostType();
    //defrost->append(Window::SideMirrorLeft, true);
    //defrost->append(Window::SideMirrorRight, false);
    //defrost->fromString(defrost->toString());
    //CHECK_EQUAL_C_STRING("{'6':'1', '7':'0'}", defrost->toString().c_str());
    //delete defrost;

    BasicPropertyType<bool> boolean(true);
    boolean.fromString(boolean.toString());
    CHECK_EQUAL_C_STRING("1", boolean.toString().c_str());

}

TEST(CANSimPlugin, createMappingTable)
{
    const char* expected = "{ "
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
        " ] } ] }";

    std::list<std::string> supportedProps(VehicleProperty::capabilities());
    supportedProps.remove(VehicleProperty::SeatHeater);
    mock().setData("properties", &supportedProps);
    std::list<std::string> sources({"3f43e231-11ec-4782-9b5a-3dbcc5221eeb", "another-source"});
    mock().setData("sources", &sources);
    PropertyInfo pi(1, {Zone::FrontLeft, Zone::FrontRight});
    mock().setData("PropertyInfo", &pi);
    mock().expectNCalls(supportedProps.size(), "AbstractRoutingEngine::sourcesForProperty");
    //mock().expectNCalls(supportedProps.size()*(sources.size()-1), "AbstractRoutingEngine::getPropertyInfo");
    mock().expectOneCall("AbstractRoutingEngine::supported");
    mock().expectOneCall("AbstractRoutingEngine::updateProperty");
    //mock().expectOneCall("AbstractRoutingEngine::setSupported");

    const std::string& mappingTable = plugin->getImplementation()->TestCreateMappingTable(routingEngine->supported());
    LOG_ERROR("json: " << mappingTable);

    CHECK(mappingTable.length() > 0);
    json_object* mp = json_tokener_parse(mappingTable.c_str());
    json_object_put(mp);
    CHECK(mp != nullptr);
    CHECK_EQUAL_C_STRING(expected, mappingTable.c_str());

}

TEST(CANSimPlugin, errorOccured)
{
    plugin->getImplementation()->errorOccured(static_cast<CANObserver::CANError>(-1));

    CANObserver::CANError error(CANObserver::GENERAL_ERROR);
    plugin->getImplementation()->errorOccured(error);
}

TEST(CANSimPlugin, standardFrameReceived)
{
    can_frame frame;
    memset(&frame, 0, sizeof(frame));
    frame.can_id = 0x00a;
    plugin->getImplementation()->standardFrameReceived(frame);
}

TEST(CANSimPlugin, extendedFrameReceived)
{
    can_frame frame;
    memset(&frame, 0, sizeof(frame));
    frame.can_id = 0xFFFFFFFF;// not existing
    mock().expectNCalls(0,"AbstractRoutingEngine::updateProperty");// no prop. with this can_id
    plugin->getImplementation()->extendedFrameReceived(frame);

    plugin->getImplementation()->TestAddPropertyToMappingTable(0xb, plugin->uuid().c_str(), "NotExisttingProperty", Zone::None);
    frame.can_id = 0xb;
    frame.can_dlc = 8;
    mock().expectNCalls(0,"AbstractRoutingEngine::updateProperty");// no prop. with this can_id
    plugin->getImplementation()->extendedFrameReceived(frame);

    // existing property VehicleSpeed
    plugin->getImplementation()->TestAddPropertyToMappingTable(0xa, plugin->uuid().c_str(), VehicleProperty::VehicleSpeed.c_str(), Zone::None);
    frame.can_id = 0xa;
    mock().expectOneCall("AbstractRoutingEngine::updateProperty");
    plugin->getImplementation()->extendedFrameReceived(frame);

}

TEST(CANSimPlugin, errorFrameReceived)
{
    can_frame frame;
    memset(&frame, 0, sizeof(frame));
    plugin->getImplementation()->errorFrameReceived(frame);
}

TEST(CANSimPlugin, remoteTransmissionRequest)
{
    can_frame frame;
    memset(&frame, 0, sizeof(frame));
    plugin->getImplementation()->remoteTransmissionRequest(frame);
}
