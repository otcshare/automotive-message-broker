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

#ifndef _CANSIMPLUGINIMPL_H_
#define _CANSIMPLUGINIMPL_H_

#include <map>
#include <memory>
#include <tgmath.h>

#include <canbus.h>
#include <canobserver.h>

#include "ambpluginimpl.h"

/*!
 *  \defgroup cansimplugin cansimplugin shared library
 *  \brief CANSimPlugin listens on configured CAN interfaces(e.g. vcan0, vcan1, ...) for CAN frames from CANGenPlugin and translates them into AMB property changes.
 *
 *  CANSimPlugin is a source plug-in for Automotive message broker(AMB). \n
 *  For the AMB library API please visit <a href="https://github.com/otcshare/automotive-message-broker">Automotive message broker web page</a>.\n
 *
 *  To load this plugin at AMB starup, insert following rows into AMB configuration file:
 *  \code
 *      {
 *          "name" : "CANSimPlugin",
 *          "path":"/usr/lib/automotive-message-broker/cansimplugin.so",
 *          "interfaces" : ["vcan0", "vcan1"]
 *      }
 *  \endcode
 *
 * \note CANSimPlugin has to be the last source plug-in listed in AMB configuration file. Otherwise it can accidentally unregister or try to simulate some AMB properties supported from other sources.
 *
 *  @{
 */

/*!
 * \brief CAN frames listener plug-in for the AMB CAN Simulator.
 *
 *  Listens on AMB notification for changes in supported properties using supportedChanged() callback. \n
 *  On startup it registers property called 'MappingTable' and all properties needed for simulation in html5UI.Dashboard and html5UI.HVAC applications. \n
 *  Immediately after it registers own properties it will get first notification supportedChanged().\n
 *  On every notification from supportedChanged() CANSimPlugin goes through all supported properties in AMB, exclude those properties for which there are known sources in the AMB,\n
 *  assigns to each single property and zone some unique CAN Id(builds 'mapping table') and this 'mapping table' stores as JSON string and updates it as a 'MappingTable' property in AMB.\n
 *  When 'MappingTable' is built and ready it starts to listen on all required CAN interfaces in AMB configuration file.\n
 *  On any received CAN frame it goes through the mappingTable, finds property name and zone based on received CAN Id, reads new value from CAN frame and updates it in the AMB.
 *
 * \see \ref libcanbus
 *
 * \class CANSimPlugin
 */
class CANSimPlugin : public AmbPluginImpl, public CANObserver {

public:
    /*!
     * \param re AbstractRoutingEngine
     * \param config Map of the configuration string values loaded on startup from AMB configuration file
     * \param parent AmbPlugin instance
     */
	CANSimPlugin(AbstractRoutingEngine* re, const std::map<std::string, std::string>& config, AbstractSource& parent);
    virtual ~CANSimPlugin(); // has to be virtual because of unit tests

    // from AbstractSink
public:

    /*! uuid() is a unique identifier
      * \return a guid-style unique identifier
      */
    const std::string uuid() const { return "3f43e231-11ec-4782-9b5a-3dbcc5221eeb"; }

    /*! Called when the supported properties changes
     * \param supportedProperties the new list of supported properties.
     */
    void supportedChanged(const PropertyList& supportedProperties);

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
     * \param error @link CANObserver#CANError Bus error code @endlink
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

private:

    /*!
     * \brief Prints received CAN frame
     * \param frame Received CAN frame.
     * \internal
     * \private
     */
    void printFrame(const can_frame& frame) const;

    /*!
     * \brief Creates string in JSON format which contains all supported AMB properties that can be used for simulation.
     * \param supported All supported AMB properties.
     * \see Automotive message broker's \b AsyncRangePropertyReply
     * \private
     */
   void createMappingTable(const PropertyList& supported);

//
// data:
//

   /*!
    * Map of the can_id and corresponding AMB properties in tuple<property_source_id, property_name, zone>
    * \property mappingTable
    * \private
    */
    std::map< canid_t, std::tuple<std::string, VehicleProperty::Property, Zone::Type> > mappingTable;

    /**
    * Map of the CAN interfaces used in simulator. Contains libcanbus#CANBus class instances.
    * \property interfaces
    * \private
    */
    std::map<std::string, std::shared_ptr<CANBus> > interfaces;
};
#endif // _CANSIMPLUGINIMPL_H_

/** @} */
