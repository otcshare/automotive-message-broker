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

#ifndef CANSOCKETRAW_H
#define CANSOCKETRAW_H

/**
 *  \addtogroup libcanbus
 *  @{
 */

#include <net/if.h>
#include <sys/poll.h>
#include <string>
#include <linux/can/raw.h>

#include "cansocket.h"

/**
* \brief CAN Socket wrapper.
* @class CANSocket
*/
class CANSocketRaw : public CANSocket
{
public:
    CANSocketRaw();

    /**
    * Opens and initializes CAN socket
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

private:
    /**
    * @internal
    */
    bool createSocket();
    bool enableCANErrors(can_err_mask_t errorMask);
    bool enableTimestamps();
    bool locateInterfaceIndex(struct ifreq& ifr);
    bool bindSocket(struct sockaddr_can &addr);
    bool closeSocket();
    int waitData(unsigned int timeout);
    bool writeFrame(const struct can_frame &frame);
    ssize_t readFrame(can_frame& frame, double &timestamp);

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

#endif // CANSOCKETRAW_H

/** @} */
