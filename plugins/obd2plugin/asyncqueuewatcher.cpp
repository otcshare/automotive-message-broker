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

#include "asyncqueuewatcher.h"

AsyncQueueWatcher::AsyncQueueWatcher(GAsyncQueue *q, AsyncQueueWatcherCallback cb, void *data)
	:Glib::Source()
{
	queue = g_async_queue_ref(q);
	userData = data;
	callback = cb;
	set_priority(G_PRIORITY_DEFAULT);
	//set_can_recurse(true);

}

bool AsyncQueueWatcher::prepare(int &timeout)
{
	timeout = -1;
	int size = g_async_queue_length (queue);
	return (size > 0);
}

bool AsyncQueueWatcher::check()
{
	return (g_async_queue_length (queue) > 0);
}

bool AsyncQueueWatcher::dispatch(sigc::slot_base *)
{
	gpointer item = g_async_queue_try_pop (queue);

	if (item == NULL)
	{
		return true;
	}

	if (callback == NULL)
	{
		return false;
	}

	callback(item, userData);
	return true;
}
