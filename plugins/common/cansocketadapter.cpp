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

#include "cansocketadapter.h"
#include "cansocket.h"
#include "canobserver.h"
#include "cansocketreader.h"
#include "logger.h"

// TODO: handle socket errors

CANSocketAdapter::CANSocketAdapter(CANObserver& observer) :
    CANAdapter(observer),
    mSocket(NULL),
    mReader(NULL)
{
    LOG_TRACE("");
}

CANSocketAdapter::~CANSocketAdapter()
{
    LOG_TRACE("");

    stop();
}

bool CANSocketAdapter::start(const char* ifName)
{
    LOG_TRACE("");

    if(!mSocket || !mReader) {
        init();
    }

    if(mSocket && mReader && mSocket->start(ifName) && mReader->start()) {
		return true;
	}

	stop();
    return false;
}

void CANSocketAdapter::stop()
{
    LOG_TRACE("");

    if(mReader) {
        mReader->stop();
        delete mReader;
        mReader = 0;
    }
    if(mSocket) {
        mSocket->stop();
        delete mSocket;
        mSocket = 0;
    }
}

bool CANSocketAdapter::sendFrame(const can_frame& frame)
{
    LOG_TRACE("");

    if(mSocket) {
		int bytesWritten(0);
		return mSocket->write(frame, bytesWritten);
    }
    return false;
}

void CANSocketAdapter::init()
{
    if(!mSocket)
    	mSocket = new CANSocket();
    if(!mReader)
    	mReader = new CANSocketReader(mObserver, *mSocket);
}

