#include "databasesink.h"

DatabaseSink::DatabaseSink(AbstractRoutingEngine *engine, map<std::string, std::string> config)
	:AbstractSink(engine,config)
{
	shared = new Shared;
	shared->db->init("storage","data","CREATE TABLE IF NOT EXISTS vehicledata (key TEXT, value BLOB, time REAL, source TEXT");

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

}


PropertyList DatabaseSink::subscriptions()
{

}


void DatabaseSink::supportedChanged(PropertyList supportedProperties)
{

}


void DatabaseSink::propertyChanged(VehicleProperty::Property property, AbstractPropertyType *value, std::string uuid, double timestamp, uint32_t sequence)
{
	DBObject* obj = new DBObject;
	obj->key = property;
	obj->value = value->toString();
	obj->source = uuid;
}


std::string DatabaseSink::uuid()
{
	return "9f88156e-cb92-4472-8775-9c08addf50d3";
}
