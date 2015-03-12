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

#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/socket.h>

#include <timestamp.h>
#include "logger.h"
#include "cansocketbcm.h"
#include "timestamp.h"

CANSocketBCM::CANSocketBCM() :
    mSocket(-1)
{
    LOG_TRACE("");
}

bool CANSocketBCM::start(const char* ifName)
{
    LOG_TRACE("");

    if(mSocket >= 0)
        return false;

    if(!createSocket())
    {
        LOG_ERROR("Socket error");
        return false;
    }

    //    can_err_mask_t errorMask = 0xFFFFFFFF;
    //    if(!enableCANErrors(errorMask)) {
    //        LOG_ERROR("Socket error");
    //        return false;
    //    }

    mPoll.fd = mSocket;
    mPoll.events = POLLIN | POLLPRI;

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, ifName);
    if(!locateInterfaceIndex(ifr)) {
        LOG_ERROR("Socket error");
        stop();
        return false;
    }

    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if(!connectSocket(addr)) {
        LOG_ERROR("Socket error");
        stop();
        return false;
    }

    return true;
}

void CANSocketBCM::stop()
{
    LOG_TRACE("");

    if(mSocket >= 0) {
        closeSocket();
        mSocket = -1;
    }
}

bool CANSocketBCM::write(const struct CANFrameInfo &message)
{
    LOG_TRACE("");

    return writeFrameOneTime(message.frame);
}

CANSocket::CANSocketReadSuccess CANSocketBCM::read(struct CANFrameInfo& message, unsigned int timeout)
{
    LOG_TRACE("timeout: " << timeout);

    CANSocket::CANSocketReadSuccess success;
    memset(&message, 0, sizeof(message));

    switch(waitData(timeout)) {
    case -1:
        LOG_ERROR("reading error");
        success = CANSocket::READING_FAILED;
        break;
    case 0:
        success = CANSocket::READING_TIMED_OUT;
        break;
    default:
        success = readMessage(message);
        break;
    }

    return success;
}

bool CANSocketBCM::createSocket()
{
    return ((mSocket = ::socket(PF_CAN, SOCK_DGRAM, CAN_BCM)) >= 0);
}

bool CANSocketBCM::locateInterfaceIndex(struct ifreq& ifr)
{
    return (::ioctl(mSocket, SIOCGIFINDEX, &ifr) == 0);
}

bool CANSocketBCM::connectSocket(struct sockaddr_can& addr)
{
    return (::connect(mSocket, (struct sockaddr*)&addr, sizeof(addr)) == 0);
}

bool CANSocketBCM::closeSocket()
{
    return (::close(mSocket) == 0);
}

int CANSocketBCM::waitData(unsigned int timeout)
{
    return ::poll(&mPoll, 1, timeout);
}

/**
 * BCM header with one message.
 * @note hdr.nframes must always be 0 or 1.
 */
struct __attribute__ ((__packed__)) bcm_msg_one{
    struct bcm_msg_head hdr;
    struct can_frame frames[1];
};

bool CANSocketBCM::writeFrameOneTime(const can_frame& frame)
{
    struct bcm_msg_one bcms;

    // fill in the header
    memset(&bcms.hdr, 0, sizeof(bcms.hdr));
    bcms.hdr.opcode = TX_SEND;
    bcms.hdr.nframes = 1;
    bcms.hdr.can_id = frame.can_id;

    // copy the frame
    memcpy(&bcms.frames[0], &frame, sizeof(frame));

    // and write everything
    ssize_t nbytes = ::write(mSocket, &bcms, sizeof(bcms));
    return nbytes == sizeof(bcms);
}

