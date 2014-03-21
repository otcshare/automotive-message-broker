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

#ifndef _CANGENPLUGINIMPL_H_
#define _CANGENPLUGINIMPL_H_

#include <map>
#include <memory>
#include <tgmath.h>
#include <libwebsockets.h>

#include <canbus.h>
#include <canobserver.h>

#include <ambpluginimpl.h>
#include "websockets.h"

/*!
 *  \defgroup cangenplugin cangenplugin shared library.
 *
 *  \brief CANGenPlugin generates CAN frames for CANSimPlugin based on WebSocket Simulator API requests.
 *
 *  CANGenPlugin is a source plug-in for Automotive message broker(AMB). \n
 *  For the AMB library API please visit <a href="https://github.com/otcshare/automotive-message-broker">Automotive message broker web page</a>.\n
 *
 *  To load this plugin at AMB starup, insert following rows into AMB configuration file:
 *  \code
 *      {
 *          "name" : "CANGenPlugin",
 *          "path":"/usr/lib/automotive-message-broker/cangenplugin.so"
 *      }
 *  \endcode
 *
 *  It supports following Simulator API commands:
 *      \li Simulator.get() - Get property request - JSON in the form:
 *              \code {"type":"method","name":"get","transactionid":"862bb93d-a302-9a58-baa9-d90265ac843c","data":[{"property":"VehicleSpeed","zone":"0"}]} \endcode
 *          CANGenPlugin replies to this command with actual value, timestamp and sequence number of the requested property with the JSON in the form:\n
 *              \code {"type":"methodReply","name":"get","data":{"property":"VehicleSpeed","zone":"0","value":"35","timestamp":"1388656508.34255","sequence": "2"},"transactionid":"862bb93d-a302-9a58-baa9-d90265ac843c"} \endcode
 *      \li Simulator.set() - Set property request(simulation of the property new value) - JSON in the form:\n
 *               \code {"type":"method","name":"set","transactionid":"d5935c94-7b05-fd67-56bc-31c320185035","data":[{"interface":"vcan0","property":"VehicleSpeed","value":"33","zone":"0"}]} \endcode
 *          Based on property name and zone CANGenPlugin finds corresponding CAN Id of the requested property, builds CAN frame and sends it via specified CAN interface in the host OS.\n
 *          I replies back to the Simulator with the operation error code on failure or without error code on success:\n
 *              \code {"type":"methodReply","name":"set","data":[{"property":"VehicleSpeed"}],"transactionid":"d5935c94-7b05-fd67-56bc-31c320185035","error":"method call failed"} \endcode
 *              \code {"type":"methodReply","name":"set","data":[{"property":"VehicleSpeed"}],"transactionid":"d5935c94-7b05-fd67-56bc-31c320185035"}\endcode
 *
 *  @{
 */

/*!
 * \brief CAN frames generator plug-in for the AMB CAN Simulator.
 *
 *  On startup it subscribe and reads 'MappingTable' property from CANSimPlugin, parses it and build
 *  own mapping table(AMB property to CAN Id map) of all properties from CANSimPlugin that can be simulated.\n
 *  In parallel to building mapping table of the AMB properties and their corresponding CAN Ids it starts listening for javascrip Simulator API commands on WebSocket port \p \b 23001. \n
 *
 * \see \ref libcanbus
 * \class CANGenPlugin
 */

static const char* CANSimPluginUUID = "3f43e231-11ec-4782-9b5a-3dbcc5221eeb";

class CANGenPlugin: public AmbPluginImpl, public CANObserver, public WebSocketsObserver {

public:

    /*!
     * \param re AbstractRoutingEngine
     * \param config Map of the configuration string values loaded on startup from AMB configuration file
     * \param parent AmbPlugin instance
     */
	CANGenPlugin(AbstractRoutingEngine* re, const std::map<std::string, std::string>& config, AbstractSource &parent);
    virtual ~CANGenPlugin(); // has to be virtual because of unit tests

    // from AbstractSink
public:

    /*! uuid() is a unique identifier of the plugin
     * @return a guid-style unique identifier
     */
    const std::string uuid() const { return "becbbef9-6cc8-4b9e-8cd7-2fbe37b9b52a"; }

    /*! propertyChanged is called when a subscribed to property changes.
      * @see AbstractRoutingEngine::subscribeToPropertyChanges()
      * \param value value of the property that changed. this is a temporary pointer that will be destroyed.
      * Do not destroy it.  If you need to store the value use value.anyValue(), value.value<T>() or
      * value->copy() to copy.
      */
    void propertyChanged(AbstractPropertyType* value);

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

    /*!
     * Second phase of the plugin initialization.
     * \fn init
     */
    virtual void init();

    // from WebSocketsObserver

