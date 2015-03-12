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

#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/socket.h>

#include "logger.h"
#include "cansocket.h"
#include "cansocketraw.h"
#include "timestamp.h"

CANSocketRaw::CANSocketRaw() :
    mSocket(-1)
{
    LOG_TRACE("");
}

bool CANSocketRaw::start(const char* ifName)
{
    LOG_TRACE("");

    if(mSocket < 0) {
        if(!createSocket()) {
            LOG_ERROR("Socket error");
        } else {
            can_err_mask_t errorMask = 0xFFFFFFFF;
            if(!enableCANErrors(errorMask)) {
                LOG_ERROR("Socket error");
            } else
                if(!enableTimestamps()) {
                    LOG_ERROR("Socket error");
                } else {
                    mPoll.fd = mSocket;
                    mPoll.events = POLLIN | POLLPRI;
                    struct ifreq ifr;
                    memset(&ifr, 0, sizeof(ifr));
                    strcpy(ifr.ifr_name, ifName);
                    if(!locateInterfaceIndex(ifr)) {
                        LOG_ERROR("Socket error");
                        stop();
                    } else {
                        struct sockaddr_can addr;
                        memset(&addr, 0, sizeof(addr));
                        addr.can_family = AF_CAN;
                        addr.can_ifindex = ifr.ifr_ifindex;
                        if(!bindSocket(addr)) {
                            LOG_ERROR("Socket error");
                            stop();
                        } else {
                            return true;
                        }
                    }
                }
        }
    }
    return false;
}

void CANSocketRaw::stop()
{
    LOG_TRACE("");

    if(mSocket >= 0) {
        closeSocket();
        mSocket = -1;
    }
}

bool CANSocketRaw::write(const struct CANFrameInfo &message)
{
    LOG_TRACE("");

    return writeFrame(message.frame);
}

CANSocket::CANSocketReadSuccess CANSocketRaw::read(struct CANFrameInfo& message, unsigned int timeout)
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
        ssize_t nbytes = (int)readFrame(message.frame, message.timestamp);
        message.status = CANFrameInfo::CANMessageStatus::GOOD;
        success = nbytes > 0 ? CANSocket::READING_SUCCEEDED : CANSocket::READING_FAILED;
    }

    return success;
}

bool CANSocketRaw::createSocket()
{
    return ((mSocket = ::socket(PF_CAN, SOCK_RAW, CAN_RAW)) >= 0);
}

bool CANSocketRaw::enableTimestamps()
{
    const int timestamp = 1;

    return (setsockopt(mSocket, SOL_SOCKET, SO_TIMESTAMP, &timestamp, sizeof(timestamp)) == 0);
}

bool CANSocketRaw::enableCANErrors(can_err_mask_t errorMask)
{
    return (setsockopt(mSocket, SOL_CAN_RAW, CAN_RAW_ERR_FILTER, &errorMask, sizeof(errorMask)) == 0);
}

bool CANSocketRaw::locateInterfaceIndex(struct ifreq& ifr)
{
    return (::ioctl(mSocket, SIOCGIFINDEX, &ifr) == 0);
}

bool CANSocketRaw::bindSocket(struct sockaddr_can& addr)
{
    return (::bind(mSocket, (struct sockaddr*)&addr, sizeof(addr)) == 0);
}

bool CANSocketRaw::closeSocket()
{
    return (::close(mSocket) == 0);
}

int CANSocketRaw::waitData(unsigned int timeout)
{
    return ::poll(&mPoll, 1, timeout);
}

bool CANSocketRaw::writeFrame(const can_frame& frame)
{
    return ::write(mSocket, &frame, sizeof(frame)) == sizeof(frame);
}

ssize_t CANSocketRaw::readFrame(can_frame& frame, double &timestamp)
{
    struct iovec io;
    struct msghdr msgh;
    struct cmsghdr *cmsg;

    // prepare buffers
    memset(&msgh, 0, sizeof(msgh));
    io.iov_base=&frame;
    io.iov_len=sizeof(can_frame);
    msgh.msg_iov=&io;
    msgh.msg_iovlen=1;
    char buffer[1024];
    msgh.msg_control=&buffer;
    msgh.msg_controllen=sizeof(buffer);

    // receive data
    ssize_t nbytes = ::recvmsg(mSocket, &msgh, 0);

    if (nbytes > 0 )
    {
        /* Receive auxiliary data in msgh */
        for (cmsg = CMSG_FIRSTHDR(&msgh); cmsg != NULL;
             cmsg = CMSG_NXTHDR(&msgh, cmsg)) {
            if (cmsg->cmsg_type == SO_TIMESTAMP) {
                struct ::timeval *tv = (struct timeval*) CMSG_DATA(cmsg);

                // convert the timestamp
                 timestamp =  amb::Timestamp::fromTimeval(*tv);

                break;
            }
        }
        if (cmsg == NULL) {
            /* No timestamp is provided by the socket. Use our own. */
            timestamp = amb::Timestamp::instance()->epochTime();
        }
    }

    return nbytes;
}
