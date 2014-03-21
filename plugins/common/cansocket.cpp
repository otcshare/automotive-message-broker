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


CANSocket::CANSocket() :
    mSocket(-1)
{
    LOG_TRACE("");
}

CANSocket::~CANSocket()
{
    LOG_TRACE("");

    stop();
}

bool CANSocket::start(const char* ifName)
{
    LOG_TRACE("");

    if(mSocket < 0) {
        if(!createSocket()) {
            LOG_ERROR("Socket error");
        } else {
            can_err_mask_t errorMask = 0xFFFFFFFF;
            if(!enableCANErrors(errorMask)) {
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

void CANSocket::stop()
{
    LOG_TRACE("");

    if(mSocket >= 0) {
        closeSocket();
        mSocket = -1;
    }
}

bool CANSocket::write(const struct can_frame &frame, int &bytesWritten)
{
    LOG_TRACE("");

    bytesWritten = (int)writeFrame(frame);
    return bytesWritten == sizeof(struct can_frame);
}

CANSocket::CANSocketReadSuccess CANSocket::read(
        struct can_frame& frame, int &bytesRead, unsigned int timeout)
{
    LOG_TRACE("timeout: " << timeout);

    CANSocket::CANSocketReadSuccess success;

    switch(waitData(timeout)) {
    case -1:
        LOG_ERROR("reading error");
        success = CANSocket::READING_FAILED;
        break;
    case 0:
        bytesRead = 0;
        success = CANSocket::READING_TIMED_OUT;
        break;
    default:
        bytesRead = (int)readFrame(frame);
        success = bytesRead >= 0 ?CANSocket::READING_SUCCEEDED : CANSocket::READING_FAILED;
    }
    return success;
}

bool CANSocket::createSocket()
{
    return ((mSocket = ::socket(PF_CAN, SOCK_RAW, CAN_RAW)) >= 0);
}

bool CANSocket::enableCANErrors(can_err_mask_t errorMask)
{
    return (setsockopt(mSocket, SOL_CAN_RAW, CAN_RAW_ERR_FILTER, &errorMask, sizeof(errorMask)) == 0);
}

bool CANSocket::locateInterfaceIndex(struct ifreq& ifr)
{
    return (::ioctl(mSocket, SIOCGIFINDEX, &ifr) == 0);
}

bool CANSocket::bindSocket(struct sockaddr_can& addr)
{
    return (::bind(mSocket, (struct sockaddr*)&addr, sizeof(addr)) == 0);
}

bool CANSocket::closeSocket()
{
    return (::close(mSocket) == 0);
}

int CANSocket::waitData(unsigned int timeout)
{
    return ::poll(&mPoll, 1, timeout);
}

ssize_t CANSocket::writeFrame(const can_frame& frame)
{
    return ::write(mSocket, &frame, sizeof(struct can_frame));
}

ssize_t CANSocket::readFrame(can_frame& frame)
{
    return ::recv(mSocket, &frame, sizeof(struct can_frame), 0);
}
