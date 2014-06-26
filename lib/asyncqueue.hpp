/*
	Copyright (C) 2014  Intel Corporation

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

#include <glib.h>

#include <mutex>

namespace amb
{

template <typename T>
class Queue
{
public:
	Queue()
	{

	}
	virtual ~Queue()
	{

	}

	int count()
	{
		std::lock_guard<std::mutex> lock(mutex);

		return mQueue.size();
	}

	T pop()
	{
		std::lock_guard<std::mutex> lock(mutex);

		auto itr = mQueue.begin();

		T item = *itr;

		mQueue.erase(itr);

		return item;
	}

	virtual void append(T item)
	{
		std::lock_guard<std::mutex> lock(mutex);

		mQueue.push_back(item);
	}

protected:
	std::mutex mutex;
	std::vector<T> mQueue;
};

template <typename T>
struct AsyncQueueSource{
	GSource source;
	Queue<T>* queue;
};

template <typename T>
class AsyncQueueWatcher
{
public:
	typedef function<void (Queue<T> *)> AsyncQueueWatcherCallback;
	AsyncQueueWatcher(Queue<T> * queue, AsyncQueueWatcherCallback cb)
		: callback(cb)
	{

		static GSourceFuncs funcs = {prepare, check, dispatch, finalize};
		GSource* source = (GSource *) g_source_new(&funcs, sizeof(AsyncQueueSource<T>));
		//GSource* source = (GSource *) g_source_new(&funcs, sizeof(GSource));

		AsyncQueueSource<T>* watch = (AsyncQueueSource<T>*)source;
		watch->queue = queue;

		g_source_set_priority(source,G_PRIORITY_DEFAULT);
		g_source_set_callback(source, nullptr, this, nullptr);

		g_source_attach(source, nullptr);
		g_source_unref(source);
	}

	AsyncQueueWatcherCallback callback;


protected:
	AsyncQueueWatcher(){}

private:

	static gboolean prepare(GSource *source, gint *timeout)
	{
		AsyncQueueSource<T>* s = (AsyncQueueSource<T>*)source;
		*timeout = -1;

		if (!s)
			return false;

		return s->queue->count() > 0;
	}

	static gboolean check(GSource *source)
	{
		AsyncQueueSource<T>* s = (AsyncQueueSource<T>*)source;

		if (!s)
			return false;

		return s->queue->count() > 0;
	}

	static gboolean dispatch(GSource *source, GSourceFunc callback, gpointer userData)
	{
		AsyncQueueSource<T>* s = (AsyncQueueSource<T>*)source;

		if (!s)
			return false;

		AsyncQueueWatcher* watcher = static_cast<AsyncQueueWatcher*>(userData);

		watcher->callback(s->queue);
		return true;
	}

	static void finalize(GSource* source)
	{

	}
};
}  // namespace amb
