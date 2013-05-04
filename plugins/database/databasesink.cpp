#include "databasesink.h"
#include "abstractroutingengine.h"
#include "listplusplus.h"

extern "C" AbstractSinkManager * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new DatabaseSinkManager(routingengine, config);
}

void * cbFunc(gpointer data)
{
	Shared *shared = static_cast<Shared*>(data);

	if(!shared)
	{
		throw std::runtime_error("Could not cast shared object.");
	}

	vector<DictionaryList<string> > insertList;

	while(1)
	{
		DBObject* obj = shared->queue.pop();

		if( obj->quit )
		{
			delete obj;
			break;
		}

		DictionaryList<string> dict;

		NameValuePair<string> one("key", obj->key);
		NameValuePair<string> two("value", obj->value);
		NameValuePair<string> three("source", obj->source);
		NameValuePair<string> four("time", boost::lexical_cast<string>(obj->time));
		NameValuePair<string> five("sequence", boost::lexical_cast<string>(obj->sequence));

		dict.push_back(one);
		dict.push_back(two);
		dict.push_back(three);
		dict.push_back(four);
		dict.push_back(five);

		insertList.push_back(dict);

		if(insertList.size() > 100)
		{
			shared->db->exec("BEGIN IMMEDIATE TRANSACTION");
			for(int i=0; i< insertList.size(); i++)
			{
				DictionaryList<string> d = insertList[i];
				shared->db->insert(d);
			}
			shared->db->exec("END TRANSACTION");
			insertList.clear();
		}
		delete obj;
	}

	/// final flush of whatever is still in the queue:

	shared->db->exec("BEGIN IMMEDIATE TRANSACTION");
	for(int i=0; i< insertList.size(); i++)
	{
		DictionaryList<string> d = insertList[i];
		shared->db->insert(d);
	}
	shared->db->exec("END TRANSACTION");

	return NULL;
}

int getNextEvent(gpointer data)
{
	PlaybackShared* pbshared = static_cast<PlaybackShared*>(data);

	if(!pbshared)
		throw std::runtime_error("failed to cast PlaybackShared object");

	if(pbshared->stop)
		return 0;

	auto itr = pbshared->playbackQueue.begin();

	if(itr == pbshared->playbackQueue.end())
	{
		return 0;
	}

	DBObject* obj = *itr;

	AbstractPropertyType* value = VehicleProperty::getPropertyTypeForPropertyNameValue(obj->key,obj->value);

	if(value)
	{
		pbshared->routingEngine->updateProperty(obj->key, value, pbshared->uuid);
		value->timestamp = obj->time;
		//value->sequence = obj->sequence;
	}

	if(++itr != pbshared->playbackQueue.end())
	{
		DBObject *o2 = *itr;
		double t = o2->time - obj->time;

		if(t > 0)
			g_timeout_add((t*1000) / pbshared->playBackMultiplier, getNextEvent, pbshared);
		else
			g_timeout_add(1, getNextEvent, pbshared);
	}

	pbshared->playbackQueue.remove(obj);
	DebugOut()<<"playback Queue size: "<<pbshared->playbackQueue.size()<<endl;
	delete obj;

	return 0;
}

DatabaseSink::DatabaseSink(AbstractRoutingEngine *engine, map<std::string, std::string> config)
	:AbstractSource(engine,config),thread(NULL),shared(NULL),playback(false),playbackShared(NULL), playbackMultiplier(1)
{
	databaseName = "storage";
	tablename = "data";
	tablecreate = "CREATE TABLE IF NOT EXISTS data (key TEXT, value BLOB, source TEXT, time REAL, sequence REAL)";

	if(config.find("databaseFile") != config.end())
	{
		databaseName = config["databaseFile"];
	}

	if(config.find("properties") != config.end())
	{
		parseConfig();
	}

	for(auto itr=propertiesToSubscribeTo.begin();itr!=propertiesToSubscribeTo.end();itr++)
	{
		engine->subscribeToProperty(*itr,this);
	}

	mSupported.push_back(DatabaseFileProperty);
	mSupported.push_back(DatabaseLoggingProperty);
	mSupported.push_back(DatabasePlaybackProperty);

	/// get supported:

	initDb();

	vector<vector<string> > supportedStr = shared->db->select("SELECT DISTINCT key FROM "+tablename);

	for(int i=0; i < supportedStr.size(); i++)
	{
		if(!ListPlusPlus<VehicleProperty::Property>(&mSupported).contains(supportedStr[i][0]))
			mSupported.push_back(supportedStr[i][0]);
	}

	delete shared;
	shared = NULL;

	routingEngine->setSupported(supported(), this);

	if(config.find("startOnLoad")!= config.end())
	{
		AsyncSetPropertyRequest request;
		request.property = DatabaseLoggingProperty;
		request.value = new DatabaseLoggingType(true);

		setProperty(request);
	}

	if(config.find("playbackMultiplier")!= config.end())
	{
		playbackMultiplier = boost::lexical_cast<uint>(config["playbackMultiplier"]);
	}

	if(config.find("playbackOnLoad")!= config.end())
	{
		AsyncSetPropertyRequest request;
		request.property = DatabasePlaybackProperty;
		request.value = new DatabasePlaybackType(true);

		setProperty(request);
	}


}

