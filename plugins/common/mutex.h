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

#ifndef _CUTIL_MUTEX_H_
#define _CUTIL_MUTEX_H_

#include <pthread.h>

/**
 * @defgroup libivipoccommon ivipoc-common static library.
 * \brief Project common C++ utilities.
 *
 * Contains Logger, Thread and Mutex utility classes.
 *
 * @{
 */

/** 
 * \brief Project common C++ utilities namespace
 * @namespace CUtil
 */

namespace CUtil {

/**
* \brief Encapsulation of the recursive posix mutex.
*
* @class Mutex
*/

class Mutex
{
    /**
    * Encapsulated posix mutex object
    * @property mutexID
    * @private
    */
    pthread_mutex_t mutexID;

public:

    Mutex();
    ~Mutex();

    /**
    * Locks mutex object
    * @fn lock
    * @public
    */
    void lock();
    /**
    * Unlocks mutex object
    * @fn unlock
    * @public
    */
    void unlock();
};

}

#endif

/** @} */
