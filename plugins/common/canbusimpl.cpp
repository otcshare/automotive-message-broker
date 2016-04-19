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

#include "canbusimpl.h"
#include "canobserver.h"
#include "canadapter.h"
#include "logger.h"

//----------------------------------------------------------------------------
// CANBusImpl
//----------------------------------------------------------------------------

CANBus::Impl::Impl(CANObserver& observer) :
    mObserver(observer),
    mAdapter(NULL)
{
    LOG_TRACE("");
}

CANBus::Impl::~Impl()
{
    LOG_TRACE("");

    stop();
}

bool CANBus::Impl::start(const char* name)
{
    LOG_TRACE("");

    if(!mAdapter) {
        init();
    }
    return mAdapter ? mAdapter->start(name) : false;
}

void CANBus::Impl::stop()
{
    LOG_TRACE("");

    if(mAdapter) {
        mAdapter->stop();
        delete mAdapter;
        mAdapter = 0;
    }
}

bool CANBus::Impl::sendStandardFrame(const can_frame& frame)
{
    LOG_TRACE("");

    if(mAdapter) {
        struct can_frame frm(frame);
        frm.can_id &= CAN_SFF_MASK;
        return mAdapter->sendFrame(frm);
    }
    return false;
}

bool CANBus::Impl::sendExtendedFrame(const can_frame& frame)
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

void CANBus::Impl::init()
{
    mAdapter = CANAdapter::createCANAdapter(mObserver);
}

//----------------------------------------------------------------------------
// CANBus
//----------------------------------------------------------------------------

CANBus::CANBus(CANObserver& observer) :
    d(new CANBus::Impl(observer))
{
    LOG_TRACE("");
}

CANBus::~CANBus()
{
    LOG_TRACE("");

    if(d) {
    	delete d;
        d = 0;
    }
}

bool CANBus::start(const char* name)
{
    LOG_TRACE("");

    return d ? d->start(name) : false;
}

void CANBus::stop()
{
    LOG_TRACE("");

    if(d)
    	d->stop();
}

bool CANBus::sendStandardFrame(const can_frame& frame)
{
    LOG_TRACE("");

    return d ? d->sendStandardFrame(frame) : false;
}

bool CANBus::sendExtendedFrame(const can_frame& frame)
{
    LOG_TRACE("");

    return d ? d->sendExtendedFrame(frame) : false;
}
