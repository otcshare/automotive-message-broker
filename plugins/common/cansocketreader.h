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

#ifndef CANSOCKETREADER_H
#define CANSOCKETREADER_H

/**
 *  \addtogroup libcanbus
 *  @{
 */

#include <sys/socket.h>

#include "thread.h"
#include "canobserver.h"

class CANSocket;
/**
* \brief Asynchronous socket CAN reader.
*
* @class CANSocketReader
*/
class CANSocketReader : public CUtil::Thread
{
public:
    /**
    * @param observer @link CANObserver Observer @endlink that will receives CAN bus frames.
    * @param socket @link CANSocket CAN Socket wrapper @endlink
    */
    CANSocketReader(CANObserver& observer, CANSocket& socket);
    virtual ~CANSocketReader();

    /**
    * Starts a new thread for listening to CAN bus.
    * @fn start
    * @return True if no error occurs.
    */

    virtual bool start();
    /**
    * Stops listening thread.
    * @fn stop
    */

    virtual void stop();

private:
    /**
    * Listening thread's main function.
    * @fn run
    */
    virtual void run();

private:
    /**
    * #CANObserver instance reference
    * @property mObserver
    * @protected
    */
    CANObserver& mObserver;
    /**
    * @link CANSocket CAN Socket wrapper @endlink instance reference
    * @property mSocket
    * @protected
    */
    CANSocket& mSocket;
};

#endif // CANSOCKETREADER_H

/** @} */
