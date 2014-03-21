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
