#ifndef _MAPPROPERTYTYPE_H_
#define _MAPPROPERTYTYPE_H_


#include "abstractpropertytype.h"
#include <map>
#include <debugout.h>
//#include <json-glib/json-glib.h>
#include <json/json.h>
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
		json_object *rootobject;
		json_tokener *tokener = json_tokener_new();
		enum json_tokener_error err;
		do
		{
			rootobject = json_tokener_parse_ex(tokener, str.c_str(),str.length());
		} while ((err = json_tokener_get_error(tokener)) == json_tokener_continue);
		if (err != json_tokener_success)
		{
			fprintf(stderr, "Error: %s\n", json_tokener_error_desc(err));
			// Handle errors, as appropriate for your application.
		}
		if (tokener->char_offset < str.length()) // XXX shouldn't access internal fields
		{
			// Handle extra characters after parsed object as desired.
			// e.g. issue an error, parse another object from that point, etc...
		}
		//Good!
		
		json_object_object_foreach(rootobject, key, val)
		{
			T one(key);
			N two(json_object_get_string(val));
			append(one,two);

		}
		json_object_put(rootobject);
		/*
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
		g_object_unref(parser);*/
	}

	GVariant* toVariant()
	{
		GVariantBuilder params;
		g_variant_builder_init(&params, G_VARIANT_TYPE_DICTIONARY);

		for(auto itr = mMap.begin(); itr != mMap.end(); itr++)
		{
			auto &foo = (*itr).first;
			g_variant_builder_add(&params,"{?*}",const_cast<T&>(foo).toVariant(),(*itr).second.toVariant());

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