CANSocket::CANSocketReadSuccess CANSocketBCM::readMessage(CANFrameInfo& message)
{
    struct bcm_msg_one bcms;

    // clear the destination
    memset(&message, 0, sizeof(message));

    // get data from socket
    size_t nbytes = ::recv(mSocket, &bcms, sizeof(bcms), 0);
    if ( nbytes < sizeof(bcms.hdr))
    {
        LOG_ERROR("Socket error");
        return CANSocket::CANSocketReadSuccess::READING_FAILED;
    }
    //TODO: implement better timestamps
    message.timestamp = amb::currentTime();

    switch (bcms.hdr.opcode)
    {
    case RX_CHANGED:
        if (bcms.hdr.nframes >= 1 && nbytes == sizeof(bcms))
        {
            if (bcms.hdr.nframes > 1)
            {
                LOG_WARNING("Dropped " << bcms.hdr.nframes - 1 << " updates from CAN bus.");
            }

            // copy the first frame
            memcpy(&message.frame, &bcms.frames[0], sizeof(bcms.frames[0]));
            message.status = CANFrameInfo::CANMessageStatus::GOOD;
            return CANSocket::CANSocketReadSuccess::READING_SUCCEEDED;
        }
        else
        {
            LOG_ERROR("Unexpected data from the socket"
                      << " " << bcms.hdr.opcode
                      << " " << bcms.hdr.nframes
                      << " " << nbytes);
            return CANSocket::CANSocketReadSuccess::READING_FAILED;
        }
    case RX_TIMEOUT:
        memcpy(&message.frame, &bcms.frames[0], sizeof(bcms.frames[0]));
        message.frame.can_id = bcms.hdr.can_id; //doubtful. Do we need to override this?
        message.status = CANFrameInfo::CANMessageStatus::TIMEOUT;
        return CANSocket::CANSocketReadSuccess::READING_SUCCEEDED;

    case TX_EXPIRED:
        // do nothing
        return CANSocket::CANSocketReadSuccess::READING_TIMED_OUT;

    default:
        LOG_ERROR("Unexpected opcode " << bcms.hdr.opcode);
        return CANSocket::CANSocketReadSuccess::READING_FAILED;
    }
}

/*
 4.2.5 Broadcast Manager receive filter timers

   The timer values ival1 or ival2 may be set to non-zero values at RX_SETUP.
   When the SET_TIMER flag is set the timers are enabled:

   ival1: Send RX_TIMEOUT when a received message is not received again within
    the given time. When START_TIMER is set at RX_SETUP the timeout detection
     is activated directly - even without a former CAN frame reception.

   ival2: Throttle the received message rate down to the value of ival2. This
     is useful to reduce messages for the application when the signal inside the
     CAN frame is stateless as state changes within the ival2 periode may get
     lost.
*/

bool CANSocketBCM::registerCyclicMessageForReceive(int canId, double minCycleTime, double maxCycleTime)
{
    struct bcm_msg_head hdr;

    // fill in the header
    memset(&hdr, 0, sizeof(hdr));
    hdr.opcode = RX_SETUP;
    // set RX_FILTER_ID | RX_CHECK_DLC because we don't differentiate messages by dlc or content yet. Only by id
    // setting RX_ANNOUNCE_RESUME may lead to duplicates in data which should be filtered by amb core.
    //        However, we won't miss any data.
    hdr.flags = RX_FILTER_ID | RX_CHECK_DLC | SETTIMER | STARTTIMER | RX_ANNOUNCE_RESUME;
    hdr.nframes = 0;
    hdr.can_id = canId;
    hdr.ival1 = amb::Timestamp::toTimeval(maxCycleTime);
    hdr.ival2 = amb::Timestamp::toTimeval(minCycleTime);

    // and write
    ssize_t nbytes = ::write(mSocket, &hdr, sizeof(hdr));
    return nbytes == sizeof(hdr);
}

bool CANSocketBCM::unregisterMessageForReceive(int canId)
{
    struct bcm_msg_head hdr;

    // fill in the header
    memset(&hdr, 0, sizeof(hdr));
    hdr.opcode = RX_DELETE;
    hdr.can_id = canId;

    // and write
    ssize_t nbytes = ::write(mSocket, &hdr, sizeof(hdr));
    return nbytes == sizeof(hdr);
}

