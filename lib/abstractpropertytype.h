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

#ifndef _ABSTRACTPROPERTYTYPE_H_
#define _ABSTRACTPROPERTYTYPE_H_

#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/utility.hpp>
#include <type_traits>
#include <glib.h>
#include <list>
#include "timestamp.h"
#include <debugout.h>
#include <boost/algorithm/string.hpp>

namespace Zone {

enum Type {
	None = 0,
	FrontLeft = 1,
	FrontMiddle = 1 << 1,
	FrontRight = 1 << 2,
	RearLeft = 1 << 3,
	RearMiddle = 1 << 4,
	RearRight = 1 << 5
};

typedef std::list<Zone::Type> ZoneList;

}

class AbstractPropertyType
{
public:
	AbstractPropertyType(std::string property): name(property), timestamp(-1), sequence(-1), zone(Zone::None) {}

	virtual ~AbstractPropertyType() { }

	virtual std::string toString() const = 0;

	virtual void fromString(std::string)= 0;

	virtual GVariant* toVariant() = 0;

	virtual void fromVariant(GVariant*) = 0;

	virtual AbstractPropertyType* copy() = 0;

	bool operator == (AbstractPropertyType &other)
	{
		std::string one = toString();
		std::string two = other.toString();
		return one == two;
	}

	bool operator != (AbstractPropertyType &other)
	{
		std::string one = toString();
		std::string two = other.toString();
		return one != two;
	}

	std::string name;

	double timestamp;

	int32_t sequence;

	std::string sourceUuid;

	Zone::Type zone;

	void setValue(boost::any val)
	{
		mValue = val;
		timestamp = amb::currentTime();
	}

	template <typename T>
	T value() const
	{
		return boost::any_cast<T>(mValue);
	}

	boost::any anyValue()
	{
		return mValue;
	}

protected:

	boost::any mValue;

};

template <typename T>
class GVS;

template <>
class GVS<int>
{
public:
	static const char* signature() { return "i"; }

	static char value(GVariant* v)
	{
		return g_variant_get_int32(v);
	}

	static std::string stringize(std::string v)
	{
		return v;
	}
};

template <>
class GVS<double>
{
public:
	static const char* signature() { return "d"; }

	static double value(GVariant* v)
	{
		return g_variant_get_double(v);
	}
	static std::string stringize(std::string v)
	{
		return v;
	}
};

template <>
class GVS<uint16_t>
{
public:
	static const char* signature() { return "q"; }

	static uint16_t value(GVariant* v)
	{
		return g_variant_get_uint16(v);
	}
	static std::string stringize(std::string v)
	{
		return v;
	}
};

template <>
class GVS<int16_t>
{
public:
	static const char* signature() { return "n"; }

	static int16_t value(GVariant* v)
	{
		return g_variant_get_int16(v);
	}
	static std::string stringize(std::string v)
	{
		return v;
	}
};

template <>
class GVS<char>
{
public:
	static const char* signature() { return "y"; }

	static char value(GVariant* v)
	{
		return g_variant_get_byte(v);
	}
	static std::string stringize(std::string v)
	{
		return v;
	}
};

template <>
class GVS<uint32_t>
{
public:
	static const char* signature() { return "u"; }

	static uint32_t value(GVariant* v)
	{
		return g_variant_get_uint32(v);
	}
	static std::string stringize(std::string v)
	{
		return v;
	}
};

template <>
class GVS<int64_t>
{
public:
	static const char* signature() { return "x"; }

	static int64_t value(GVariant* v)
	{
		return g_variant_get_int64(v);
	}
	static std::string stringize(std::string v)
	{
		return v;
	}
};

template <>
class GVS<uint64_t>
{
public:
	static const char* signature() { return "t"; }

	static uint64_t value(GVariant* v)
	{
		return g_variant_get_uint64(v);
	}
	static std::string stringize(std::string v)
	{
		return v;
	}
};

template <>
class GVS<bool>
{
public:
	static const char* signature() { return "b"; }

	static bool value(GVariant *v)
	{
		return g_variant_get_boolean(v);
	}
	static std::string stringize(std::string v)
	{
		boost::algorithm::to_lower(v);
		return v == "true" ? "1":"0";
	}
};