    /*!
     * Called when data received from libwebsockets
     * \fn dataReceived
     * \param socket libwebsocket* to be used to send any reply.
     * \param data Received data pointer.
     * \param len Length of the data.
     * \return None
     */
    void dataReceived(libwebsocket* socket, const char* data, size_t len);

//
// Internal methods:
//
private:

    /*!
     * \brief Prints received CAN frame
     * \param frame Received CAN frame.
     * \internal
     * \private
     */
    void printFrame(const can_frame& frame) const;

    /*!
     * Parses 'MappingTable' property from CANSimPlugin.
     * Result is stored in internal MappingTable class(AMB property and Zone to CAN Id map) which contains all properties that can be simulated.
     * \param json Content of the 'MappingTable' property in JSON format.
     */
    void parseMappingTable(const std::string& json);

    /*!
     * \brief Simulator.get request handler function.
     * Builds and sends reply with the property value, timestamp and sequence number in JSON format.
     * \param socket libwebsocket handle to be used to send reply.
     * \param property Name of the property.
     * \param zone Property's zone.
     * \param uuid Request's transaction id.
     * \private
     */
    void getValue(libwebsocket* socket, const std::string& property, int zone, const std::string& uuid);

    /*!
     * \brief Simulator.set request handler function.
     * Formats property's value as a AMB's AbstractPropertyValue and passes it to sendValue. Reply to the Simulator with reply string in JSON format.
     * \param socket libwebsocket handle to be used to send reply.
     * \param property Name of the property.
     * \param value Property's new value to be simulated.
     * \param zone Property's zone.
     * \param interface CAN interface to be used to send CAN frame.
     * \param transactionId Request's transaction id.
     * \private
     */
    void setValue(libwebsocket* socket, const std::string& property, const std::string& value, int zone, const std::string& interface, const std::string& transactionId);

    /*!
     * \brief Build and sends CAN frame to CANSimPlugin.
     * Finds CAN Id using mappingTable for requested property name and zone, builds CAN frame with the property's new value and tries to send it via requested CAN interface.
     * \param interface CAN interface to be used to send CAN frame.
     * \param value AMB's AbstractPropertyValue which encapsulates property name, zone and value.
     * \return true if CAN frame was successfully sent, otherwise false.
     * \private
     */
    bool sendValue(const std::string& interface, AbstractPropertyType* value);

    /*!
     * Internal helper class
     * AMB property and property's zone to CAN Id map
     * \class MappingTable
     * \private
     * \internal
     *
     */
    class MappingTable{
    public:
        MappingTable()
        {
        }

        MappingTable(const MappingTable& other) = delete;
        MappingTable& operator=(const MappingTable& other) = delete;
        MappingTable(MappingTable&& other) = default;
        MappingTable& operator=(MappingTable&& other) = default;

        void addProperty(const std::string& source, json_object* signal)
        {
            json_object* canIdObj = json_object_object_get(signal, "can_id");
            json_object* nameObj = json_object_object_get(signal, "name");
            if(!canIdObj || !nameObj) // mandatory
                return;
            Zone::Type zone(Zone::None);
            json_object* zoneObj = json_object_object_get(signal, "zone");
            if(zoneObj)
                zone = json_object_get_int(zoneObj);

            auto& zp = mapping[source];
            auto& prop = zp[Zone::Type(zone)];
            std::string name(json_object_get_string(nameObj));
            int can_id = json_object_get_int(canIdObj);
            prop[name] = can_id; // update an existing value
        }

        int getCanId(const std::string& source, const Zone::Type& zone, const VehicleProperty::Property& name) const
        {
            //return mapping[source][zone][name]; // caution! this will insert if not found. I don't want it.
            auto sourceIt = mapping.find(source);
            if(sourceIt == mapping.end())
                return 0;
            auto zoneIt = sourceIt->second.find(zone);
            if(zoneIt == sourceIt->second.end())
                return 0;
            auto propIt = zoneIt->second.find(name);
            if(propIt == zoneIt->second.end())
                return 0;
            else
                return propIt->second;
        }

        void clear()
        {
            mapping.clear();
        }

    private:
        typedef std::map< Zone::Type, std::map<VehicleProperty::Property, canid_t> > ZonedProperty;
        std::map<std::string, ZonedProperty> mapping;
    };

//
// data:
//

    /*!
     * AMB property and property's zone to CAN Id map
     * \private
     */
    MappingTable mappingTable;

    /*!
     * Opened CAN interfaces used to send CAN frames
     * \private
     */
    std::map<std::string, std::shared_ptr<CANBus> > interfaces;

    /*!
     * Encapsulated libwebsocket library
     * \private
     */
    std::unique_ptr<WebSockets> ws;

    /*!
     * Mutex to protect mappingTable container during property 'MappingTable' parsing on change notification
     * \private
     */
    interprocess_recursive_mutex mutex;
};

#endif // _CANGENPLUGINIMPL_H_

/** @} */
