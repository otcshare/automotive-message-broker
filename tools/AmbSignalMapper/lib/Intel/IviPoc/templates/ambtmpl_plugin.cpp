/*****************************************************************
Copyright (C) 2014  Intel Corporation

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
 *****************************************************************/

#include <boost/assert.hpp>
#include <glib.h>
#include <cstdarg>

#include <vehicleproperty.h>
#include <listplusplus.h>

#include <logger.h>

#include "ambtmpl_plugin.h"
#include "ambtmpl_cansignals.h"

//using namespace AmbTmplPlugin;

static const char* DEFAULT_CAN_IF_NAME = "vcan0";

// library exported function for plugin loader
extern "C" AbstractSource * create(AbstractRoutingEngine* routingengine, std::map<std::string, std::string> config)
{
#ifndef UNIT_TESTS
    DEBUG_CONF("AmbTmplPlugin",
        CUtil::Logger::file_off|CUtil::Logger::screen_on,
        CUtil::Logger::EInfo, CUtil::Logger::EInfo
    );
#endif

    std::unique_ptr< AmbPlugin<AmbTmplPlugin> > plugin(new AmbPlugin<AmbTmplPlugin>(routingengine, config));
    plugin->init();
    return plugin.release();
}

void AmbTmplPlugin::timerDestroyNotify(gpointer data)
{
    TimerData* timerData = reinterpret_cast<TimerData*>(data);
    if(timerData){
        AmbTmplPlugin* plugin = std::get<0>(*timerData);
        const can_frame& frame = std::get<3>(*timerData);
        plugin->eraseTimer(frame.can_id);
        delete timerData;
    }

}

gboolean AmbTmplPlugin::timeoutCallback(gpointer data)
{
    TimerData* timerData = reinterpret_cast<TimerData*>(data);
    if(!timerData)
        return false;

    CANBus* bus = std::get<1>(*timerData);
    int retries = --std::get<2>(*timerData);
    const can_frame& frame = std::get<3>(*timerData);

    if(retries > 0){
        if(bus)
            bus->sendStandardFrame(frame);
        return true;
    }
    else return false;
}

//----------------------------------------------------------------------------
// AmbTmplPlugin
//----------------------------------------------------------------------------

AmbTmplPlugin::AmbTmplPlugin(AbstractRoutingEngine* re, const map<string, string>& config, AbstractSource& parent) :
      AmbPluginImpl(re, config, parent),
      interface(DEFAULT_CAN_IF_NAME),
      canBus(new CANBus(*static_cast<CANObserver*>(this))),
      announcementIntervalTimer(1000),
      announcementCount(20)
{
    auto it = config.find("interface");
    if (it != config.end() && it->second.length())
        interface = it->second;

    it = config.find("announcementIntervalTimer");
    if (it != config.end() && it->second.length())
        announcementIntervalTimer = atoi(std::string(it->second).c_str());
    if(announcementIntervalTimer < 20)
        announcementIntervalTimer = 20;

    it = config.find("announcementCount");
    if (it != config.end() && it->second.length())
        announcementCount = atoi(std::string(it->second).c_str());
    if(announcementCount < 1)
            announcementIntervalTimer = 1;

    registerMessages();
}

AmbTmplPlugin::~AmbTmplPlugin()
{
    std::list<guint> timerList;

    mutex.lock();
    for(auto it=timers.begin();it!=timers.end();++it)
        timerList.push_back(it->second);
    mutex.unlock();

    for(auto it=timerList.begin();it!=timerList.end();++it)
        g_source_remove(*it);

    canBus->stop();
}

void AmbTmplPlugin::init()
{
    canBus->start(interface.c_str());
}

AsyncPropertyReply *AmbTmplPlugin::setProperty(const AsyncSetPropertyRequest& request )
{
    AsyncPropertyReply* reply = new AsyncPropertyReply(request);
    reply->success = false;
    reply->error = AsyncPropertyReply::InvalidOperation;

    AbstractPropertyType *value = findPropertyType(request.property, request.zoneFilter);
    if (value && request.value) {
        DebugOut(2) << "updating property "<< request.property << " to: " << request.value->toString() << endl;

        std::unique_ptr<GVariant, decltype(&g_variant_unref)> variant(request.value->toVariant(), &g_variant_unref);
        value->fromVariant(variant.get());

        if(sendValue(request.value)) {
            DebugOut(2) << "New value of property "<< request.property << " is: " << value->toString() << endl;
            value->timestamp = amb::currentTime();
            routingEngine->updateProperty(value, uuid());

            reply->success = true;
            reply->error = AsyncPropertyReply::NoError;
        }
    }

    try {
        if(reply->completed)
            reply->completed(reply);
    }
    catch (...) { }

    return reply;
}