template <typename T>
class BasicPropertyType: public AbstractPropertyType
{
public:
	BasicPropertyType(): AbstractPropertyType("")
	{
		mValue = T();
	}
	BasicPropertyType(BasicPropertyType const &other)
		:AbstractPropertyType(other.name)
	{
		setValue(other.value<T>());
		timestamp = other.timestamp;
		sequence = other.sequence;
		sourceUuid = other.sourceUuid;
		name = other.name;
		zone = other.zone;

	}

	BasicPropertyType & operator = (BasicPropertyType const & other)
	{
		setValue(other.value<T>());
		timestamp = other.timestamp;
		sequence = other.sequence;
		sourceUuid = other.sourceUuid;
		name = other.name;
		zone = other.zone;

		return *this;
	}

	bool operator < (const BasicPropertyType<T>& other) const
	{
		return value<T>() < other.value<T>();
	}

	bool operator > (const BasicPropertyType<T>& other) const
	{
		return value<T>() > other.value<T>();
	}

	BasicPropertyType( T val)
		:AbstractPropertyType("")
	{
		setValue(val);
	}

	BasicPropertyType( std::string propertyName, T val)
		:AbstractPropertyType(propertyName)
	{
		setValue(val);
	}

	BasicPropertyType( std::string propertyName, std::string val)
		:AbstractPropertyType(propertyName)
	{
		if(!val.empty() && val != "")
		{
			serialize<T>(val);
		}
		else setValue(T());
	}

	BasicPropertyType(std::string val)
		:AbstractPropertyType("")
	{
		if(!val.empty() && val != "")
		{
			serialize<T>(val);
		}
		else setValue(T());
	}

	AbstractPropertyType* copy()
	{
		return new BasicPropertyType<T>(*this);
	}

	void fromString(std::string val)
	{
		if(!val.empty() && val != "")
		{
			serialize<T>(val);
		}
	}

	std::string toString() const
	{
		std::stringstream stream;
		stream.precision(10);
		stream<<value<T>();

		return stream.str();
	}

	GVariant* toVariant()
	{
		return serializeVariant<T>(value<T>());
	}

	void fromVariant(GVariant *v)
	{
		setValue(deserializeVariant<T>(v));
	}

private:

	//GVariant* mVariant;

	template <class N>
	void serialize(std::string val,  typename std::enable_if<std::is_enum<N>::value, N>::type* = 0)
	{
		int someTemp;

		std::stringstream stream(val);

		stream>>someTemp;
		setValue((N)someTemp);
	}

	template <class N>
	void serialize(std::string  val,  typename std::enable_if<!std::is_enum<N>::value, N>::type* = 0)
	{
		std::stringstream stream(GVS<T>::stringize(val));
		N someTemp;
		stream>>someTemp;
		setValue(someTemp);
	}

	template <class N>
	GVariant* serializeVariant(T val, typename std::enable_if<std::is_enum<N>::value, N>::type* = 0)
	{
		//mVariant = Glib::VariantBase(Glib::Variant<gint16>::create((int)val).gobj());

		return (g_variant_new("i",(int)val));
	}

	template <class N>
	GVariant* serializeVariant(T val, typename std::enable_if<!std::is_enum<N>::value, N>::type* = 0)
	{
		//mVariant = Glib::Variant<T>::create(val);
		//mVariant = g_variant_ref(g_variant_new(GVS<T>::signature(),val));
		return g_variant_new(GVS<T>::signature(),val);
	}

	template <class N>
	T deserializeVariant(GVariant* v, typename std::enable_if<std::is_enum<N>::value, N>::type* = 0)
	{
//		return (T)((Glib::Variant<int>::cast_dynamic<Glib::Variant<int> >(*v)).get());

		return (T)GVS<int>::value(v);
	}

	template <class N>
	T deserializeVariant(GVariant* v, typename std::enable_if<!std::is_enum<N>::value, N>::type* = 0)
	{
		//	return Glib::VariantBase::cast_dynamic<Glib::Variant<T> >(*v).get();
		return GVS<T>::value(v);
	}
};

class StringPropertyType: public AbstractPropertyType
{
public:
	StringPropertyType(std::string propertyName)
		:AbstractPropertyType(propertyName)
	{

	}

	StringPropertyType(std::string propertyName, std::string val)
		:AbstractPropertyType(propertyName)
	{
		setValue(val);
	}

	StringPropertyType(StringPropertyType const & other)
	:AbstractPropertyType(other.name)
	{
		setValue(other.value<std::string>());
		timestamp = other.timestamp;
		sequence = other.sequence;
		sourceUuid = other.sourceUuid;
		name = other.name;
		zone = other.zone;
	}

