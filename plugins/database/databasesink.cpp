#include "databasesink.h"
#include "abstractroutingengine.h"
#include "listplusplus.h"
#include "superptr.hpp"
#include "uuidhelper.h"
#include "ambplugin.h"

#include <thread>

int bufferLength = 100;
int timeout=1000;

extern "C" void create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	auto plugin = new AmbPlugin<DatabaseSink>(routingengine, config);
	plugin->init();
}

static void * cbFunc(Shared* shared)
{
	if(!shared)
	{
		throw std::runtime_error("Could not get shared object.");
	}

	///new tripID:
	shared->tripId = amb::createUuid();

	vector<DictionaryList<string> > insertList;

	double startTime = amb::currentTime();

	while(1)
	{
		DBObject obj = shared->queue.pop();

		if( obj.quit )
		{
			break;
		}

		DictionaryList<string> dict;

		NameValuePair<string> one("key", obj.key);
		NameValuePair<string> two("value", obj.value);
		NameValuePair<string> three("source", obj.source);
		NameValuePair<string> zone("zone", boost::lexical_cast<string>(obj.zone));
		NameValuePair<string> four("time", boost::lexical_cast<string>(amb::Timestamp::instance()->epochTime(obj.time)));
		NameValuePair<string> five("sequence", boost::lexical_cast<string>(obj.sequence));
		NameValuePair<string> six("tripId", shared->tripId);

		dict.push_back(one);
		dict.push_back(two);
		dict.push_back(three);
		dict.push_back(zone);
		dict.push_back(four);
		dict.push_back(five);
		dict.push_back(six);

		insertList.push_back(dict);

		if(insertList.size() >= bufferLength && amb::currentTime() - startTime >= timeout / 1000)
		{
			startTime = amb::currentTime();

			shared->db->exec("BEGIN IMMEDIATE TRANSACTION");
			for(int i=0; i< insertList.size(); i++)
			{
				DictionaryList<string> d = insertList[i];
				shared->db->insert(d);
			}
			shared->db->exec("END TRANSACTION");
			insertList.clear();
		}
		//delete obj;
	}

	/// final flush of whatever is still in the queue:

	shared->db->exec("BEGIN IMMEDIATE TRANSACTION");
	for(auto d : insertList)
	{
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

	DBObject obj = *itr;

	auto value = amb::make_unique(VehicleProperty::getPropertyTypeForPropertyNameValue(obj.key, obj.value));

	if(value)
	{
		value->priority = AbstractPropertyType::Instant;
		value->timestamp = obj.time;
		value->sequence = obj.sequence;
		value->sourceUuid = obj.source;
		value->zone = obj.zone;
		pbshared->routingEngine->updateProperty(value.get(), pbshared->uuid);
	}

	if(++itr != pbshared->playbackQueue.end())
	{
		DBObject o2 = *itr;
		double t = o2.time - obj.time;

		if(t > 0)
			g_timeout_add((t*1000) / pbshared->playBackMultiplier, getNextEvent, pbshared);
		else
			g_timeout_add(1, getNextEvent, pbshared);
	}

	pbshared->playbackQueue.remove(obj);
	DebugOut()<<"playback Queue size: "<<pbshared->playbackQueue.size()<<endl;

	return 0;
}

DatabaseSink::DatabaseSink(AbstractRoutingEngine *engine, map<std::string, std::string> config, AbstractSource &parent)
	:AmbPluginImpl(engine, config, parent), shared(nullptr), playback(false), playbackShared(nullptr), playbackMultiplier(1)
{
	tablename = "data";
	tablecreate = database_table_create;

	if(config.find("bufferLength") != config.end())
	{
		bufferLength = boost::lexical_cast<int>(config["bufferLength"]);
	}

	if(config.find("frequency") != config.end())
	{
		try
		{
			int t = boost::lexical_cast<int>(config["frequency"]);
			timeout = 1000 / t;
		}catch(...)
		{
			DebugOut(DebugOut::Error)<<"Failed to parse frequency: Invalid value "<<config["frequency"]<<endl;
		}
	}

	if(config.find("properties") != config.end())
	{
		parseConfig();
	}

	for(auto itr : propertiesToSubscribeTo)
	{
		engine->subscribeToProperty(itr, &parent);
	}

	databaseName = addPropertySupport(Zone::None, [](){ return new DatabaseFileType("storage"); });
	playback = addPropertySupport(Zone::None, [](){ return new DatabasePlaybackType(false); });
	databaseLogging = addPropertySupport(Zone::None, [](){ return new DatabaseLoggingType(false); });

	if(config.find("startOnLoad")!= config.end())
	{
		DebugOut() << "start on load? " << config["startOnLoad"] << endl;
		databaseLogging->setValue(config["startOnLoad"] == "true");
	}

	if(config.find("playbackMultiplier")!= config.end())
	{
		playbackMultiplier = boost::lexical_cast<uint>(config["playbackMultiplier"]);
	}

	if(config.find("playbackOnLoad")!= config.end())
	{
		playback->setValue(config["playbackOnLoad"] == "true");
	}
}

DatabaseSink::~DatabaseSink()
{
	if(shared)
	{
		stopDb();
	}

	if(playbackShared)
	{
		delete playbackShared;
	}
}


void DatabaseSink::supportedChanged(const PropertyList &supportedProperties)
{

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

	//json_object_put(propobject);
	json_object_put(rootobject);
}

void DatabaseSink::stopDb()
{
	databaseLogging->setValue(false);

	if(!shared)
		return;

	DBObject obj;
	obj.quit = true;
	shared->queue.append(obj);

	if(thread && thread->joinable())
		thread->join();

	delete shared;
	shared = NULL;

	routingEngine->updateProperty(databaseLogging.get(), source.uuid());
}

void DatabaseSink::startDb()
{
	if(playback->value<bool>())
	{
		DebugOut(DebugOut::Error)<<"ERROR: tried to start logging during playback.  Only logging or playback can be used at one time"<<endl;
		return;
	}

	if(shared)
	{
		DebugOut(DebugOut::Warning)<<"WARNING: logging already started.  doing nothing."<<endl;
		return;
	}

	initDb();

	if(thread && thread->joinable())
		thread->detach();

	thread = amb::make_unique(new std::thread(cbFunc, shared));

	databaseLogging->setValue(true);
	routingEngine->updateProperty(databaseLogging.get(), source.uuid());
}

void DatabaseSink::startPlayback()
{
	if(playback->value<bool>())
		return;

	playback->setValue(true);

	initDb();

	/// populate playback queue:

	vector<vector<string> > results = shared->db->select("SELECT * FROM "+tablename);

	stopDb();

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

		DBObject obj;

		obj.key = results[i][0];
		obj.value = results[i][1];
		obj.source = results[i][2];
		obj.zone = boost::lexical_cast<int>(results[i][3]);
		obj.time = boost::lexical_cast<double>(results[i][4]);
		obj.sequence = boost::lexical_cast<double>(results[i][5]);

		playbackShared->playbackQueue.push_back(obj);
	}

	g_timeout_add(0, getNextEvent, playbackShared);
}

