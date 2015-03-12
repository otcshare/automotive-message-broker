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

#include "canbusimpl.h"
#include "canobserver.h"
#include "canadapter.h"
#include "logger.h"

CANBusImpl::CANBusImpl(CANObserver& observer) :
    mObserver(observer),
    mAdapter(NULL)
{
    LOG_TRACE("");
}

CANBusImpl::~CANBusImpl()
{
    LOG_TRACE("");

    stop();
}

bool CANBusImpl::start(const char* name)
{
    LOG_TRACE("");

    if(!mAdapter) {
        init();
    }
    return mAdapter ? mAdapter->start(name) : false;
}

void CANBusImpl::stop()
{
    LOG_TRACE("");

    if(mAdapter) {
        mAdapter->stop();
        delete mAdapter;
        mAdapter = 0;
    }
}

bool CANBusImpl::sendStandardFrame(const can_frame& frame)
{
    LOG_TRACE("");

    if(mAdapter) {
        struct can_frame frm(frame);
        frm.can_id &= CAN_SFF_MASK;
        return mAdapter->sendFrame(frm);
    }
    return false;
}

bool CANBusImpl::sendExtendedFrame(const can_frame& frame)
{
    LOG_TRACE("");

    if(mAdapter) {
        struct can_frame frm(frame);
        frm.can_id &= CAN_EFF_MASK;
        frm.can_id |= CAN_EFF_FLAG;
        return mAdapter->sendFrame(frm);
    }
    return false;
}

void CANBusImpl::init()
{
    mAdapter = CANAdapter::createCANAdapter(mObserver);
}

bool CANBusImpl::registerCyclicMessageForReceive(int canId, double minCycleTime, double maxCycleTime)
{
    if(mAdapter) {
        return mAdapter->registerCyclicMessageForReceive(canId, minCycleTime, maxCycleTime);
    }
    return false;
}

bool CANBusImpl::unregisterMessageForReceive(int canId)
{
    if(mAdapter) {
     return mAdapter->unregisterMessageForReceive(canId);
    }
    return false;
}

