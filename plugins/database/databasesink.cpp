#include "databasesink.h"
#include "abstractroutingengine.h"
#include "listplusplus.h"

#include <json-glib/json-glib.h>

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

		shared->db->insert(dict);
		delete obj;
	}

	return NULL;
}

int getNextEvent(gpointer data)
{
	PlaybackShared* pbshared = static_cast<PlaybackShared*>(data);

	if(!pbshared)
		throw std::runtime_error("failed to cast PlaybackShared object");

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
	}

	if(++itr != pbshared->playbackQueue.end())
	{
		DBObject *o2 = *itr;
		double t = o2->time - obj->time;

		if(t > 0)
			g_timeout_add(t*1000, getNextEvent, pbshared);
		else
			g_timeout_add(t, getNextEvent, pbshared);
	}

	pbshared->playbackQueue.remove(obj);
	delete obj;

	return 0;
}

DatabaseSink::DatabaseSink(AbstractRoutingEngine *engine, map<std::string, std::string> config)
	:AbstractSource(engine,config),thread(NULL),shared(NULL),playback(false),playbackShared(NULL)
{
	databaseName = "storage";
	tablename = "data";
	tablecreate = "CREATE TABLE IF NOT EXISTS data (key TEXT, value BLOB, source TEXT, time REAL, sequence REAL)";

	//startDb();

	if(config.find("startOnLoad")!= config.end())
	{
		startDb();
	}

	parseConfig();

	for(auto itr=propertiesToSubscribeTo.begin();itr!=propertiesToSubscribeTo.end();itr++)
	{
		engine->subscribeToProperty(*itr,this);
	}

	mSupported.push_back(DatabaseFileProperty);
	mSupported.push_back(DatabaseLoggingProperty);
	mSupported.push_back(DatabasePlaybackProperty);

	routingEngine->setSupported(mSupported,this);

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
	JsonParser* parser = json_parser_new();
	GError* error = nullptr;
	if(!json_parser_load_from_data(parser, configuration["properties"].c_str(),configuration["properties"].size(), &error))
	{
		DebugOut()<<"Failed to load config: "<<error->message;
		throw std::runtime_error("Failed to load config");
	}

	JsonNode* node = json_parser_get_root(parser);

	if(node == nullptr)
		throw std::runtime_error("Unable to get JSON root object");

	JsonReader* reader = json_reader_new(node);

	if(reader == nullptr)
		throw std::runtime_error("Unable to create JSON reader");

	json_reader_read_member(reader,"properties");

	g_assert(json_reader_is_array(reader));

	for(int i=0; i < json_reader_count_elements(reader); i++)
	{
		json_reader_read_element(reader, i);
		std::string prop = json_reader_get_string_value(reader);
		propertiesToSubscribeTo.push_back(prop);
		json_reader_end_element(reader);

		DebugOut()<<"DatabaseSink logging: "<<prop<<endl;
	}

	if(error) g_error_free(error);

	g_object_unref(reader);
	g_object_unref(parser);
}

void DatabaseSink::stopDb()
{
	if(!shared)
		return;

	DBObject *obj = new DBObject();
	obj->quit = true;
	shared->queue.append(obj);

	g_thread_join(thread);
	g_thread_unref(thread);

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

//	thread = g_thread_new("dbthread", cbFunc, shared);
}

void DatabaseSink::startPlayback()
{
	if(playback)
		return;

	playback = true;

	initDb();

	/// get supported:

	vector<vector<string> > supportedStr = shared->db->select("SELECT DISTINCT key FROM "+tablename);

	for(int i=0; i < supportedStr.size(); i++)
	{
		if(!ListPlusPlus<VehicleProperty::Property>(&mSupported).contains(supportedStr[i][0]))
			mSupported.push_back(supportedStr[i][0]);
	}

	routingEngine->setSupported(supported(), this);

	/// populate playback queue:

	vector<vector<string> > results = shared->db->select("SELECT * FROM "+tablename);

	if(playbackShared)
	{
		delete playbackShared;
	}

	playbackShared = new PlaybackShared(routingEngine,uuid());

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
		obj->sequence = boost::lexical_cast<uint16_t>(results[i][4]);

		playbackShared->playbackQueue.push_back(obj);
	}
}

void DatabaseSink::initDb()
{
	if(shared) delete shared;

	shared = new Shared;
	shared->db->init(databaseName, tablename, tablecreate);
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
		StringPropertyType temp(databaseName);
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

	/// reply is owned by the requester of this call.  we own the data:
	for(auto itr = cleanup.begin(); itr != cleanup.end(); itr++)
	{
		delete *itr;
	}

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
			///TODO: start or stop logging thread
			startDb();
			reply->success = true;
		}
		else
		{
			stopDb();
			reply->success = true;
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
			startPlayback();
		}
		else
		{
			/// TODO: stop playback
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
