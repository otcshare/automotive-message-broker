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

#ifndef CANSOCKET_H
#define CANSOCKET_H

/**
 *  \addtogroup libcanbus
 *  @{
 */

#include <linux/can.h>

#include "canbus.h"

/**
* \brief Wrapper around different implementations of SocketCAN.
* @class CANSocket
*/
class CANSocket
{
public:

    /**
    * CAN bus socket error.
    * @enum CANSocketReadSuccess
    * @public
    */
    enum CANSocketReadSuccess {
        READING_FAILED = -1,
        READING_TIMED_OUT,
        READING_SUCCEEDED,
    };

public:
    CANSocket();
    virtual ~CANSocket();

    /**
    * Opens and initializes CAN socket
    * @fn start
    * @param ifName Name of the CAN bus network interface.
    * @return True if no error occurs.
    */
    virtual bool start(const char* ifName) = 0;
    /**
    * Closes the socket
    * @fn stop
    */
    virtual void stop();
    /**
    * Writes CAN frame using the socket
    * @fn write
    * @param message CAN frame with additional information
    * @param bytesWritten Number of written bytes.
    * @return True if no error occurs.
    */
    virtual bool write(const struct CANFrameInfo &message);
    /**
    * Try to read CAN frame
    * @fn read
    * @param message Buffer for CAN frame with additional information
    * @param timeout Timeout for reading in [ms].
    * @return Reading operation status code.
    */
    virtual CANSocket::CANSocketReadSuccess read(struct CANFrameInfo& message, unsigned int timeout = 1000);
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
     * Un-registers CAN ID of a message used of receiving. Valid for cyclic and sporadic messages.
     * @fn unregisterMessageForReceive
     * @param canId CAN ID of the message.
     * @return True if de-registration succeeds.
     */
    virtual bool unregisterMessageForReceive(int canId);
};

#endif // CANSOCKET_H

/** @} */
