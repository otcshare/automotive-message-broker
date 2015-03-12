/*
Copyright (C) 2012 Intel Corporation
Copyright (C) 2015 Cogent Embedded Inc.
Copyright (C) 2015 Renesas Electronics Corporation

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

#ifndef CANBUSIMPL_H
#define CANBUSIMPL_H

/**
 *  \addtogroup libcanbus
 *  @{
 */

#include "canbus.h"

class CANObserver;
class CANAdapter;

/**
* \brief CANBus class implementation
*
* @class CANBus::Impl
*/

class CANBusImpl : public CANBus
{
public:
    /**
    * @param observer \link #CANObserver Observer \endlink that will receive CAN bus frames
    */
    CANBusImpl(CANObserver& observer);
    virtual ~CANBusImpl();

    /**
    * Starts the CAN bus instance on the specified interface
    * @fn start
    * @param name Name of the CAN bus network interface
    * @return True if no error occurs.
    */
    bool start(const char*name);
    /**
    * Stops the CAN bus instance
    * @fn stop
    */
    void stop();
    /**
    * Sends standard(11bit) CAN frame over the bus
    * @fn sendStandardFrame
    * @param frame CAN frame to be sent
    * @return True if frame was sent
    */
    bool sendStandardFrame(const can_frame& frame);
    /**
    * Sends extended(29bit) CAN frame over the bus
    * @fn sendExtendedFrame
    * @param frame CAN frame to be sent
    * @return True if frame was sent
    */
    bool sendExtendedFrame(const can_frame& frame);
    /**
     * Registers CAN ID of a cyclic message for receiving
     * @fn registerCyclicMessageForReceive
     * @param canId CAN ID of the message.
     * @param minCycleTime Minimal interval between messages in seconds. Set to 0 if not used.
     * @param maxCycleTime Maximum interval between messages for timeout detection in seconds. Set to 0 if no timeout detection is necessary.
     * @return True if registration succeeds.
     */
    virtual bool registerCyclicMessageForReceive(int canId, double minCycleTime, double maxCycleTime);
    /**
     * Unregisters CAN ID for receiving
     * @fn unregisterMessageForReceive
     * @param canId CAN ID of the message.
     * @return True if de-registration succeeds.
     */
    virtual bool unregisterMessageForReceive(int canId);

protected:
    /**
    * CAN bus initialization
    * @fn init
    */
    virtual void init();

private:
    /**
    * #CANObserver instance reference
    * @property mObserver
    * @protected
    */
    CANObserver& mObserver;
    /**
    * Pointer to #CANAdapter -  CAN hardware object representation
    * @property mAdapter
    * @protected
    */
    CANAdapter* mAdapter;
};

#endif // CANBUSIMPL_H

/** @} */