int AmbTmplPlugin::supportedOperations() const
{
    return AbstractSource::Get | AbstractSource::Set;
}

void AmbTmplPlugin::onMessage(const can_frame& frame)
{
    auto messageIt = messages.find(frame.can_id);
    if(messageIt == messages.end())
        return;

    CANMessage& message(messageIt->second);
    const std::string guid = uuid();
    AbstractRoutingEngine* re = routingEngine;
    message.onMessage( frame, [&re, &guid](AbstractPropertyType* value){re->updateProperty(value, guid);} );
}

bool AmbTmplPlugin::sendValue(AbstractPropertyType* value)
{
    if(!value)
        return false;

    auto it = propertyToMessage.find(value->name);
    if(it == propertyToMessage.end())
        return false;

    CANMessage* canMessage = it->second;
    can_frame frame;
    canMessage->setupFrame(&frame);
    return sendStandardFrame(frame);
}

// from CANObserver
void AmbTmplPlugin::errorOccured(CANObserver::CANError error)
{
    (void) error;
    LOG_INFO( "AmbTmplPlugin::errorOccured() not implemented "<< std::endl );
}

void AmbTmplPlugin::standardFrameReceived(const can_frame& frame)
{
    LOG_INFO("AmbTmplPlugin::standardFrameReceived()");
    printFrame( frame );

    onMessage(frame);
}

void AmbTmplPlugin::extendedFrameReceived(const can_frame& frame)
{
    LOG_INFO("AmbTmplPlugin::extendedFrameReceived()");
    printFrame(frame);

    onMessage(frame);
}

void AmbTmplPlugin::errorFrameReceived(const can_frame& frame)
{
    LOG_INFO("AmbTmplPlugin::errorFrameReceived()");
    printFrame(frame);
}

void AmbTmplPlugin::remoteTransmissionRequest(const can_frame& frame)
{
    (void) frame;
    LOG_INFO( "AmbTmplPlugin::remoteTransmissionRequest() not implemented "<< std::endl );
}

bool AmbTmplPlugin::sendStandardFrame(const can_frame& frame)
{
    guint oldTimer(0);
    mutex.lock();
    auto it = timers.find(frame.can_id);
    if(it != timers.end()){
        oldTimer = it->second;
        timers.erase(it);
    }
    mutex.unlock();

    if(oldTimer != 0)
        g_source_remove(oldTimer);

    if ( canBus->sendStandardFrame(frame) ) {
        LOG_TRACE( "AmbTmplPlugin::sendStandardFrame() success "<< std::endl );
        scoped_lock<interprocess_recursive_mutex> lock(mutex);
        std::unique_ptr< TimerData > timerData(new TimerData(this, canBus.get(), announcementCount, frame));
        guint timer = g_timeout_add_full(G_PRIORITY_HIGH, announcementIntervalTimer, timeoutCallback, timerData.get(), timerDestroyNotify);
        if(timer){
            timers[frame.can_id] = timer;
            timerData.release();
        }
        return true;
    }
    else {
        LOG_WARNING( "AmbTmplPlugin::sendStandardFrame() failed "<< std::endl );
        return false;
    }
}

bool AmbTmplPlugin::sendExtendedFrame(const can_frame& frame)
{
    if ( canBus->sendExtendedFrame(frame) ) {
        LOG_TRACE( "AmbTmplPlugin::sendExtendedFrame() success "<< std::endl );
        return true;
    }
    else {
        LOG_WARNING( "AmbTmplPlugin::sendExtendedFrame() failed "<< std::endl );
        return false;
    }
}

void AmbTmplPlugin::printFrame(const can_frame& frame) const
{
    LOG_INFO( "AmbTmplPlugin::printFrame can_id: " << std::hex << frame.can_id << std::dec );
    LOG_INFO( "AmbTmplPlugin::printFrame can_dlc: " << int(frame.can_dlc) );

    std::stringstream ss;
    for(int i=0; i<frame.can_dlc; ++i){
        ss << " " << std::hex << (int)(frame.data[i]);
    }
    ss << std::dec;

    LOG_INFO( "AmbTmplPlugin::printFrame can data" << ss.str() );
}

void AmbTmplPlugin::registerMessages()
{
/*GENERATED_CODE*/
}
