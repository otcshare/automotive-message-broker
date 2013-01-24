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

#include <functional>

struct Source: public GSource {
	AsyncQueueWatcher* queueWatcher;
};

gboolean prepare(GSource* source, gint *timeout)
{
	*timeout = 0;
	Source* s = static_cast<Source*>(source);
	int size = g_async_queue_length (s->queueWatcher->queue);
	return (size > 0);
}

gboolean check(GSource *source)
{
	Source* s = static_cast<Source*>(source);
	int size = g_async_queue_length (s->queueWatcher->queue);
	return (size > 0);
}

gboolean dispatch(GSource* source, GSourceFunc c, gpointer u)
{
	Source* s = static_cast<Source*>(source);
	int size = g_async_queue_length (s->queueWatcher->queue);
	gpointer item = g_async_queue_try_pop (s->queueWatcher->queue);

	if (item == NULL)
	{
		return true;
	}

	if (s->queueWatcher->callback == NULL)
	{
		return false;
	}

	s->queueWatcher->callback(item, s->queueWatcher->userData);
	return true;
}

void finalize(GSource *source)
{

}


AsyncQueueWatcher::AsyncQueueWatcher(GAsyncQueue *q, AsyncQueueWatcherCallback cb, void *data)
{
	using namespace std::placeholders;

	queue = g_async_queue_ref(q);
	userData = data;
	callback = cb;

	auto foo1 = std::bind(prepare, _1, _2);

	GSourceFuncs vtable;
	vtable.prepare = prepare;
	vtable.check = check;
	vtable.dispatch =  dispatch;
	vtable.finalize =  finalize;

	Source* src = static_cast<Source*>(g_source_new(&vtable,sizeof(Source)));
	src->queueWatcher = this;
	g_source_attach(src,NULL);
	g_source_set_can_recurse(src, TRUE);

}
