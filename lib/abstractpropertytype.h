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

#include "debugout.h"
#include "jsonhelper.h"
#include "picojson.h"
#include "superptr.hpp"
#include "timestamp.h"

#include <boost/algorithm/string.hpp>
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/utility.hpp>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include <glib.h>

class Zone {

public:

	typedef int Type;

	enum {
		None = 0,
		Front = 1,
		Middle = 1 << 1,
		Right = 1 << 2,
		Left = 1 << 3,
		Rear = 1 << 4,
		Center = 1 << 5,
		LeftSide = 1 << 6,
		RightSide = 1 << 7,
		FrontSide = 1 << 8,
		BackSide = 1 << 9
	};

static const Zone::Type FrontRight;
static const Zone::Type FrontLeft;
static const Zone::Type MiddleRight;
static const Zone::Type MiddleLeft;
static const Zone::Type RearRight;
static const Zone::Type RearLeft;

typedef std::vector<Zone::Type> ZoneList;

};

class AbstractPropertyType
{
public:

	/*!
	 * \brief The Priority enum describes prority of the property type.
	 */
	enum Priority
	{
		/*!< normal priority.  This is default */
		Normal = 0,
		/*!< Low priority. */
		Low,
		/*!< High priority*/
		High,
		/*!< Instant.  Using this priority is not thread safe.  This is typically used for
		 *    Properties that need to be deterministic.
		 */
		Instant
	};

	AbstractPropertyType(std::string property)
		: name(property), timestamp(amb::currentTime()), sequence(-1), zone(Zone::None), priority(Normal)
	{

	}

	virtual ~AbstractPropertyType()
	{
		for(auto i : destroyed)
		{
			if(i) i(this);
		}
	}

	/*!
	 * \brief toJson convert this type to json representation.
	 * The json typically looks something like this:
	 * \code
	 * {
	 *  "name" : "VehicleSpeed",
	 *  "type" : "UInt16",
	 *  "source" : "daf23v32342ddsdffafaeefe",
	 *  "zone" : 0,
	 *  "value" : 25
	 * }
	 * \endcode
	 * \return json value representing the type
	 */
	virtual const picojson::value toJson();

	/*!
	 * \brief fromJson instantiate this type from json
	 * \param json
	 */
	virtual void fromJson(const picojson::value & json);

	/*!
	 * \brief toString
	 * \return strigified value
	 */
	virtual std::string toString() const = 0;

	/*!
	 * \brief fromString converts from string value
	 */
	virtual void fromString(std::string) = 0;

	/*!
	 * \brief toVariant
	 * \return GVariant representation of value. Caller must unref the returned GVariant
	 */
	virtual GVariant* toVariant() = 0;

	/*!
	 * \brief fromVariant converts GVariant value into compatible native value.  Caller owns
	 * GVariant argument.
	 */
	virtual void fromVariant(GVariant*) = 0;

	/*!
	 * \brief copy
	 * \return a copy of the AbstractPropertyType
	 */
	virtual AbstractPropertyType* copy() = 0;

	/*!
	 * \brief quickCopy is intended as a way to quickly copy the often changing bits from one abstract property to another
	 * It assumes that the properties are almost identical in name, source, and zone.
	 * \param other the property to copy from
	 */
	virtual void quickCopy(AbstractPropertyType* other)
	{
		sequence = other->sequence;
		mValue = other->anyValue();
		timestamp = other->timestamp;
	}

	bool operator == (AbstractPropertyType &other)
	{
		std::string one = toString();
		std::string two = other.toString();
		return one == two
				&& zone == other.zone
				&& sourceUuid == other.sourceUuid
				&& name == other.name;
	}

	bool operator != (AbstractPropertyType &other)
	{
		std::string one = toString();
		std::string two = other.toString();
		return one != two;
	}

	/*!
	 * \brief name Property name. \see VehicleProperty for built-in supported property names
	 */
	std::string name;

	/*!
	 * \brief alias alias for the property name
	 * \return alias if any of name if alias has not been set
	 */
	const std::string alias() { return mAlias.empty() ? name : mAlias; }

	void setAlias(const std::string & a) { mAlias = a; }

