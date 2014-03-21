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

#include "cangenplugin.h"

//----------------------------------------------------------------------------
// CANGenPlugin
//----------------------------------------------------------------------------

// library exported function for plugin loader
extern "C" AbstractSource* create(AbstractRoutingEngine* routingengine, std::map<std::string, std::string> config)
{
#ifndef UNIT_TESTS
    DEBUG_CONF("cangenplugin",
        CUtil::Logger::file_off|CUtil::Logger::screen_on,
        CUtil::Logger::EInfo, CUtil::Logger::EInfo
    );
#endif

    std::unique_ptr< AmbPlugin<CANGenPlugin> > plugin(new AmbPlugin<CANGenPlugin>(routingengine, config));
    plugin->init();
    return plugin.release();
}

//----------------------------------------------------------------------------
// CANGenPlugin
//----------------------------------------------------------------------------

CANGenPlugin::CANGenPlugin(AbstractRoutingEngine* re, const map<string, string>& config, AbstractSource& parent) :
    AmbPluginImpl(re, config, parent),
    ws(new WebSockets(*this))
{
}

CANGenPlugin::~CANGenPlugin()
{
    for(auto it = interfaces.begin(); it != interfaces.end(); ++it){
        it->second->stop();
    }
}

void CANGenPlugin::init()
{
    routingEngine->subscribeToProperty("MappingTable", &parent);
}

void CANGenPlugin::propertyChanged(AbstractPropertyType* value)
{
    if(!value)
        return;
    if(!value->name.compare("MappingTable")) {
        parseMappingTable(value->toString());
    }
}

void CANGenPlugin::parseMappingTable(const std::string& table)
{
    scoped_lock<interprocess_recursive_mutex> lock(mutex);

    std::string json(table);
    std::replace(json.begin(), json.end(), '\'', '"');// replace all ' to "
    std::unique_ptr<json_object, decltype(&json_object_put)> rootobject(json_tokener_parse(json.c_str()), &json_object_put);
    if(!rootobject)
    {
        LOG_ERROR("Failed to parse json: " << json);
        return;
    }

    // Success, use json_obj here.
    mappingTable.clear();
    json_object *sources = json_object_object_get(rootobject.get(),"sources");
    if(!sources)
        return;
    array_list* arraySources = json_object_get_array(sources);
    if(!arraySources)
        return;
    for(int i=0; i < array_list_length(arraySources); ++i)
    {
        json_object *rootsource = static_cast<json_object*>(array_list_get_idx(arraySources,i));
        if(!rootsource)
            continue;
        json_object* source = json_object_object_get(rootsource, "source");
        if(!source)
            continue;
        json_object* guid = json_object_object_get(source, "guid");
        const std::string guidstr(guid ? json_object_get_string(guid) : "");
        json_object* signals = json_object_object_get(rootsource, "signals");
        if(!signals)
            continue;
        array_list* arraySignals = json_object_get_array(signals);
        for(int j = 0; j < array_list_length(arraySignals); ++j)
        {
            json_object *signal = static_cast<json_object*>(array_list_get_idx(arraySignals,j));
            if(!signal)
                continue;
            mappingTable.addProperty(guidstr, signal);
        }// signals array loop
    }// sources array loop
}

// from CANObserver
void CANGenPlugin::errorOccured(CANObserver::CANError error)
{
	(void) error;
    LOG_INFO( "CANPlugin::errorOccured() not implemented "<< std::endl );
}

void CANGenPlugin::standardFrameReceived(const can_frame& frame)
{
	(void) frame;
    LOG_INFO( "CANPlugin::standardFrameReceived() not implemented "<< std::endl );
}

void CANGenPlugin::extendedFrameReceived(const can_frame& frame)
{
    LOG_INFO("CANPlugin::extendedFrameReceived()");
    printFrame(frame);
}

void CANGenPlugin::errorFrameReceived(const can_frame& frame)
{
    LOG_INFO("CANPlugin::errorFrameReceived()");
    printFrame(frame);
}

void CANGenPlugin::remoteTransmissionRequest(const can_frame& frame)
{
	(void) frame;
    LOG_INFO( "CANPlugin::remoteTransmissionRequest() not implemented "<< std::endl );
}

void CANGenPlugin::printFrame(const can_frame& frame) const
{
    LOG_INFO( "CANPlugin::printFrame can_id: " << std::hex << frame.can_id << std::dec << endl );
    LOG_INFO( "CANPlugin::printFrame can_dlc: " << int(frame.can_dlc) << endl );

    std::stringstream ss;
    for(int i=0; i<frame.can_dlc; ++i){
        ss << " " << std::hex << (int)(frame.data[i]);
    }
    ss << std::dec;

    LOG_INFO( "CANPlugin::printFrame can data" << ss.str() << endl );
}

