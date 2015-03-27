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

#ifndef CANOBSERVER_H
#define CANOBSERVER_H

/**
 *  \addtogroup libcanbus
 *  @{
 */

#include <linux/can.h>

/**
* \brief Interface. Receives notifications about the CAN bus traffic and errors.
*
* CANBus user has to derive from this interface.
* @class CANObserver
*/

class CANObserver
{
public:
	/**
	* General CAN bus error. Currently only GENERAL_ERROR is specified.
	* @enum CANError
	* @public
	*/
	enum CANError {
		GENERAL_ERROR = 0
	};

	virtual ~CANObserver(){} /*LCOV_EXCL_LINE*/
	/**
	* Called when error occurred on the bus.
	* @fn errorOccured
	* @param error Bus error code
	*/
	virtual void errorOccured(CANObserver::CANError error) = 0;             /* socket error */
	/**
	* Called when standard frame was is received from the bus.
	* @fn standardFrameReceived
	* @param frame Received frame
	*/
	virtual void standardFrameReceived(const can_frame& frame) = 0;       /* SFF was present */
	/**
	* Called when extended frame was is received from the bus.
	* @fn extendedFrameReceived
	* @param frame Received frame
	*/
	virtual void extendedFrameReceived(const can_frame& frame) = 0;       /* EFF was present */
	/**
	* Called when error frame was received from the bus.
	* @fn errorFrameReceived
	* @param frame Error frame
	*/
	virtual void errorFrameReceived(const can_frame& frame) = 0;          /* error frame */
	/**
	* Called when remote transmission frame was received from the bus.
	* @fn remoteTransmissionRequest
	* @param frame RTR frame
	*/
	virtual void remoteTransmissionRequest(const can_frame& frame) = 0;   /* remote transmission request (SFF/EFF is still present)*/

};

#endif // CANOBSERVER_H

/** @} */