	/*!
	 * \brief timestamp.  Timestamp when the value was last updated by the system. This is updated automatically
	 * any time setValue() is called
	 * \see amb::currentTime()
	 * \see setValue()
	 */
	double timestamp;

	/*!
	 * \brief sequence internal counter.  Useful as a unique indentifier.  values is -1 if not used (default).
	 */
	int32_t sequence;

	/*!
	 * \brief sourceUuid  uuid of the source that produced this property.  This is set by the routingengine
	 * if left unmodified.
	 */
	std::string sourceUuid;

	/*!
	 * \brief zone that the property is situated in.
	 */
	Zone::Type zone;

	/*!
	 * \brief priority is used to tell the routing engine how to prioritize routing the value to plugins.
	 * setting this value to AbstractPropertyType::Instant will tell the routing engine to immedietly
	 * route the value without any reliance on the mainloop.  Instant priority is NOT thread safe.
	 * Default priority is AbstractPropertyType::Normal.
	 */
	Priority priority;

	/*!
	 * \brief setValue
	 * \param val boost::any value.  NOTE: boost::any does not accept type coercion.  Types must match exactly
	 * with native type. (ie, don't use "int" if the native type is "uint")
	 */
	virtual void setValue(boost::any val)
	{
		mValue = val;
		timestamp = amb::currentTime();
	}

	/*!
	 * \brief value() native value.  Does not use type coercion.  Will throw if types do not match.
	 */
	template <typename T>
	T value() const
	{
		return boost::any_cast<T>(mValue);
	}

	/*!
	 * \brief anyValue
	 * \return boost::any value
	 */
	boost::any anyValue()
	{
		return mValue;
	}

	/*!
	 * \brief signature
	 * \return gvariant signature
	 */
	virtual const string signature()
	{
		auto var = amb::make_super(toVariant());
		if(!var) return "";

		const string s = g_variant_get_type_string(var.get());
		return s;
	}

	/*!
	 * \brief destroyed is called if this property is destroyed.
	 */
	std::vector<std::function<void(AbstractPropertyType*)>> destroyed;

protected:

	boost::any mValue;

	std::string mAlias;
};

namespace amb
{

struct PropertyCompare
{
	bool operator()(AbstractPropertyType* const & lhs, AbstractPropertyType* & rhs) const
	{
		if (lhs->name == rhs->name
				&& lhs->sourceUuid == rhs->sourceUuid
				&& lhs->zone == rhs->zone)
		{
			return true;
		}

		return false;
	}

};

}


class JsonNumber
{
public:
	static double fromJson(picojson::value v)
	{
		return v.get<double>();
	}

	static picojson::value toJson(double v)
	{
		return picojson::value(v);
	}
};

class JsonBoolean
{
public:
	static bool fromJson(picojson::value v)
	{
		return v.get<bool>();
	}

	static picojson::value toJson(bool v)
	{
		return picojson::value(v);
	}
};

class JsonString
{
public:
	static std::string fromJson(picojson::value v)
	{
		return v.get<std::string>();
	}

	static picojson::value toJson(std::string v)
	{
		return picojson::value(v);
	}
};


template <typename T>
class BaseGVS
{
public:
	static T gvalue(T t)
	{
		return t;
	}
};

template <typename T>
class GVS;

template <>
class GVS<int> : public BaseGVS<int>, public JsonNumber
{
public:
	static const char* signature() { return "i"; }

	static int value(GVariant* v)
	{
		int val = 0;
		g_variant_get(v, signature(), &val);
		return val;
	}

	static std::string stringize(std::string v)
	{
		return v;
	}
};

template <>
class GVS<double> : public BaseGVS<double>, public JsonNumber
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
class GVS<uint16_t> : public BaseGVS<uint16_t>, public JsonNumber
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
class GVS<int16_t> : public BaseGVS<int16_t>, public JsonNumber
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
class GVS<char> : public BaseGVS<char>, public JsonNumber
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
class GVS<uint32_t> : public BaseGVS<uint32_t>, public JsonNumber
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
class GVS<int64_t> : public BaseGVS<int64_t>, public JsonNumber
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
class GVS<uint64_t> : public BaseGVS<uint64_t>, public JsonNumber
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
class GVS<bool> : public BaseGVS<bool>, public JsonBoolean
{
public:
	static const char* signature() { return "b"; }

