#include "databasesink.h"
#include "abstractroutingengine.h"

extern "C" AbstractSinkManager * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new DatabaseSinkManager(routingengine, config);
}

DatabaseSink::DatabaseSink(AbstractRoutingEngine *engine, map<std::string, std::string> config)
	:AbstractSource(engine,config)
{
	databaseName = "storage";
	tablename = "data";
	tablecreate = "CREATE TABLE IF NOT EXISTS data (key TEXT, value BLOB, source TEXT, time REAL, sequence REAL)";
	shared = new Shared;
	shared->db->init(databaseName, tablename, tablecreate);

	engine->subscribeToProperty(VehicleProperty::EngineSpeed, this);
	engine->subscribeToProperty(VehicleProperty::VehicleSpeed, this);

	PropertyList props;
	props.push_back(VehicleProperty::EngineSpeed);
	props.push_back(VehicleProperty::VehicleSpeed);

	engine->setSupported(supported(),this);

	auto cb = [](gpointer data)
	{
		Shared *shared = (Shared*)data;

		while(1)
		{
			DBObject* obj = shared->queue.pop();

			if( obj->quit )
			{
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

		void* ret = NULL;
		return ret;
	};

	thread = g_thread_new("dbthread", cb, shared);

}

DatabaseSink::~DatabaseSink()
{
	DBObject* obj = new DBObject();
	obj->quit = true;

	shared->queue.append(obj);

	g_thread_join(thread);

	delete shared;
}


void DatabaseSink::supportedChanged(PropertyList supportedProperties)
{

}

PropertyList DatabaseSink::supported()
{
	PropertyList props;

	props.push_back(VehicleProperty::EngineSpeed);
	props.push_back(VehicleProperty::VehicleSpeed);

	return props;
}

void DatabaseSink::propertyChanged(VehicleProperty::Property property, AbstractPropertyType *value, std::string uuid)
{
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

	for(auto i=0;i<data.size();i++)
	{
		for(auto n=0;n<data[i].size();n++)
			cout<<"Data: "<<data[i][n]<<endl;
	}

	delete db;
}

AsyncPropertyReply *DatabaseSink::setProperty(AsyncSetPropertyRequest request)
{
	AsyncPropertyReply* reply = new AsyncPropertyReply(request);
	reply->success = false;
	return reply;
}

void DatabaseSink::subscribeToPropertyChanges(VehicleProperty::Property )
{

}

void DatabaseSink::unsubscribeToPropertyChanges(VehicleProperty::Property )
{
}