bool CANGenPlugin::sendValue(const std::string& interface, AbstractPropertyType* value)
{
    scoped_lock<interprocess_recursive_mutex> lock(mutex);

    if(!value)
        return false;
    int can_id = mappingTable.getCanId(value->sourceUuid, value->zone, value->name);
    if(can_id == 0)
        return false;

    can_frame frame;
    frame.can_id = can_id;
    std::unique_ptr<GVariant, decltype(&g_variant_unref)> v(value->toVariant(), &g_variant_unref);
    gsize vs = g_variant_get_size(v.get());
    assert(vs <= sizeof(frame.data));// Has to be <= 8
    frame.can_dlc = vs > sizeof(frame.data) ? sizeof(frame.data) : vs;
    memcpy(frame.data, g_variant_get_data(v.get()), frame.can_dlc);

    auto& canBus = interfaces[interface];
    if(!canBus){
        canBus = std::shared_ptr<CANBus>(new CANBus(*static_cast<CANObserver*>(this)));
        bool started(canBus->start(interface.c_str()));
        if(!started)
            return false;
    }
    return canBus->sendExtendedFrame(frame);
}

void CANGenPlugin::getValue(libwebsocket* socket, const std::string& property, int zone, const std::string& id)
{
    AsyncPropertyRequest request;
    PropertyList foo = VehicleProperty::capabilities();
    if (ListPlusPlus<VehicleProperty::Property>(&foo).contains(property))
    {
        request.property = property;
    }
    else
    {
        DebugOut(0)<<"websocketsink: Invalid property requested: "<<property;
        return;
    }

    request.zoneFilter = zone;
    request.completed = [socket,id,property](AsyncPropertyReply* reply)
    {
        DebugOut()<<"Got property: "<<reply->property.c_str()<<endl;
        if(!reply->value){
            DebugOut()<<"Property value is null"<<endl;
            delete reply;
            return;
        }

        stringstream s;
        s.precision(15);

        s << "{\"type\":\"methodReply\",\"name\":\"get\",\"data\":{";
        s << "\"property\":\"" << property << "\",\"zone\":\"" << reply->value->zone << "\",\"value\":\"" << reply->value->toString() << "\",\"timestamp\":\""<<reply->value->timestamp<<"\",";
        s <<"\"sequence\": \""<<reply->value->sequence<<"\"}";
        s << ",\"transactionid\":\"" << id << "\"}";

        string replystr = s.str();
        //printf("Reply: %s\n",replystr.c_str());
        LOG_MESSAGE("Reply:" << replystr << endl);

        //if(replystr.length() > 4096){
        //    WebSockets::Write(socket, replystr.substr(0,4096));
        //    WebSockets::Write(socket, replystr.substr(4096, 4096));
        //}
        //else WebSockets::Write(socket, replystr);
        WebSockets::Write(socket, replystr);

        delete reply;
    };

    routingEngine->getPropertyAsync(request);
}

void CANGenPlugin::setValue(libwebsocket* socket, const std::string& property, const std::string& value, int zone, const std::string& interface, const std::string& transactionId)
{
    LOG_MESSAGE( "CANGenPlugin::setValue called with arguments:" << property << ", " << value << endl);

    bool sent(false);
    std::unique_ptr<AbstractPropertyType> type(VehicleProperty::getPropertyTypeForPropertyNameValue(property,value));
    if(type) {
        type->zone = zone;
        type->sourceUuid = CANSimPluginUUID;

        sent = sendValue(interface, type.get());
    }

    stringstream ss;
    ss << "{\"type\":\"methodReply\",\"name\":\"set\",\"data\":[{\"property\":\"" << property << "\"}],\"transactionid\":\"" << transactionId << "\"";
    if(!sent)
        ss << ",\"error\":\"method call failed\"";
    ss << "}";

    string replystr = ss.str();
    LOG_MESSAGE( "Reply:" << replystr << endl);

    WebSockets::Write(socket, replystr);
}

