/*****************************************************************
 * INTEL CONFIDENTIAL
 * Copyright 2011 - 2013 Intel Corporation All Rights Reserved.
 * 
 * The source code contained or described herein and all documents related to the
 * source code("Material") are owned by Intel Corporation or its suppliers or
 * licensors.Title to the Material remains with Intel Corporation or its
 * suppliers and licensors.The Material may contain trade secrets and proprietary
 * and confidential information of Intel Corporation and its suppliers and
 * licensors, and is protected by worldwide copyright and trade secret laws and
 * treaty provisions.No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or disclosed
 * in any way without Intels prior express written permission.
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise.Any license under such intellectual property rights must be
 * express and approved by Intel in writing. 
 * 
 * Unless otherwise agreed by Intel in writing, you may not remove or alter this
 * notice or any other notice embedded in Materials by Intel or Intels suppliers
 * or licensors in any way.
 *****************************************************************/

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