void DatabaseSink::initDb()
{
	if(shared) delete shared;

	shared = new Shared;
	shared->db->init(databaseName->value<std::string>(), tablename, tablecreate);
}

void DatabaseSink::updateForNewDbFilename()
{
	bool wasLogging = databaseLogging->value<bool>();

	stopDb();
	initDb();

	vector<vector<string> > supportedStr = shared->db->select("SELECT DISTINCT key, zone, source FROM " + tablename);

	for(int i=0; i < supportedStr.size(); i++)
	{
		std::string name = supportedStr[i][0];

		if(!contains(supported(), name))
		{
			std::string zoneStr = supportedStr[i][1];
			std::string sourceStr = supportedStr[i][2];

			DebugOut() << "adding property " << name << " in zone: " << zoneStr << "for source: " << sourceStr << endl;

			Zone::Type zone = boost::lexical_cast<Zone::Type>(zoneStr);
			auto property = addPropertySupport(zone, [name]() { return VehicleProperty::getPropertyTypeForPropertyNameValue(name); });
			property->sourceUuid = sourceStr;
		}
	}

	if(wasLogging)
	{
		stopDb();
		startDb();
	}

	routingEngine->updateSupported(supported(), PropertyList(), &source);
}

void DatabaseSink::propertyChanged(AbstractPropertyType *value)
{
	VehicleProperty::Property property = value->name;

	DebugOut() << "Received property change for " << property << endl;

	if(!shared)
		return;

	if(!contains(supported(), property))
	{
		addPropertySupport(value->zone, [property]() { return VehicleProperty::getPropertyTypeForPropertyNameValue(property);});
		routingEngine->updateSupported(supported(), PropertyList(), &source);
	}

	DBObject obj;
	obj.key = property;
	obj.value = value->toString();
	obj.source = value->sourceUuid;
	obj.time = value->timestamp;
	obj.sequence = value->sequence;
	obj.zone = value->zone;

	shared->queue.append(obj);
}


