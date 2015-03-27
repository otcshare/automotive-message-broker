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

#ifndef CANBUS_H
#define CANBUS_H

/**
 *  @defgroup libcanbus libcanbus static library
 *  \brief SocketCAN based CAN bus library.
 *  @{
 */

#include "canobserver.h"

/**
* \brief Encapsulation of the raw CAN bus.
*
* @class CANBus
*/
class CANBus
{
public:
	/**
	* @param observer Object derived from #CANObserver that will receive CAN bus frames
	*/
	CANBus(CANObserver& observer);
	virtual ~CANBus();

	/**
	* Starts the CAN bus instance on the specified interface
	* @fn start
	* @param name Name of the CAN bus network interface
	* @return True if no error occurs.
	*/
	virtual bool start(const char* name);
	/**
	* Stops the CAN bus instance
	* @fn stop
	*/
	virtual void stop();
	/**
	* Sends standard(11bit) CAN frame over the bus
	* @fn sendStandardFrame
	* @param frame CAN frame to be sent
	* @return True if frame was sent
	*/
	virtual bool sendStandardFrame(const can_frame& frame);
	/**
	* Sends extended(29bit) CAN frame over the bus
	* @fn sendExtendedFrame
	* @param frame CAN frame to be sent
	* @return True if frame was sent
	*/
	virtual bool sendExtendedFrame(const can_frame& frame);

	class Impl;
protected:
	/**
	* CANBus class private implementation
	* @property d
	* @protected
	*/
	Impl* d;
};

#endif // CANBUS_H

/** @} */
