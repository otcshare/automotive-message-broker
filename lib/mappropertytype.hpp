#ifndef _MAPPROPERTYTYPE_H_
#define _MAPPROPERTYTYPE_H_


#include "abstractpropertytype.h"
#include <map>
#include <debugout.h>
#include <json-glib/json-glib.h>

template <class T, class N>
class MapPropertyType: public AbstractPropertyType
{
public:
	MapPropertyType(){}

	void append(T  key, N  value)
	{
		appendPriv(key,value);
	}

	AbstractPropertyType* copy()
	{
		MapPropertyType<T,N> *t = new MapPropertyType<T,N>();

		t->setMap(mMap);

		return t;
	}

	std::string toString() const
	{
		std::stringstream str;

		str<<"{";

		for(auto itr = mMap.begin(); itr != mMap.end(); itr++)
		{
			if(str.str() != "{")
				str << ", ";

			 auto t = *itr;

			 str <<"'"<< t.first.toString() <<"':'"<<t.second.toString()<<"'";
		}

		str << "}";

		return str.str();
	}

	void fromString(std::string str)
	{
		JsonParser* parser = json_parser_new();
		GError* error = nullptr;
		if(!json_parser_load_from_data(parser, str.c_str(), str.length(), &error))
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

		DebugOut()<<"Config members: "<<json_reader_count_members(reader)<<endl;

		gchar** srcMembers = json_reader_list_members(reader);

		for(int i=0; i< json_reader_count_members(reader); i++)
		{
			json_reader_read_member(reader,srcMembers[i]);
			T one(srcMembers[i]);
			N two(json_reader_get_string_value(reader));

			append(one,two);
			json_reader_end_member(reader);
		}

		g_free(srcMembers);
		g_object_unref(reader);
		g_object_unref(parser);
	}

	GVariant* toVariant()
	{
		GVariantBuilder params;
		g_variant_builder_init(&params, G_VARIANT_TYPE_DICTIONARY);

		for(auto itr = mMap.begin(); itr != mMap.end(); itr++)
		{
			GVariant **v = g_new(GVariant*,2);
//			v[0] = (*itr).first.toVariant();
			v[1] = (*itr).second.toVariant();
			GVariant* tuple = g_variant_new_tuple(v,2);

			g_variant_builder_add_value(&params,tuple);

			g_free(v);
		}

		GVariant* var =  g_variant_builder_end(&params);
		g_assert(var);
		return var;
	}

	void fromVariant(GVariant*)
	{

	}

	void setMap(std::map<T, N> m)
	{
		mMap = m;
	}

private:

	void appendPriv(T  key, N  value)
	{
		mMap[key] = value;
	}

	std::map<T, N> mMap;
};


#endif
