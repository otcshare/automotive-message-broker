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

/* Refactored to an abstract interface. See http://stackoverflow.com/a/825365 */

#ifndef CANBUS_H
#define CANBUS_H

/**
 *  @defgroup libcanbus libcanbus static library
 *  \brief SocketCAN based CAN bus library.
 *  @{
 */

#include "canobserver.h"

/**
* \brief Encapsulation of the raw CAN bus.
*
* @class CANBus
*/
class CANBus
{
public:
    virtual ~CANBus(){} /*LCOV_EXCL_LINE*/

    /**
    * Starts the CAN bus instance on the specified interface
    * @fn start
    * @param name Name of the CAN bus network interface
    * @return True if no error occurs.
    */
    virtual bool start(const char* name) = 0;
    /**
    * Stops the CAN bus instance
    * @fn stop
    */
    virtual void stop() = 0; 
    /**
    * Sends standard(11bit) CAN frame over the bus
    * @fn sendStandardFrame
    * @param frame CAN frame to be sent
    * @return True if frame was sent
    */
    virtual bool sendStandardFrame(const can_frame& frame) = 0;
    /**
    * Sends extended(29bit) CAN frame over the bus
    * @fn sendExtendedFrame
    * @param frame CAN frame to be sent
    * @return True if frame was sent
    */
    virtual bool sendExtendedFrame(const can_frame& frame) = 0;
    /**
    * Registers CAN ID of a cyclic message for receiving
    * @fn registerCyclicMessageForReceive
    * @param canId CAN ID of the message.
    * @param minCycleTime Minimal interval between messages in seconds. Set to 0 if not used.
    * @param maxCycleTime Maximum interval between messages for timeout detection in seconds. Set to 0 if no timeout detection is necessary.
    * @return True if registration succeeds.
    */
    virtual bool registerCyclicMessageForReceive(int canId, double minCycleTime, double maxCycleTime) = 0;
    /**
    * Registers CAN ID of a message for receiving with no timeout. Perfect for sporadic messages.
    * @fn registerMessageForReceive
    * @param canId CAN ID of the message.
    * @return True if registration succeeds.
    */
    virtual bool registerMessageForReceive(int canId)
    {
	return registerCyclicMessageForReceive(canId, 0, 0);
    }
    /**
    * Un-registers CAN ID of a message used of receiving. Valid for cyclic and sporadic messages.
    * @fn unregisterMessageForReceive
    * @param canId CAN ID of the message.
    * @return True if de-registration succeeds.
    */
    virtual bool unregisterMessageForReceive(int canId) = 0;
};

#endif // CANBUS_H

/** @} */
