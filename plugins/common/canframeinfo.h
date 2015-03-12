/*
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

#ifndef CANFRAMEINFO_H
#define CANFRAMEINFO_H

/**
 *  \addtogroup libcanbus
 *  @{
 */

#include <stdlib.h>
#include <linux/can.h>

#include "timestamp.h"

/**
 * CAN frame with additional information
 */
struct CANFrameInfo
{
    CANFrameInfo(const can_frame &frame)
    {
        this->status = CANFrameInfo::CANMessageStatus::GOOD;
        this->frame = frame;
        this->timestamp = amb::currentTime();
    }

    CANFrameInfo() { }

    enum CANMessageStatus {
        TIMEOUT = -2,
        EMPTY = 0,
        GOOD = 1,
    };

    /**
     * The actual frame written or read from socket
     */
    struct can_frame frame;

    /**
     * Status of the message.
     */
    CANFrameInfo::CANMessageStatus status;

    /**
     * Timestamp of sending or receiving action
     */
    double timestamp;
};

#endif // CANFRAMEINFO_H

/** @} */