void CANGenPlugin::dataReceived(libwebsocket* socket, const char* data, size_t len)
{
    if(!data || len == 0)
        return;

    //TODO: refactor ? copied from websocketsink
    std::unique_ptr<json_object, decltype(&json_object_put)> rootobject(nullptr, &json_object_put);
    std::unique_ptr<json_tokener, decltype(&json_tokener_free)> tokener(json_tokener_new(), &json_tokener_free);
    enum json_tokener_error err;
    do
    {   std::unique_ptr<json_object, decltype(&json_object_put)> tmpobject(json_tokener_parse_ex(tokener.get(), data, len), &json_object_put);
        rootobject.swap(tmpobject);
    } while ((err = json_tokener_get_error(tokener.get())) == json_tokener_continue);
    if (err != json_tokener_success)
    {
        LOG_ERROR("Error: " << json_tokener_error_desc(err) << std::endl);
        return;
    }
    if(!rootobject)
    {
        LOG_ERROR("Failed to parse json: " << data << std::endl);
        return;
    }

    if (tokener->char_offset < len) // XXX shouldn't access internal fields
    {
        // Handle extra characters after parsed object as desired.
        // e.g. issue an error, parse another object from that point, etc...
    }
    // Success, use jobj here.
    json_object *typeobject = json_object_object_get(rootobject.get(),"type");
    json_object *nameobject = json_object_object_get(rootobject.get(),"name");
    json_object *transidobject = json_object_object_get(rootobject.get(),"transactionid");

    string type = string(json_object_get_string(typeobject));
    string name = string(json_object_get_string(nameobject));
    string id;
    if (json_object_get_type(transidobject) == json_type_string)
    {
        id = string(json_object_get_string(transidobject));
    }
    else
    {
        stringstream strstr;
        strstr << json_object_get_int(transidobject);
        id = strstr.str();
    }
    if (type == "method") {

        vector<string> propertyNames;
        list< std::tuple<string, string, string, Zone::Type, string> > propertyData;

        json_object *dataobject = json_object_object_get(rootobject.get(),"data");
        if (json_object_get_type(dataobject) == json_type_array)
        {
            array_list *arraylist = json_object_get_array(dataobject);
            for (int i=0;i<array_list_length(arraylist);i++)
            {
                json_object *arrayobject = (json_object*)array_list_get_idx(arraylist,i);
                if (json_object_get_type(arrayobject) == json_type_object)
                {
                    json_object *interfaceobject = json_object_object_get(arrayobject,"interface");
                    json_object *propobject = json_object_object_get(arrayobject,"property");
                    json_object *valueobject = json_object_object_get(arrayobject,"value");
                    json_object *zoneobject = json_object_object_get(arrayobject,"zone");
                    json_object *sourceobject = json_object_object_get(arrayobject,"source");
                    string interfacestr = string(interfaceobject ? json_object_get_string(interfaceobject) : "vcan0");
                    string keystr = string(propobject ? json_object_get_string(propobject) : "");
                    string valuestr = string(valueobject ? json_object_get_string(valueobject): "");
                    string sourcestr = string(sourceobject ? json_object_get_string(sourceobject): "");
                    Zone::Type z(Zone::None);
                    if(zoneobject){
                        try {
                            z = static_cast<Zone::Type>(boost::lexical_cast<int,std::string>(json_object_get_string(zoneobject)));
                        } catch (...) { }
                    }
                    propertyData.push_back(make_tuple(interfacestr, keystr, valuestr, z, sourcestr));
                }
                else if (json_object_get_type(arrayobject) == json_type_string)
                {
                    string propertyName = string(json_object_get_string(arrayobject));
                    propertyNames.push_back(propertyName);
                }
            }
            //array_list_free(arraylist);
        }
        else
        {
            string path = json_object_get_string(dataobject);
            if (path != "")
            {
                propertyNames.push_back(path);
            }
        }
        if (type == "method")
        {
            if (name == "get")
            {
                if (!propertyNames.empty())
                {
                    //GetProperty is going to be a singleshot sink.
                    getValue(socket,propertyNames.front(),Zone::None,id);
                }
                else if (!propertyData.empty())
                {
                    //GetProperty is going to be a singleshot sink.
                    auto prop = propertyData.front();
                    getValue(socket,std::get<1>(prop),std::get<3>(prop),id);
                }
                else
                {
                    LOG_WARNING(" \"get\" method called with no data! Transaction ID:" << id);
                }
            }
            else if (name == "set")
            {
                if (!propertyNames.empty())
                {
                    //Should not happen
                }
                else if (!propertyData.empty())
                {
                    auto prop = propertyData.begin();
                    for (auto prop = propertyData.begin(); prop != propertyData.end(); ++prop)
                    {
                        LOG_MESSAGE("websocketsinkmanager setting " << std::get<1>(*prop) << " to " << std::get<2>(*prop) << " in zone " << std::get<3>(*prop));
                        setValue(socket,std::get<1>(*prop),std::get<2>(*prop),std::get<3>(*prop),std::get<0>(*prop), id);
                    }
                }
            }
            else if (name == "getSupportedEventTypes")
            {
                //If data.front() dosen't contain a property name, return a list of properties supported.
                //if it does, then return the event types that particular property supports.
                string typessupported = "";
                if (propertyNames.empty())
                {
                    //Send what properties we support
                    PropertyList foo(routingEngine->supported());
                    PropertyList::const_iterator i=foo.cbegin();
                    while (i != foo.cend())
                    {
                        if(i==foo.cbegin())
                            typessupported.append("\"").append((*i)).append("\"");
                        else
                            typessupported.append(",\"").append((*i)).append("\"");
                        ++i;
                    }
                }
                else
                {
                    //Send what events a particular property supports
                    PropertyList foo(routingEngine->supported());
                    if (ListPlusPlus<VehicleProperty::Property>(&foo).contains(propertyNames.front()))
                    {
                        //sinkManager->addSingleShotSink(wsi,data.front(),id);
                        typessupported = "\"get\",\"getSupportedEventTypes\"";
                    }
                }
                stringstream s;
                string s2;
                s << "{\"type\":\"methodReply\",\"name\":\"getSupportedEventTypes\",\"data\":[" << typessupported << "],\"transactionid\":\"" << id << "\"}";
                string replystr = s.str();
                LOG_INFO(" JSON Reply: " << replystr);
                WebSockets::Write(socket, replystr);
            }
            else
            {
                DebugOut(0)<<"Unknown method called."<<endl;
            }
        }
    }
}