	StringPropertyType & operator = (StringPropertyType const & other)
	{
		setValue(other.value<std::string>());
		timestamp = other.timestamp;
		sequence = other.sequence;
		sourceUuid = other.sourceUuid;
		name = other.name;
		zone = other.zone;

		return *this;
	}

	void fromString(std::string val)
	{
		setValue(val);
	}

	AbstractPropertyType* copy()
	{
		return new StringPropertyType(*this);
	}

	std::string toString() const
	{
		return value<std::string>();
	}

	GVariant* toVariant()
	{
		//mVariant = Glib::Variant<std::string>::create(toString());

		return g_variant_new_string(toString().c_str());

	}

	void fromVariant(GVariant *v)
	{
		setValue(std::string(g_variant_get_string(v,NULL)));
	}
};

template <class T>
class ListPropertyType: public AbstractPropertyType
{
public:

	ListPropertyType(std::string propertyName)
		: AbstractPropertyType(propertyName), initialized(false)
	{

	}

	ListPropertyType(std::string propertyName, AbstractPropertyType *value)
		: AbstractPropertyType(propertyName), initialized(false)
	{
		appendPriv(value->copy());
	}

	ListPropertyType(ListPropertyType & other)
		:AbstractPropertyType(other.name),initialized(false)
	{
		std::list<AbstractPropertyType*> l = other.list();
		for(auto itr = l.begin(); itr != l.end(); itr++)
		{
			append(*itr);
		}

		timestamp = other.timestamp;
		sequence = other.sequence;
		sourceUuid = other.sourceUuid;
		name = other.name;
		zone = other.zone;
	}

	~ListPropertyType()
	{
		for(auto itr = mList.begin(); itr != mList.end(); itr++)
		{
			delete *itr;
		}
	}

	/** append - appends a property to the list
	 * @arg property - property to be appended.  Property will be copied and owned by ListPropertyType.
	 * You are responsible for freeing property after append is called.
	 **/
	void append(AbstractPropertyType* property)
	{
		if(!initialized)
		{
			for(auto itr = mList.begin(); itr != mList.end(); itr++)
			{
				AbstractPropertyType *p = *itr;
				delete p;
			}
			mList.clear();
			initialized = true;
		}

		appendPriv(property->copy());
	}

	uint count()
	{
		return mList.size();
	}

	AbstractPropertyType* copy()
	{
		return new ListPropertyType(*this);
	}

	std::string toString() const
	{
		std::string str = "[";

		for(auto itr = mList.begin(); itr != mList.end(); itr++)
		{
			if(str != "[")
				str += ",";

			AbstractPropertyType* t = *itr;

			str += t->toString();
		}

		str += "]";

		return str;
	}


	void fromString(std::string str )
	{
		if(!str.length())
			return;

		if(str[0] != '[' && str[str.length()-1] != ']')
		{
			return;
		}

		str = str.substr(1,str.length() - 2);

		std::vector<std::string> elements;

		std::istringstream f(str);

		std::string element;
		while(std::getline(f,element,','))
		{
			T *foo = new T(element);
			append (foo);

			delete foo;
		}
	}


	GVariant* toVariant()
	{

		GVariantBuilder params;
		g_variant_builder_init(&params, ((const GVariantType *) "av"));

		for(auto itr = mList.begin(); itr != mList.end(); itr++)
		{
			AbstractPropertyType* t = *itr;
			GVariant *var = t->toVariant();
			GVariant *newvar = g_variant_new("v",var);
			g_variant_builder_add_value(&params, newvar);
			
		}

		GVariant* var =  g_variant_builder_end(&params);
		g_assert(var);
		return var;

	}

	void fromVariant(GVariant* v)
	{
		/// TODO: fill this in
		gsize dictsize = g_variant_n_children(v);
		for (int i=0;i<dictsize;i++)
		{
			GVariant *childvariant = g_variant_get_child_value(v,i);
			GVariant *innervariant = g_variant_get_variant(childvariant);
			T *t = new T();
			t->fromVariant(innervariant);
			appendPriv(t);
		}
	}

	std::list<AbstractPropertyType*> list() { return mList; }

private:
	void appendPriv(AbstractPropertyType* i)
	{
		mList.push_back(i);
	}

	bool initialized;

	std::list<AbstractPropertyType*> mList;
};

#endif
