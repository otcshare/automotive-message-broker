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

#ifndef AMBTMPL_PLUGIN_H_
#define AMBTMPL_PLUGIN_H_

#include <map>
#include <memory>
#include <tgmath.h>
#include <limits>
#include <byteswap.h>
#include <boost/interprocess/sync/interprocess_recursive_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include <canbus.h>
#include <canobserver.h>

#include <ambplugin.h>
#include <ambpluginimpl.h>
#include "ambtmpl_cansignal.h"

using namespace boost::interprocess;

// forward declaration
class CANSignal;
class CANMessage;

class AmbTmplPlugin : public AmbPluginImpl, public CANObserver {

public:
    AmbTmplPlugin(AbstractRoutingEngine* re, const std::map<std::string, std::string>& config, AbstractSource& parent);
    virtual ~AmbTmplPlugin(); // has to be virtual because of unit tests

    // from AbstractSink
public:

    /*! uuid() is a unique identifier of the plugin
      * @return a guid-style unique identifier
      */
    const std::string uuid() const { return "/*GENERATED_CODE*/"; }

    /*!
     * \brief setProperty is called when a sink requests to set a value for a given property.
     * This is only called if the source supports the Set Operation.
     * \param request the requested property to set.
     * \return returns a pointer to the new value for the property.
     */
    AsyncPropertyReply *setProperty(const AsyncSetPropertyRequest& request );

    /*!
     * \brief supportedOperations
     * \return returns the supported operations.
     */
    int supportedOperations() const;


    // from CANObserver
public:
    /*!
     * Called when error occurred on the bus.
     * \fn errorOccured
     * \param error \link CANObserver#CANError Bus error code \endlink
     */
    virtual void errorOccured(CANObserver::CANError error);/* socket error */
    /*!
     * Called when standard frame was is received from the bus.
     * \fn standardFrameReceived
     * \param frame Received frame
     */
    virtual void standardFrameReceived(const can_frame& frame);/* SFF was present */
    /*!
     * Called when extended frame was is received from the bus.
     * \fn extendedFrameReceived
     * \param frame Received frame
     */
    virtual void extendedFrameReceived(const can_frame& frame);/* EFF was present */
    /*!
     * Called when error frame was received from the bus.
     * \fn errorFrameReceived
     * \param frame Error frame
     */
    virtual void errorFrameReceived(const can_frame& frame);/* error frame */
    /*!
     * Called when remote transmission frame was received from the bus.
     * \fn remoteTransmissionRequest
     * \param frame RTR frame
     */
    virtual void remoteTransmissionRequest(const can_frame& frame);/* remote transmission request (SFF/EFF is still present)*/

    /**
    * Sends standard(11bit) CAN frame over the bus
    * \fn sendStandardFrame
    * \param frame CAN frame to be sent
    * \return True if frame was sent
    */
    bool sendStandardFrame(const can_frame& frame);

    /**
    * Sends extended(29bit) CAN frame over the bus
    * \fn sendExtendedFrame
    * \param frame CAN frame to be sent
    * \return True if frame was sent
    */
    bool sendExtendedFrame(const can_frame& frame);

    /*!
     * Second phase of the plugin initialization.
     * \fn init
     */
    virtual void init();

    void eraseTimer(const canid_t& id)
    {
        scoped_lock<interprocess_recursive_mutex> lock(mutex);
        auto it = timers.find(id);
        if(it != timers.end()){
            timers.erase(it);
        }
    }

protected:

    void registerMessage(const canid_t& canId, const __u8& canDlc)
    {
        LOG_MESSAGE("registered message: " << canId);
    }

    template<typename Signal, typename... Rest>
    void registerMessage(const canid_t& canId, const __u8& canDlc, Signal* canSignal, Rest... rest)
    {
        static_assert(std::is_base_of<CANSignal, Signal>::value, "CANSignal has to be a base of Signal");

        if(!canSignal)
            return;

        std::shared_ptr<AbstractPropertyType> prop(AmbPluginImpl::addPropertySupport(Zone::None, canSignal->factory()));
        if(prop) {
            canSignal->setAmbProperty(prop);
            auto messageIt = messages.find(canId);
            if(messageIt == messages.end()){
                messageIt = messages.insert(make_pair(canId, CANMessage(canId, canDlc))).first;
            }
            auto& message = messageIt->second;
            message.addSignal(prop->name, std::shared_ptr<CANSignal>(canSignal));
            propertyToMessage[prop->name] = &message;
        }

        registerMessage(canId, canDlc, rest...);
    }

private:

    void printFrame(const can_frame& frame) const;
    void onMessage(const can_frame& frame);
    bool sendValue(AbstractPropertyType* value);
    void registerMessages();

    static void timerDestroyNotify(gpointer data);
    static gboolean timeoutCallback(gpointer data);
//
// data:
//
    std::map< canid_t, CANMessage > messages;
    std::map< VehicleProperty::Property, CANMessage* > propertyToMessage;

    std::string interface;

    /**
    * CANBus class instance
    * \property canBus
    * \private
    */
    std::unique_ptr<CANBus> canBus;

    typedef std::tuple<AmbTmplPlugin*, CANBus*, int, const can_frame> TimerData;
    std::map< canid_t, guint > timers;
    interprocess_recursive_mutex mutex;
    uint announcementIntervalTimer;
    uint announcementCount;
};

#endif /* AMBTMPL_PLUGIN_H_ */
