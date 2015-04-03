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

/******************************************************************
*
*	CUtil for C++
*
*	File: Thread.cpp
*
*	Revision;
*
*	07/07/13
*		- first revision
******************************************************************/

#include <errno.h>
#include <stdio.h>

#include "mutex.h"
#include "thread.h"
#include <stdlib.h>

////////////////////////////////////////////////
// Thread Func
////////////////////////////////////////////////

static CUtil::Mutex gMutex;
static int gActiveThreadCount(0);

static void *PosixThreadProc(void *param)
{
	gMutex.lock();
	++gActiveThreadCount;
	gMutex.unlock();

	CUtil::Thread *thread = (CUtil::Thread *)param;
	thread->run();

	gMutex.lock();
	--gActiveThreadCount;
	LOG_INFO("PosixThreadProc() - active threads: " << gActiveThreadCount);
	gMutex.unlock();
	return 0;
}

namespace CUtil{
////////////////////////////////////////////////
// Thread
////////////////////////////////////////////////

Thread::Thread() :
	thread(0),
	runnableFlag(false)
{
	pthread_cond_init( &cond, NULL );
	pthread_mutex_init( &mutex, NULL );
}

bool Thread::start()
{
	pthread_mutex_lock(&mutex);
	if (runnableFlag) {// already running
		pthread_mutex_unlock(&mutex);
		return false;
	}
	// try to run
	if (pthread_create(&thread, NULL/*&thread_attr*/, PosixThreadProc, this) != 0) {
		//pthread_attr_destroy(&thread_attr);
		pthread_mutex_unlock(&mutex);
		return false;
	}
	//pthread_attr_destroy(&thread_attr);
	runnableFlag = true;
	pthread_mutex_unlock(&mutex);
	return true;
}


bool Thread::setPriority(int priority)
{
	pthread_mutex_lock(&mutex);
	if (!runnableFlag) {// not running yet or terminated already
		pthread_mutex_unlock(&mutex);
		return false;
	}

	priority = priority < 1  ? 1  : priority;
	priority = priority < 99 ? priority : 99;

	// set priority
	struct sched_param pr;
	pr.__sched_priority = priority;
	if (pthread_setschedparam(thread, SCHED_FIFO, &pr) < 0)
	{
		pthread_mutex_unlock(&mutex);
		return false;
	}

	pthread_mutex_unlock(&mutex);
	return true;
}

Thread::~Thread()
{
	stop();
	pthread_cond_destroy( &cond );
	pthread_mutex_destroy( &mutex );
	thread = 0;
}

void Thread::stop()
{
	if (setRunnableFlag(false) == true) {
		if( thread != 0 ){
			if (thread == pthread_self()){
				int s = pthread_detach(thread);
				((void)s);// prevent compiler warning in RELEASE build
				LOG_MESSAGE("Thread::stop() - thread " << std::hex << int(thread) << std::dec << " detached, returned value was " << s);
			}
			else{
				int s = pthread_join(thread, NULL);
				if (s != 0){
					LOG_ERROR("Thread::stop() - Joined with thread " << std::hex << int(thread) << std::dec << ", returned value was " << s);
				}
				else{
					LOG_MESSAGE("Thread::stop() - Joined with thread " << std::hex << int(thread) << std::dec << ", returned value was " << s);
				}
			}
			thread = 0;
		}
	}
	return;
}

bool Thread::setRunnableFlag(bool flag)
{
	pthread_mutex_lock(&mutex);
	bool retval(runnableFlag);
	runnableFlag = flag;

	if(!runnableFlag)
		pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
	return retval;
}

bool Thread::isRunnable(long miliseconds)
{
	bool runnable(false);

	pthread_mutex_lock(&mutex);
	if (miliseconds != 0){
		wait(miliseconds);
	}
	runnable = runnableFlag;

	pthread_mutex_unlock(&mutex);
	return runnable;
}

bool Thread::wait( long miliseconds )
{
	struct timespec abstime;
	clock_gettime(CLOCK_REALTIME, &abstime);
	abstime.tv_sec += ( miliseconds / 1000 );
	miliseconds %= 1000;
	abstime.tv_nsec += ( miliseconds * 1000000L ); // in nanoseconds
	if ( abstime.tv_nsec > 1000000000L /* > 1s */ ){
		abstime.tv_sec += 1; // +1s
		abstime.tv_nsec -= 1000000000L; // -1s
	}

	int status = pthread_cond_timedwait( &cond, &mutex, &abstime );
	return ( status == ETIMEDOUT );
}

}
