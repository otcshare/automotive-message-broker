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
#ifndef ASYNCQUEUEWATCHER_H_
#define ASYNCQUEUEWATCHER_H_

#include <glibmm/main.h>

typedef void (*AsyncQueueWatcherCallback) (gpointer, void* data);

class AsyncQueueWatcher: public Glib::Source
{
public:
	AsyncQueueWatcher(GAsyncQueue* q, AsyncQueueWatcherCallback cb, void* data);

	bool prepare(int& timeout);
	bool check();
	bool dispatch(sigc::slot_base* slot);

private:
	int id;
	GAsyncQueue* queue;
	AsyncQueueWatcherCallback callback;
	void* userData;
};


#endif