const std::string DatabaseSink::uuid() const
{
	return "9f88156e-cb92-4472-8775-9c08addf50d3";
}

void DatabaseSink::init()
{
	if(configuration.find("databaseFile") != configuration.end())
	{
		setValue(databaseName, configuration["databaseFile"]);
		updateForNewDbFilename();
	}

	DebugOut() << "databaseLogging: " << databaseLogging->value<bool>() << endl;

	routingEngine->updateSupported(supported(), PropertyList(), &source);
}

void DatabaseSink::getRangePropertyAsync(AsyncRangePropertyReply *reply)
{
	BaseDB * db = new BaseDB();
	db->init(databaseName->value<std::string>(), tablename, tablecreate);

	ostringstream query;
	query.precision(15);

	query<<"SELECT * from "<<tablename<<" WHERE (";

	for(auto itr = reply->properties.begin(); itr != reply->properties.end(); itr++)
	{
		DebugOut() << "prop: " << (*itr) << endl;
		if(itr != reply->properties.begin())
			query<<" OR ";

		query<<"key='"<<(*itr)<<"'";
	}

	query<<")";

	if(reply->timeEnd)
	{
		query<<" AND time BETWEEN "<<reply->timeBegin<<" AND "<<reply->timeEnd;
	}

	if(reply->sequenceBegin >= 0 && reply->sequenceEnd >=0)
	{
		query<<" AND sequence BETWEEN "<<reply->sequenceBegin<<" AND "<<reply->sequenceEnd;
	}

	if(reply->sourceUuid != "")
		query<<" AND source='"<<reply->sourceUuid<<"'";

	query<<" AND zone="<<reply->zone;

	std::vector<std::vector<string>> data = db->select(query.str());

	DebugOut()<<"Dataset size "<<data.size()<<endl;

	if(!data.size())
	{
		reply->success = false;
		reply->error = AsyncPropertyReply::InvalidOperation;
	}
	else
	{
		reply->success = true;
	}

	for(auto i=0; i<data.size(); i++)
	{
		if(data[i].size() != 7)
			continue;

		DBObject dbobj;
		dbobj.key = data[i][0];
		dbobj.value = data[i][1];
		dbobj.source = data[i][2];
		dbobj.zone = boost::lexical_cast<double>(data[i][3]);
		dbobj.time = boost::lexical_cast<double>(data[i][4]);
		dbobj.sequence = boost::lexical_cast<double>(data[i][5]);
		dbobj.tripId = data[i][6];

		AbstractPropertyType* property = VehicleProperty::getPropertyTypeForPropertyNameValue(dbobj.key, dbobj.value);
		if(property)
		{
			property->timestamp = dbobj.time;
			property->sequence = dbobj.sequence;

			reply->values.push_back(property);
		}
	}


	reply->completed(reply);

	delete db;
}

AsyncPropertyReply *DatabaseSink::setProperty(const AsyncSetPropertyRequest &request)
{
	AsyncPropertyReply* reply = AmbPluginImpl::setProperty(request);

	if(request.property == DatabaseLogging)
	{
		if(databaseLogging->value<bool>())
		{
			startDb();
		}
		else
		{
			stopDb();
		}
	}
	else if(request.property == DatabaseFile)
	{
		updateForNewDbFilename();
	}
	else if( request.property == DatabasePlayback)
	{
		if(playback->value<bool>())
		{
			startPlayback();
		}
		else
		{
			if(playbackShared)
				playbackShared->stop = true;

			playback = false;
		}
	}

	return reply;
}

void DatabaseSink::subscribeToPropertyChanges(VehicleProperty::Property )
{

}

void DatabaseSink::unsubscribeToPropertyChanges(VehicleProperty::Property )
{
}

