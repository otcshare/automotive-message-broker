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

#ifndef _CUTIL_THREAD_H_
#define _CUTIL_THREAD_H_

#include <pthread.h>
#include <sys/prctl.h>
#include "logger.h"

/** \addtogroup libamb-plugins-common
 *  @{
 */

namespace CUtil {

/**
 * \brief Encapsulation of the POSIX thread.
 *
 * Supports starting, stopping, restarting and thread status checking.
 * @class Thread
 */
class Thread
{
	/**
	* Condition variable used to signal runnable status changes
	* @property cond
	* @private
	*/
	pthread_cond_t cond;
	/**
	* Mutex used to lock runnable status changes
	* @property mutex
	* @private
	*/
	pthread_mutex_t mutex;
	/**
	* Instance of the encapsulated posix thread
	* @property thread
	* @private
	*/
	pthread_t thread;
	/**
	* Runnable status of the thread. True means that thread is running
	* @property runnableFlag
	* @private
	*/
	bool runnableFlag;

private:

	/**
	* Sets new runnable status of the thread. Holds mutex and signals condition variable
	* @fn setRunnableFlag
	* @private
	* @param[in] flag New status of the thread to be set
	* @return Actual runnable status after operation
	*/
	bool setRunnableFlag(bool flag);

	/**
	* Sleeps current thread for specified time in miliseconds. Wait can be canceled if thread is in stopped/stopping state.
	* @fn wait
	* @private
	* @param miliseconds Sleep time in miliseconds.
	* @return True if wait wasn't canceled, false otherwise.
	*/
	bool wait( long miliseconds );

public:

	/**
	* Method to be executed in this thread. Has to be overwritten.
	* @fn run
	* @public
	*/
	virtual void run() = 0;

protected:

	/**
	* Gets runnable status of current thread. Sleeps calling thread for a specified time.
	* @protected
	* @fn isRunnable
	* @param miliseconds Time in miliseconds. 0 means no sleep.
	* @return True if thread should still run.
	*
	* \b Example
	* @code
	*
	*       void MyCustomThread::run()
	*       {
	*           while(isRunnable(1000)) { // execute some op once per second. Exits thread if stop() was called.
	*           ...
	*           }
	*       }
	* @endcode
	*/
	bool isRunnable(long miliseconds = 0);

public:

	Thread();
	virtual ~Thread();

	/**
	* Starts the thread
	* @fn start
	* @return True if thread was started
	* @public
	*/
	virtual bool start();

    /**
     * Sets the priority of the thread for FIFO scheduling.
     * @fn set_priority
     * @param priority Integer ranging from 1 (lowest) to 99 (highest).
     * @return True if the operation was successful.
     * @public
     */
    bool setPriority(int priority);

	/**
	* Stops the thread
	* @fn stop
	* @public
	*/
	virtual void stop();

	/**
	* Restarts the thread.
	* @fn restart
	* @return True if thread was started again
	* @public
	*/
	bool restart()
	{
		stop();
		return start();
	}
};

}

#endif

/** @} */

