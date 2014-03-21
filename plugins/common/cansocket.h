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

#include <net/if.h>
#include <sys/poll.h>
#include <string>
#include <linux/can/raw.h>

/**
* \brief CAN Socket wrapper.
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
        READING_SUCCEEDED
    };

public:
    CANSocket();
    virtual ~CANSocket();

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
    * @param frame CAN frame buffer
    * @param bytesWritten Number of written bytes.
    * @return True if no error occurs.
    */
    virtual bool write(const struct can_frame &frame, int &bytesWritten);

    /**
    * Try to read CAN frame
    * @fn read
    * @param frame CAN frame buffer
    * @param bytesRead Number of read bytes.
    * @param timeout Timeout for reading.
    * @return Reading operation status code.
    */
    virtual CANSocket::CANSocketReadSuccess read( struct can_frame& frame, int &bytesRead, unsigned int timeout = 1000);

private:
    /**
    * @internal
    */
    virtual bool createSocket();
    virtual bool enableCANErrors(can_err_mask_t errorMask);
    virtual bool locateInterfaceIndex(struct ifreq& ifr);
    virtual bool bindSocket(struct sockaddr_can &addr);
    virtual bool closeSocket();
    virtual int waitData(unsigned int timeout);
    virtual ssize_t writeFrame(const struct can_frame &frame);
    virtual ssize_t readFrame(struct can_frame& frame);

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

#endif // CANSOCKET_H

/** @} */
