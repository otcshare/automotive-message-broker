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
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/utility.hpp>
#include <type_traits>
#include <glibmm/variant.h>
#include "timestamp.h"

class AbstractPropertyType
{
public:
	AbstractPropertyType(): timestamp(-1), sequence(-1) {}

	virtual std::string toString() const = 0;

	virtual void fromString(std::string)= 0;

	virtual Glib::VariantBase* toVariant() = 0;

	virtual void fromVariant(Glib::VariantBase*) = 0;

	virtual AbstractPropertyType* copy() = 0;

	double timestamp;

	int32_t sequence;

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
class BasicPropertyType: public AbstractPropertyType
{
public:
	BasicPropertyType(BasicPropertyType const &other)
		:AbstractPropertyType()
	{
		setValue(other.value<T>());
	}

	BasicPropertyType & operator = (BasicPropertyType const & other)
	{
		setValue(other.value<T>());
		return *this;
	}

	BasicPropertyType(T val)
		:AbstractPropertyType()
	{
		setValue(val);
	}

	BasicPropertyType(std::string val)
		:AbstractPropertyType()
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
		stream<<value<T>();

		return stream.str();
	}

	Glib::VariantBase* toVariant()
	{
		serializeVariant<T>(value<T>());

		return &mVariant;
	}

	void fromVariant(Glib::VariantBase *v)
	{
		setValue(deserializeVariant<T>(v));
	}

private:

	Glib::VariantBase mVariant;

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
		std::stringstream stream(val);
		N someTemp;
		stream>>someTemp;
		setValue(someTemp);
	}

	template <class N>
	void serializeVariant(T val, typename std::enable_if<std::is_enum<N>::value, N>::type* = 0)
	{
		mVariant = Glib::VariantBase(Glib::Variant<gint16>::create((int)val).gobj());
	}

	template <class N>
	void serializeVariant(T val, typename std::enable_if<!std::is_enum<N>::value, N>::type* = 0)
	{
		//mVariant = Glib::Variant<int>::create(0);
		mVariant = Glib::Variant<T>::create(val);
	}

	template <class N>
	T deserializeVariant(Glib::VariantBase* v, typename std::enable_if<std::is_enum<N>::value, N>::type* = 0)
	{
		return (T)((Glib::Variant<int>::cast_dynamic<Glib::Variant<int> >(*v)).get());
	}

	template <class N>
	T deserializeVariant(Glib::VariantBase* v, typename std::enable_if<!std::is_enum<N>::value, N>::type* = 0)
	{
		return Glib::VariantBase::cast_dynamic<Glib::Variant<T> >(*v).get();
	}


};

class StringPropertyType: public AbstractPropertyType
{
public:
	StringPropertyType(std::string val)
		:AbstractPropertyType()
	{
		setValue(val);
	}

	StringPropertyType(StringPropertyType const & other)
	:AbstractPropertyType()
	{
		setValue(other.value<std::string>());
	}

	StringPropertyType & operator = (StringPropertyType const & other)
	{
		setValue(other.value<std::string>());
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

	Glib::VariantBase* toVariant()
	{
		mVariant = Glib::Variant<std::string>::create(toString());

		return &mVariant;
	}

	void fromVariant(Glib::VariantBase *v)
	{
		setValue(std::string(v->print()));
	}

private:

	Glib::Variant<std::string> mVariant;
};

#endif
