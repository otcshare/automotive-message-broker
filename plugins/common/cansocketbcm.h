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

#ifndef CANSOCKETBCM_H
#define CANSOCKETBCM_H

/**
 *  \addtogroup libcanbus
 *  @{
 */

#include <net/if.h>
#include <sys/poll.h>
#include <string>
#include <stdlib.h>
#include <linux/can/bcm.h>
#include <linux/can/raw.h>

#include "cansocket.h"

/**
* \brief CAN Socket wrapper.
* @class CANSocket
*/
class CANSocketBCM : public CANSocket
{
public:
    CANSocketBCM();
    virtual ~CANSocketBCM(){} /*LCOV_EXCL_LINE*/

    /**
    * Opens and initialize CAN socket
    * @fn start
    * @param ifName Name of the CAN bus network interface.
    * @return True if no error occurs.
    */
    virtual bool start(const char* ifName);
    /**
    * Closes socket
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
     * Unregisters CAN ID for receiving
     * @fn unregisterMessageForReceive
     * @param canId CAN ID of the message.
     * @return True if de-registration succeeds.
     */
    virtual bool unregisterMessageForReceive(int canId);

private:
    /**
    * @internal
    */
    bool createSocket();
    bool locateInterfaceIndex(struct ifreq& ifr);
    bool connectSocket(struct sockaddr_can& addr);
    bool closeSocket();
    int  waitData(unsigned int timeout);
    bool writeFrameOneTime(const can_frame& frame);
    CANSocket::CANSocketReadSuccess readMessage(CANFrameInfo& message);

private:
    /**
    * Socket file descriptor.
    * @property mSocket
    * @private
    */
    int mSocket;
    /**
    * Data structure describing a polling request.
    * @property mPoll
    * @private
    */
    struct pollfd mPoll;
};

#endif // CANSOCKETBCM_H

/** @} */
