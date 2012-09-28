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

struct QueueWatch{
	GAsyncQueue* queue;
	GSource source;
};

static gboolean prepare (GSource *source, gint *timeout)
{
	QueueWatch *watch = (QueueWatch *)source;
	*timeout = -1;
	return (g_async_queue_length (watch->queue) > 0);
}

static gboolean check (GSource *source)
{
	QueueWatch *watch = (QueueWatch *)source;
	return (g_async_queue_length (watch->queue) > 0);
}

static gboolean dispatch (GSource *source, GSourceFunc callback, gpointer data)
{
	QueueWatch *watch = (QueueWatch *)source;
	AsyncQueueWatcherCallback cb = (AsyncQueueWatcherCallback)callback;

	gpointer item = g_async_queue_try_pop (watch->queue);

	if (item == NULL)
	{
		return true;
	}

	if (cb == NULL)
	{
		return false;
	}

	cb(item, data);
	return true;
}

static void finalize (GSource *source)
{
	QueueWatch *watch = (QueueWatch *)source;

	if (watch->queue) {
		g_async_queue_unref(watch->queue);
		watch->queue = NULL;
	}
}

static GSourceFuncs funcs = {
	prepare,
	check,
	dispatch,
	finalize
};


AsyncQueueWatcher::AsyncQueueWatcher(GAsyncQueue *queue, AsyncQueueWatcherCallback callback, void *data)
{
	GSource *src = g_source_new(&funcs, sizeof(QueueWatch));

	QueueWatch* watch = (QueueWatch*)src;
	watch->queue = g_async_queue_ref(queue);

	g_source_set_callback(src, (GSourceFunc)callback, data, NULL);

	id = g_source_attach(src,NULL);

	g_source_unref(src);
}