DatabaseSink::~DatabaseSink()
{
	if(shared)
	{
		DBObject* obj = new DBObject();
		obj->quit = true;

		shared->queue.append(obj);

		g_thread_join(thread);
		g_thread_unref(thread);
		delete shared;
	}

	if(playbackShared)
	{
		delete playbackShared;
	}
}


void DatabaseSink::supportedChanged(PropertyList supportedProperties)
{

}

PropertyList DatabaseSink::supported()
{
	return mSupported;
}

void DatabaseSink::parseConfig()
{
	json_object *rootobject;
	json_tokener *tokener = json_tokener_new();
	enum json_tokener_error err;
	do
	{
		rootobject = json_tokener_parse_ex(tokener, configuration["properties"].c_str(),configuration["properties"].size());
	} while ((err = json_tokener_get_error(tokener)) == json_tokener_continue);
	if (err != json_tokener_success)
	{
		fprintf(stderr, "Error: %s\n", json_tokener_error_desc(err));
	}
	if (tokener->char_offset < configuration["properties"].size()) // XXX shouldn't access internal fields
	{
		//Should handle the extra data here sometime...
	}
	
	json_object *propobject = json_object_object_get(rootobject,"properties");
	
	g_assert(json_object_get_type(propobject) == json_type_array);

	array_list *proplist = json_object_get_array(propobject);
	
 	for(int i=0; i < array_list_length(proplist); i++)
	{
		json_object *idxobj = (json_object*)array_list_get_idx(proplist,i);
		std::string prop = json_object_get_string(idxobj);
		propertiesToSubscribeTo.push_back(prop);

		DebugOut()<<"DatabaseSink logging: "<<prop<<endl;
	}

	json_object_put(propobject);
	json_object_put(rootobject);
}

void DatabaseSink::stopDb()
{
	if(!shared)
		return;

	DBObject *obj = new DBObject();
	obj->quit = true;
	shared->queue.append(obj);

	g_thread_join(thread);

	delete shared;
	shared = NULL;
}

void DatabaseSink::startDb()
{
	if(playback)
	{
		DebugOut(0)<<"ERROR: tried to start logging during playback.  Only logging or playback can be used at one time"<<endl;
		return;
	}

	if(shared)
	{
		DebugOut(0)<<"WARNING: logging already started.  doing nothing."<<endl;
		return;
	}

	initDb();

	thread = g_thread_new("dbthread", cbFunc, shared);
}

void DatabaseSink::startPlayback()
{
	if(playback)
		return;

	playback = true;

	initDb();

	/// populate playback queue:

	vector<vector<string> > results = shared->db->select("SELECT * FROM "+tablename);

	/// we are done with shared.  clean up:
	delete shared;
	shared = NULL;

	if(playbackShared)
	{
		delete playbackShared;
	}

	playbackShared = new PlaybackShared(routingEngine, uuid(), playbackMultiplier);

	for(int i=0;i<results.size();i++)
	{
		if(results[i].size() < 5)
		{
			throw std::runtime_error("column mismatch in query");
		}

		DBObject* obj = new DBObject();

		obj->key = results[i][0];
		obj->value = results[i][1];
		obj->source = results[i][2];
		obj->time = boost::lexical_cast<double>(results[i][3]);

		/// TODO: figure out why sequence is broken:

//		obj->sequence = boost::lexical_cast<int>(results[i][4]);

		playbackShared->playbackQueue.push_back(obj);
	}

	g_timeout_add(0,getNextEvent,playbackShared);
}

void DatabaseSink::initDb()
{
	if(shared) delete shared;

	shared = new Shared;
	shared->db->init(databaseName, tablename, tablecreate);
}

void DatabaseSink::setPlayback(bool v)
{
	AsyncSetPropertyRequest request;
	request.property = DatabasePlaybackProperty;
	request.value = new DatabasePlaybackType(v);

	setProperty(request);
}

void DatabaseSink::setLogging(bool b)
{
	AsyncSetPropertyRequest request;
	request.property = DatabaseLoggingProperty;
	request.value = new DatabaseLoggingType(b);

	setProperty(request);
}

