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

#ifndef CANSOCKETADAPTER_H
#define CANSOCKETADAPTER_H

/**
 *  \addtogroup libcanbus
 *  @{
 */

#include "canadapter.h"

class CANObserver;
class CANSocket;
class CANSocketReader;

/**
* \brief Socket CAN device implementation based on <a href="http://developer.berlios.de/projects/socketcan">BerliOS Socket CAN</a> API.
*
* @class CANSocketAdapter
*/
class CANSocketAdapter : public CANAdapter
{
public:
    /**
    * @param observer @link CANObserver Observer @endlink that will receives CAN bus frames
    */
    CANSocketAdapter(CANObserver& observer);
    virtual ~CANSocketAdapter();

    /**
    * Creates connection the specified network interface and starts listening on it.
    * @fn start
    * @param ifName Name of the CAN bus network interface
    * @return True if no error occurs.
    */
    virtual bool start(const char* ifName);
    /**
    * Closes socket connection and exits listening thread.
    * @fn stop
    */
    virtual void stop();
    /**
    * Sends CAN frame over the socket CAN interface
    * @fn sendFrame
    * @param frame CAN frame to be sent
    * @return True if frame was sent
    */
    virtual bool sendFrame(const can_frame& frame);
    /**
    * Registers CAN ID of a cyclic message for receiving
    * @fn registerCyclicMessageForReceive
    * @param canId CAN ID of the message.
    * @param minCycleTime Minimal interval between messages in seconds. Set to 0 if not used.
    * @param maxCycleTime Maximum interval between messages for timeout detection in seconds. Set to 0 if no timeout detection is necessary.
    * @return True if registration succeeds.
    */
    virtual bool registerCyclicMessageForReceive(int canId, double minCycleTime, double maxCycleTime);
    /**
    * Un-registers CAN ID of a message used of receiving. Valid for cyclic and sporadic messages.
    * @fn unregisterMessageForReceive
    * @param canId CAN ID of the message.
    * @return True if de-registration succeeds.
    */
    virtual bool unregisterMessageForReceive(int canId);

protected:
    /**
    * Socket initialization and starts reading thread
    * @fn init
    * @protected
    */
    virtual void init();

private:
    /**
    * @link CANSocket CAN Socket wrapper @endlink instance reference
    * @property mSocket
    * @protected
    */
    CANSocket* mSocket;
    /**
    * @link CANSocketReader CANSocket reader @endlink instance reference
    * @property mReader
    * @protected
    */
    CANSocketReader* mReader;
};

#endif // CANSOCKETADAPTER_H

/** @} */
