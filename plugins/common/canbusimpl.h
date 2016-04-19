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

#ifndef CANBUSIMPL_H
#define CANBUSIMPL_H

/**
 *  \addtogroup libcanbus
 *  @{
 */

#include "canbus.h"

class CANObserver;
class CANAdapter;

/**
* \brief CANBus class implementation
*
* @class CANBus::Impl
*/

class CANBus::Impl
{
public:
    /**
    * @param observer \link #CANObserver Observer \endlink that will receives CAN bus frames
    */
    Impl(CANObserver& observer);
    virtual ~Impl();

    /**
    * Starts the CAN bus instance on the specified interface
    * @fn start
    * @param name Name of the CAN bus network interface
    * @return True if no error occurs.
    */
    bool start(const char*name);
    /**
    * Stops the CAN bus instance
    * @fn stop
    */
    void stop();
    /**
    * Sends standard(11bit) CAN frame over the bus
    * @fn sendStandardFrame
    * @param frame CAN frame to be sent
    * @return True if frame was sent
    */
    bool sendStandardFrame(const can_frame& frame);
    /**
    * Sends extended(29bit) CAN frame over the bus
    * @fn sendExtendedFrame
    * @param frame CAN frame to be sent
    * @return True if frame was sent
    */
    bool sendExtendedFrame(const can_frame& frame);

protected:
    /**
    * CAN bus initialization
    * @fn init
    */
    virtual void init();

private:
    /**
    * #CANObserver instance reference
    * @property mObserver
    * @protected
    */
    CANObserver& mObserver;
    /**
    * Pointer to #CANAdapter -  CAN hardware object representation
    * @property mAdapter
    * @protected
    */
    CANAdapter* mAdapter;
};

#endif // CANBUSIMPL_H

/** @} */