void DatabaseSink::propertyChanged(VehicleProperty::Property property, AbstractPropertyType *value, std::string uuid)
{
	if(!shared)
		return;

	DBObject* obj = new DBObject;
	obj->key = property;
	obj->value = value->toString();
	obj->source = uuid;
	obj->time = value->timestamp;
	obj->sequence = value->sequence;

	shared->queue.append(obj);
}


std::string DatabaseSink::uuid()
{
	return "9f88156e-cb92-4472-8775-9c08addf50d3";
}

void DatabaseSink::getPropertyAsync(AsyncPropertyReply *reply)
{
	reply->success = false;

	if(reply->property == DatabaseFileProperty)
	{
		DatabaseFilePropertyType temp(databaseName);
		reply->value = &temp;

		reply->success = true;
		reply->completed(reply);

		return;
	}
	else if(reply->property == DatabaseLoggingProperty)
	{
		BasicPropertyType<bool> temp = shared;

		reply->value = &temp;
		reply->success = true;
		reply->completed(reply);

		return;
	}

	else if(reply->property == DatabasePlaybackProperty)
	{
		BasicPropertyType<bool> temp = playback;
		reply->value = &temp;
		reply->success = true;
		reply->completed(reply);

		return;
	}

	reply->completed(reply);
}

void DatabaseSink::getRangePropertyAsync(AsyncRangePropertyReply *reply)
{
	BaseDB * db = new BaseDB();
	db->init(databaseName, tablename, tablecreate);

	ostringstream query;
	query.precision(15);

	query<<"SELECT * from "<<tablename<<" WHERE ";

	if(reply->timeBegin && reply->timeEnd)
	{
		query<<" time BETWEEN "<<reply->timeBegin<<" AND "<<reply->timeEnd;
	}

	if(reply->sequenceBegin >= 0 && reply->sequenceEnd >=0)
	{
		query<<" AND sequence BETWEEN "<<reply->sequenceBegin<<" AND "<<reply->sequenceEnd;
	}

	std::vector<std::vector<string>> data = db->select(query.str());

	std::list<AbstractPropertyType*> cleanup;

	for(auto i=0;i<data.size();i++)
	{
		if(data[i].size() != 5)
			continue;

		DBObject dbobj;
		dbobj.key = data[i][0];
		dbobj.value = data[i][1];
		dbobj.source = data[i][2];
		dbobj.time = boost::lexical_cast<double>(data[i][3]);
		dbobj.sequence = boost::lexical_cast<double>(data[i][4]);

		AbstractPropertyType* property = VehicleProperty::getPropertyTypeForPropertyNameValue(dbobj.key, dbobj.value);
		if(property)
		{
			property->timestamp = dbobj.time;
			property->sequence = dbobj.sequence;

			reply->values.push_back(property);
			cleanup.push_back(property);
		}
	}

	reply->success = true;
	reply->completed(reply);

	delete db;
}

AsyncPropertyReply *DatabaseSink::setProperty(AsyncSetPropertyRequest request)
{
	AsyncPropertyReply* reply = new AsyncPropertyReply(request);
	reply->success = false;

	if(request.property == DatabaseLoggingProperty)
	{
		if(request.value->value<bool>())
		{
			setPlayback(false);
			startDb();
			reply->success = true;
			BasicPropertyType<bool> temp(true);
			routingEngine->updateProperty(DatabaseLoggingProperty,&temp,uuid());
		}
		else
		{
			stopDb();
			reply->success = true;
			BasicPropertyType<bool> temp(false);
			routingEngine->updateProperty(DatabaseLoggingProperty,&temp,uuid());
		}
	}

	else if(request.property == DatabaseFileProperty)
	{
		std::string fname = request.value->toString();

		databaseName = fname;

		StringPropertyType temp(databaseName);

		routingEngine->updateProperty(DatabaseFileProperty,&temp,uuid());

		reply->success = true;
	}
	else if( request.property == DatabasePlaybackProperty)
	{
		if(request.value->value<bool>())
		{
			setLogging(false);
			startPlayback();

			BasicPropertyType<bool> temp(playback);

			routingEngine->updateProperty(DatabasePlaybackProperty,&temp,uuid());
		}
		else
		{
			if(playbackShared)
				playbackShared->stop = true;

			playback = false;

			BasicPropertyType<bool> temp(playback);

			routingEngine->updateProperty(DatabasePlaybackProperty, &temp, uuid());
		}

		reply->success = true;
	}

	return reply;
}

void DatabaseSink::subscribeToPropertyChanges(VehicleProperty::Property )
{

}

void DatabaseSink::unsubscribeToPropertyChanges(VehicleProperty::Property )
{
}
