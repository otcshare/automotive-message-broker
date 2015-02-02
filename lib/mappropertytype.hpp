#ifndef _MAPPROPERTYTYPE_H_
#define _MAPPROPERTYTYPE_H_


#include "abstractpropertytype.h"

#include <map>
#include <debugout.h>
#include "picojson.h"

template <class N>
class MapPropertyType: public AbstractPropertyType
{
public:
	MapPropertyType(std::string propertyName):AbstractPropertyType(propertyName){}

	void append(std::string  key, N  value)
	{
		appendPriv(key, value);
	}

	AbstractPropertyType* copy()
	{
		MapPropertyType<N> *t = new MapPropertyType<N>(name);

		t->setMap(mMap);
		t->timestamp = timestamp;
		t->sequence = sequence;
		t->sourceUuid = sourceUuid;
		t->name = name;
		t->zone = zone;

		return t;
	}

	std::string toString() const
	{
		std::stringstream str;

		str<<"{";

		for(auto itr: mMap)
		{
			if(str.str() != "{")
				str << ", ";


			str <<"\""<< itr.first <<"\":\""<<itr.second.toString()<<"\"";
		}

		str << "}";

		return str.str();
	}

	void fromString(std::string str)
	{
		clear();

		DebugOut() << str << endl;

		picojson::value value;
		picojson::parse(value, str);

		std::string picojsonerr = picojson::get_last_error();

		if(!value.is<picojson::object>() || !picojsonerr.empty())
		{
			DebugOut(DebugOut::Warning) << "JSon is invalid for MapPropertyType: " << str << endl;
			DebugOut(DebugOut::Warning) << picojsonerr << endl;
			return;
		}

		const picojson::object& obj = value.get<picojson::object>();

		DebugOut() << "obj size: " << obj.size() << endl;

		for(auto i : obj)
		{
			std::string key = i.first;
			N val("");
			val.fromString(i.second.to_str());
			append(key, val);
		}
	}

	GVariant* toVariant()
	{
		GVariantBuilder params;
		g_variant_builder_init(&params, G_VARIANT_TYPE_DICTIONARY);
		for(auto itr = mMap.begin(); itr != mMap.end(); itr++)
		{
			auto &foo = (*itr).first;
			g_variant_builder_add(&params,"{sv}",foo.c_str(), (*itr).second.toVariant());
		}

		GVariant* var =  g_variant_builder_end(&params);
		g_assert(var);
		return var;
	}

	void fromVariant(GVariant* variant)
	{
		clear();
		gsize dictsize = g_variant_n_children(variant);
		for (int i=0;i<dictsize;i++)
		{
			GVariant *childvariant = g_variant_get_child_value(variant,i);
			gsize dictvalsize = g_variant_n_children(childvariant);
			if (dictvalsize == 2)
			{
				//It is a dictionary entry
				GVariant *keyvariant = g_variant_get_child_value(childvariant,0);
				GVariant *valvariant = g_variant_get_child_value(childvariant,1);
				std::string key = g_variant_get_string(keyvariant, nullptr);
				N n = N();
				GVariant *innerValue = nullptr;

				DebugOut() << "variantType: " << g_variant_get_type_string(valvariant) << endl;

				g_variant_get(valvariant, "v", &innerValue);

				DebugOut() << "inner variange: " << g_variant_get_type_string(innerValue) << endl;

				n.fromVariant(innerValue);
				appendPriv(key,n);
			}
		}

	}

	void setMap(std::map<std::string, N> m)
	{
		mMap = m;
	}

private:

	void clear()
	{
		mMap.clear();
	}

	void appendPriv(std::string  key, N  value)
	{
		mMap[key] = value;
	}

	std::map<std::string, N> mMap;
};


#endif
