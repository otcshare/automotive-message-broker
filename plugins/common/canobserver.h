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