	static bool value(GVariant *v)
	{
		return g_variant_get_boolean(v);
	}
	static std::string stringize(std::string v)
	{
		if(v == "0" || v == "1")
			return v;

		boost::algorithm::to_lower(v);
		return v == "true" ? "1":"0";
	}
};

template <>
class GVS<std::string> : public JsonString
{
public:
	static const char* signature() { return "s"; }

	static const char* value(GVariant *v)
	{
		return g_variant_get_string(v, nullptr);
	}
	static std::string stringize(std::string v)
	{
		return v;
	}
	static const char* gvalue(std::string v)
	{
		return v.c_str();
	}
};

/*!
 * \brief BasicPropertyType is a typed property type.  Most internal types are derived from this class
 * \example
 * std::unique_ptr<BasicPropertyType<int>> boostPSI = new BasicPropertyType<int>("BoostPSI",5);
 * boostPSI->priority = AbstractPropertyType::Instant; //set instant because we clean up right after.
 * routingEngine->updateProperty(boostPSI.get(), sourceUuid());
 */
template <typename T>
class BasicPropertyType: public AbstractPropertyType
{
public:
	BasicPropertyType(): AbstractPropertyType("")
	{
		mValue = T();
	}

	BasicPropertyType(BasicPropertyType const & other)
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

	BasicPropertyType & operator = (T const & other)
	{
		setValue(other);
		return *this;
	}

	BasicPropertyType & operator ++ ()
	{
		setValue(basicValue() + 1);
	}

