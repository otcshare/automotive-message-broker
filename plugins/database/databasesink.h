/*
	Copyright (C) 2012  Intel Corporation

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


#ifndef DATABASESINK_H
#define DATABASESINK_H

#include "abstractsink.h"
#include "abstractsource.h"
#include "basedb.hpp"
#include "listplusplus.h"

#include <glib.h>

#include <functional>
#include <unordered_map>

const std::string DatabaseLogging = "DatabaseLogging";
const std::string DatabasePlayback = "DatabasePlayback";
const std::string DatabaseFile = "DatabaseFile";

template <typename T>
class Queue
{
public:
	Queue()
	{
		g_mutex_init(&mutex);
		g_cond_init(&cond);
	}
	virtual ~Queue()
	{

	}

	int count()
	{
		g_mutex_lock(&mutex);
		int ret = mQueue.count();
		g_mutex_unlock(&mutex);

		return ret;
	}

	T pop()
	{
		g_mutex_lock(&mutex);

		while(!mQueue.size())
		{
			g_cond_wait(&cond, &mutex);
		}

		auto itr = mQueue.begin();

		T item = *itr;

		mQueue.erase(itr);

		g_mutex_unlock(&mutex);

		return item;
	}

	virtual void append(T item)
	{
		g_mutex_lock(&mutex);

		g_cond_signal(&cond);

		mQueue.push_back(item);

		g_mutex_unlock(&mutex);
	}

protected:
	GMutex mutex;
	GCond cond;
	std::vector<T> mQueue;
};

template <typename T>
class UniqueQueue
{
public:
	UniqueQueue()
	{
		g_mutex_init(&mutex);
		g_cond_init(&cond);
	}
	virtual ~UniqueQueue()
	{

	}

	int count()
	{
		g_mutex_lock(&mutex);
		int ret = mQueue.count();
		g_mutex_unlock(&mutex);

		return ret;
	}

	T pop()
	{
		g_mutex_lock(&mutex);

		while(!mQueue.size())
		{
			g_cond_wait(&cond, &mutex);
		}

		auto itr = mQueue.begin();

		T item = (*itr);

		mQueue.erase(itr);

		g_mutex_unlock(&mutex);

		return item;
	}

	void append(T item)
	{
		g_mutex_lock(&mutex);

		g_cond_signal(&cond);

		if(contains(mQueue, item))
		{
			/// remove old one.  We only want the freshest of values
			mQueue.erase(std::find(mQueue.begin(), mQueue.end(), item));
		}
		mQueue.push_back(item);

		g_mutex_unlock(&mutex);
	}

private:
	GMutex mutex;
	GCond cond;
	std::vector<T> mQueue;
};

class DBObject {
public:
	DBObject(): zone(0), time(0), sequence(0), quit(false) {}
	std::string key;
	std::string value;
	std::string source;
	int32_t zone;
	double time;
	int32_t sequence;
	bool quit;

	bool operator ==(const DBObject & other) const
	{
		return (key == other.key && source == other.source && zone == other.zone &&
				value == other.value && sequence == other.sequence && time == other.time);
	}

	bool operator != (const DBObject & other)
	{
		return (*this == other) == false;
	}
};

class Shared
{
public:
	Shared()
	{
		db = new BaseDB;
	}
	~Shared()
	{
		delete db;
	}

	BaseDB * db;
	UniqueQueue<DBObject> queue;
};

class PlaybackShared
{
public:
	PlaybackShared(AbstractRoutingEngine* re, std::string u, uint playbackMult)
		:routingEngine(re),uuid(u),playBackMultiplier(playbackMult),stop(false) {}
	~PlaybackShared()
	{
		for(auto itr = playbackQueue.begin(); itr != playbackQueue.end(); itr++)
		{
			DBObject obj = *itr;
		}

		playbackQueue.clear();
	}

	AbstractRoutingEngine* routingEngine;
	std::list<DBObject> playbackQueue;
	uint playBackMultiplier;
	std::string uuid;
	bool stop;
};

PROPERTYTYPEBASIC(DatabaseLogging, bool)
PROPERTYTYPEBASIC(DatabasePlayback, bool)
PROPERTYTYPE(DatabaseFile, DatabaseFileType, StringPropertyType, std::string)

class DatabaseSink : public AbstractSource
{

public:
	DatabaseSink(AbstractRoutingEngine* engine, map<string, string> config);
	~DatabaseSink();
	virtual void supportedChanged(const PropertyList & supportedProperties);
	virtual void propertyChanged(AbstractPropertyType *value);
	const std::string uuid();

	///source role:
	virtual void getPropertyAsync(AsyncPropertyReply *reply);
	virtual void getRangePropertyAsync(AsyncRangePropertyReply *reply);
	virtual AsyncPropertyReply * setProperty(AsyncSetPropertyRequest request);
	virtual void subscribeToPropertyChanges(VehicleProperty::Property property);
	virtual void unsubscribeToPropertyChanges(VehicleProperty::Property property);
	virtual PropertyList supported();
	int supportedOperations() { return GetRanged | Get | Set;}

	PropertyInfo getPropertyInfo(VehicleProperty::Property property);

private: //methods:

	void parseConfig();
	void stopDb();
	void startDb();
	void startPlayback();
	void initDb();
	void setPlayback(bool v);
	void setLogging(bool b);
	void setDatabaseFileName(std::string filename);

private:
	PropertyList mSubscriptions;
	Shared *shared;
	GThread* thread;
	//std::string databaseName;
	std::string tablename;
	std::string tablecreate;
	std::list<VehicleProperty::Property> propertiesToSubscribeTo;
	PropertyList mSupported;
	PlaybackShared* playbackShared;
	uint playbackMultiplier;
	DatabasePlaybackType playback;
	DatabaseFileType databaseName;
	DatabaseLoggingType databaseLogging;
};


class DatabaseSinkManager: public AbstractSinkManager
{
public:
	DatabaseSinkManager(AbstractRoutingEngine* engine, map<string, string> config)
	:AbstractSinkManager(engine, config)
	{
		new DatabaseSink(routingEngine, config);
		VehicleProperty::registerProperty(DatabaseLogging, [](){return new DatabaseLoggingType(false);});
		VehicleProperty::registerProperty(DatabasePlayback, [](){return new DatabasePlaybackType(false);});
		VehicleProperty::registerProperty(DatabaseFile, [](){return new DatabaseFileType("storage");});
	}
};

#endif // DATABASESINK_H
