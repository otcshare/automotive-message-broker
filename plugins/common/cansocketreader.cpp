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

#include "logger.h"
#include "cansocket.h"
#include "cansocketreader.h"

CANSocketReader::CANSocketReader(CANObserver& observer, CANSocket& socket) :
    mObserver(observer),
    mSocket(socket)
{
    LOG_TRACE("");
}

CANSocketReader::~CANSocketReader()
{
    LOG_TRACE("");

    stop();
}

bool CANSocketReader::start()
{
    LOG_TRACE("");

    bool res = CUtil::Thread::start();

    // try to set higher priority
    if (res) res = setPriority(4);
    return res;
}

void CANSocketReader::stop()
{
    LOG_TRACE("");

    CUtil::Thread::stop();
    return;
}

void CANSocketReader::run()
{
    LOG_TRACE("");

    while(isRunnable()) {

        CANFrameInfo message;
        CANSocket::CANSocketReadSuccess success = mSocket.read(message);

        switch(success)
        {
        case CANSocket::READING_SUCCEEDED:
            dispatchMessage(message);
            break;

        case CANSocket::READING_TIMED_OUT:
            // read again
            break;

        case CANSocket::READING_FAILED:
        default:
            LOG_ERROR("reading failed");
            mObserver.errorOccured(CANObserver::GENERAL_ERROR);
            break;
        }
    }
}

void CANSocketReader::dispatchMessage(const CANFrameInfo &message)
{
    struct can_frame frame = message.frame;

    switch (message.status)
    {
    case CANFrameInfo::CANMessageStatus::GOOD:
        if(frame.can_id & CAN_ERR_FLAG) {
            frame.can_id &= (CAN_ERR_FLAG|CAN_ERR_MASK);
            mObserver.errorFrameReceived(frame);
        }
        else if( frame.can_id & CAN_RTR_FLAG){
            if(!( frame.can_id & CAN_EFF_FLAG)){
                frame.can_id &= CAN_SFF_MASK;
            }
            else{
                frame.can_id &= (~CAN_RTR_FLAG);
            }
            mObserver.remoteTransmissionRequest(frame);
        }
        else if(frame.can_id & CAN_EFF_FLAG){
            frame.can_id &= CAN_EFF_MASK;
            mObserver.extendedFrameReceived(frame);
        }
        else{
            frame.can_id &= CAN_SFF_MASK;
            mObserver.standardFrameReceived(frame);
        }
        break;

    case CANFrameInfo::CANMessageStatus::TIMEOUT:
        if(frame.can_id & CAN_EFF_FLAG)
            frame.can_id &= CAN_EFF_MASK;
        else
            frame.can_id &= CAN_SFF_MASK;

        mObserver.timeoutDetected(frame);
        break;

    default:
        LOG_ERROR("Unexpected CAN message status " << message.status);
        mObserver.errorOccured(CANObserver::GENERAL_ERROR);
        break;
    }
}

