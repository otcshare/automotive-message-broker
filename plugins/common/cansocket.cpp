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

CANSocket::CANSocket()
{
    // default implementation doesn't do anything
    LOG_TRACE("");
}

CANSocket::~CANSocket()
{
    // default implementation doesn't do anything
    LOG_TRACE("");

    stop();
}

void CANSocket::stop()
{
    // default implementation doesn't do anything
}

bool CANSocket::write(const struct CANFrameInfo &message)
{
    // default implementation doesn't do anything
    LOG_TRACE("");

    return false;
}


CANSocket::CANSocketReadSuccess CANSocket::read(struct CANFrameInfo& message, unsigned int timeout)
{
    // default implementation doesn't do anything
    LOG_TRACE("");

    return CANSocketReadSuccess::READING_FAILED;
}

bool CANSocket::registerCyclicMessageForReceive(int canId, double minCycleTime, double maxCycleTime)
{
    // default implementation doesn't do anything
    LOG_TRACE("");

    return false;
}

bool CANSocket::unregisterMessageForReceive(int canId)
{
    // default implementation doesn't do anything
    LOG_TRACE("");

    return false;
}
