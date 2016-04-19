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
*	File: Mutex.cpp
*
*	Revision;
*		- first revision
*
******************************************************************/

#include "mutex.h"

using namespace CUtil;

////////////////////////////////////////////////
//	Mutex
////////////////////////////////////////////////

Mutex::Mutex()
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init( &attr );
    pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );
    pthread_mutex_init( &mutexID, &attr );
    pthread_mutexattr_destroy( &attr );
}

Mutex::~Mutex()
{
    pthread_mutex_destroy( &mutexID );
}

void Mutex::lock()
{
    pthread_mutex_lock( &mutexID );
}

void Mutex::unlock()
{
    pthread_mutex_unlock( &mutexID );
}
