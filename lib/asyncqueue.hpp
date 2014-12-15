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

#include <abstractpropertytype.h>
#include "listplusplus.h"

#include <mutex>
#include <condition_variable>
#include <unordered_set>

namespace amb
{

template <typename T, class Pred = std::equal_to<T> >
class Queue
{
public:
	Queue(bool blocking = false)
		:mBlocking(blocking)
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
		std::unique_lock<std::mutex> lock(mutex);

		if(mBlocking)
		{
			while(!mQueue.size())
			{
				cond.wait(lock);
			}
		}

		auto itr = mQueue.begin();

		T item = *itr;

		mQueue.erase(itr);

		return item;
	}

	virtual void append(T item)
	{
		std::lock_guard<std::mutex> lock(mutex);

		mQueue.insert(item);

		if(mBlocking)
		{
			cond.notify_one();
		}
	}

	void remove(T item)
	{
		std::lock_guard<std::mutex> lock(mutex);
		removeOne(&mQueue, item);
	}

protected:
	bool mBlocking;
	std::mutex mutex;
	std::condition_variable cond;
	std::unordered_set<T, std::hash<T>, Pred> mQueue;
};

template <typename T, class Pred = std::equal_to<T> >
struct AsyncQueueSource{
	GSource source;
	Queue<T, Pred>* queue;
	int minQueueSize;
};

template <typename T, class Pred = std::equal_to<T> >
class AsyncQueueWatcher
{
public:
	typedef function<void (Queue<T, Pred> *)> AsyncQueueWatcherCallback;
	AsyncQueueWatcher(Queue<T, Pred> * queue, AsyncQueueWatcherCallback cb, int queueSize = 0, AbstractPropertyType::Priority priority = AbstractPropertyType::Normal)
		: callback(cb), mMaxQueueSize(queueSize)
	{

		static GSourceFuncs funcs = {prepare, check, dispatch, finalize};
		GSource* source = (GSource *) g_source_new(&funcs, sizeof(AsyncQueueSource<T, Pred>));

		AsyncQueueSource<T, Pred>* watch = (AsyncQueueSource<T, Pred>*)source;
		watch->queue = queue;
		watch->minQueueSize = queueSize;

		gint p = G_PRIORITY_DEFAULT;

		if(priority == AbstractPropertyType::Normal)
			p = G_PRIORITY_DEFAULT;
		else if(priority == AbstractPropertyType::High)
			p = G_PRIORITY_HIGH;
		else if(priority == AbstractPropertyType::Low)
			p = G_PRIORITY_LOW;

		g_source_set_priority(source, p);
		g_source_set_callback(source, nullptr, this, nullptr);

		g_source_attach(source, nullptr);
		g_source_unref(source);
	}

	AsyncQueueWatcherCallback callback;


protected:
	AsyncQueueWatcher(){}

	int mMaxQueueSize;

private:

	static gboolean prepare(GSource *source, gint *timeout)
	{
		AsyncQueueSource<T, Pred>* s = (AsyncQueueSource<T, Pred>*)source;
		*timeout = -1;

		if (!s)
			return false;

		return s->queue->count() > s->minQueueSize;
	}

	static gboolean check(GSource *source)
	{
		AsyncQueueSource<T, Pred>* s = (AsyncQueueSource<T, Pred>*)source;

		if (!s)
			return false;

		return s->queue->count() > s->minQueueSize;
	}

	static gboolean dispatch(GSource *source, GSourceFunc callback, gpointer userData)
	{
		AsyncQueueSource<T, Pred>* s = (AsyncQueueSource<T, Pred>*)source;

		if (!s)
			return false;

		AsyncQueueWatcher<T, Pred>* watcher = static_cast<AsyncQueueWatcher<T, Pred>*>(userData);

		watcher->callback(s->queue);
		return true;
	}

	static void finalize(GSource* source)
	{

	}
};
}  // namespace amb
