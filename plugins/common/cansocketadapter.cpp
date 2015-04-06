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

#include "cansocketadapter.h"
#include "cansocket.h"
#include "canobserver.h"
#include "cansocketreader.h"
#include "logger.h"
#include "cansocketbcm.h"
#include "cansocketraw.h"

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
		CANFrameInfo message(frame);
		return mSocket->write(message);
	}
	return false;
}

void CANSocketAdapter::init()
{
	if(!mSocket)
		mSocket = new CANSocketBCM();
	if(!mReader)
		mReader = new CANSocketReader(mObserver, *mSocket);
}

bool CANSocketAdapter::registerCyclicMessageForReceive(int canId, double minCycleTime, double maxCycleTime)
{
	if(mSocket)
		return mSocket->registerCyclicMessageForReceive(canId, minCycleTime, maxCycleTime);
	else
		return false;
}

bool CANSocketAdapter::unregisterMessageForReceive(int canId)
{
	if(mSocket) 
		return mSocket->unregisterMessageForReceive(canId);
	else
		return false;
}