	BasicPropertyType & operator -- ()
	{
		setValue(basicValue() - 1);
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

	BasicPropertyType(std::string propertyName)
		:AbstractPropertyType(propertyName)
	{
		mValue = T();
	}

	AbstractPropertyType* copy()
	{
		return new BasicPropertyType<T>(*this);
	}

	const picojson::value toJson()
	{
		picojson::value v = AbstractPropertyType::toJson();

		picojson::object object = v.get<picojson::object>();

		object["value"] = amb::gvariantToJson(toVariant());

		return picojson::value(object);
	}

	virtual void fromJson(const picojson::value &json)
	{
		AbstractPropertyType::fromJson(json);

		fromVariant(amb::jsonToGVariant(json.get("value"), signature()));
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

	/*!
	 * \brief basicValue
	 * \return Typed version of value.  Slightly more useful than \see AbstractPropertyType::value()
	 */

	T basicValue()
	{
		return value<T>();
	}

	void setValue(T val)
	{
		AbstractPropertyType::setValue(val);
	}

	void setValue(boost::any val)
	{
		AbstractPropertyType::setValue(val);
	}

private:

	template <class N>
	void serialize(const std::string & val,  typename std::enable_if<std::is_enum<N>::value, N>::type* = 0)
	{
		int someTemp;

		std::stringstream stream(val);

		stream>>someTemp;
		setValue((N)someTemp);
	}

	template <class N>
	void serialize(const std::string & val,  typename std::enable_if<!std::is_enum<N>::value, N>::type* = 0)
	{
		std::stringstream stream(GVS<T>::stringize(val));
		N someTemp;
		stream>>someTemp;
		setValue(someTemp);
	}

	template <class N>
	GVariant* serializeVariant(const T val, typename std::enable_if<std::is_enum<N>::value, N>::type* = 0)
	{
		return (g_variant_new("i",(int)val));
	}

	template <class N>
	GVariant* serializeVariant(const T val, typename std::enable_if<!std::is_enum<N>::value, N>::type* = 0)
	{
		return g_variant_new(GVS<T>::signature(),val);
	}

	template <class N>
	T deserializeVariant(GVariant* v, typename std::enable_if<std::is_enum<N>::value, N>::type* = 0)
	{
		return (T)GVS<int>::value(v);
	}

	template <class N>
	T deserializeVariant(GVariant* v, typename std::enable_if<!std::is_enum<N>::value, N>::type* = 0)
	{
		return GVS<T>::value(v);
	}
};

class StringPropertyType: public AbstractPropertyType
{
public:


	StringPropertyType()
		:AbstractPropertyType("")
	{
		setValue(std::string());
	}

	StringPropertyType(std::string propertyName)
		:AbstractPropertyType(propertyName)
	{
		setValue(std::string());
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

	StringPropertyType & operator = (std::string const & other)
	{
		setValue(std::string(other));
		return *this;
	}

	bool operator < (const StringPropertyType& other) const
	{
		return value<std::string>() < other.value<std::string>();
	}

	virtual const picojson::value toJson()
	{
		auto val = AbstractPropertyType::toJson();

		picojson::object obj = val.get<picojson::object>();

		obj["value"] = amb::gvariantToJson(toVariant());
	}

	virtual void fromJson(const picojson::value &json)
	{
		AbstractPropertyType::fromJson(json);

		fromString(json.get("value").to_str());
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

/*!
 * \brief ListPropertyType is a AbstractPropertyType for arrays of AbstractPropertyTypes
 */
template <class T>
class ListPropertyType: public AbstractPropertyType
{
public:

	ListPropertyType(std::string propertyName)
		: AbstractPropertyType(propertyName), initialized(false)
	{

	}

	ListPropertyType(std::string propertyName, T value)
		: AbstractPropertyType(propertyName), initialized(false)
	{
		appendPriv(value);
	}

	ListPropertyType(ListPropertyType & other)
		:AbstractPropertyType(other.name),initialized(false)
	{
		std::vector<T> l = other.list();
		for(auto i : l)
		{
			append(i);
		}

		timestamp = other.timestamp;
		sequence = other.sequence;
		sourceUuid = other.sourceUuid;
		name = other.name;
		zone = other.zone;
	}

	~ListPropertyType()
	{
		clear();
	}

	/*! \brief append - appends a property to the list
	 * \arg property - property to be appended.
	 **/
	void append(T property)
	{
		if(!initialized)
		{
			mList.clear();
			initialized = true;
		}

		appendPriv(property);
	}

	uint count()
	{
		return mList.size();
	}

	AbstractPropertyType* copy()
	{
		return new ListPropertyType(*this);
	}

	void quickCopy(AbstractPropertyType* other)
	{
		AbstractPropertyType::quickCopy(other);
		ListPropertyType<T>* v = static_cast<ListPropertyType<T>*>(other);
		if(!v)
		{
			DebugOut(DebugOut::Error) << "ListPropertyType Quick Copy failed" << endl;
			return;
		}
		mList = v->list();
	}

	std::string toString() const
	{
		picojson::array array;

		for(auto i : mList)
		{
			array.push_back(GVS<T>::toJson(i));
		}

		return picojson::value(array).serialize();
	}


	void fromString(std::string str)
	{
		if(!boost::algorithm::starts_with(str, "["))
			str = "[" + str;

		if(!boost::algorithm::ends_with(str, "]"))
			str+= "]";

		picojson::value value;
		picojson::parse(value, str);

		DebugOut()<< "str " << str << endl;


		picojson::array array = value.get<picojson::array>();

		for(auto i : array)
		{
			mList.push_back(GVS<T>::fromJson(i));
		}

		timestamp = amb::currentTime();
	}


	GVariant* toVariant()
	{
		GVariantBuilder params;
		g_variant_builder_init(&params, ((const GVariantType *) "av"));

		for(auto itr : mList)
		{
			GVariant *newvar = g_variant_new("v", g_variant_new(GVS<T>::signature(), GVS<T>::gvalue(itr)));
			g_variant_builder_add_value(&params, newvar);
		}

		GVariant* var =  g_variant_builder_end(&params);
		g_assert(var);
		return var;

	}

	void fromVariant(GVariant* v)
	{
		clear();

		/// TODO: fill this in
		gsize dictsize = g_variant_n_children(v);
		for (int i=0;i<dictsize;i++)
		{
			GVariant *childvariant = g_variant_get_child_value(v,i);
			GVariant *innervariant = g_variant_get_variant(childvariant);
			appendPriv(GVS<T>::value(innervariant));
		}
	}

	std::vector<T> list() { return mList; }

private:

	void clear()
	{
		mList.clear();
	}

	void appendPriv(T i)
	{
		mList.push_back(i);
	}

	bool initialized;

	std::vector<T> mList;
};

#endif
