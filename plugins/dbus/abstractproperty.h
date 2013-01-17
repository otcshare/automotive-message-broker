/*
Copyright (C) 2012 Intel Corporation

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

#ifndef ABSTRACTPROPERTY_H
#define ABSTRACTPROPERTY_H

#include <string>
#include <functional>
#include <boost/any.hpp>
#include <glib.h>

#include "debugout.h"
#include "abstractpropertytype.h"

class AbstractDBusInterface;

using namespace std;

typedef function<void (boost::any)> SetterFunc;

class AbstractProperty
{

public:

	enum Access {
		Read,
		Write,
		ReadWrite
	};

	AbstractProperty(string propertyName, string signature, Access access, AbstractDBusInterface* interface);
	
	virtual void setSetterFunction(SetterFunc setterFunc)
	{
		mSetterFunc = setterFunc;
	}
	
	virtual string signature()
	{
		return mSignature;
	}
	
	virtual string name() 
	{
		return mPropertyName;
	}

	virtual Access access()
	{
		return mAccess;
	}
	
	virtual GVariant* toGVariant() = 0;
	virtual void fromGVariant(GVariant *value) = 0;

	double timestamp()
	{
		return mTimestamp;
	}

	virtual void setValue(AbstractPropertyType* val)
	{
		mValue = val->copy();
		mAnyValue = val->anyValue();
		mTimestamp = val->timestamp;
		updateValue();
	}

	template<typename T>
	void setValue(T val)
	{
		mAnyValue = val;
		updateValue();
	}



	template<typename T>
	T anyValue()
	{
		return boost::any_cast<T>(mAnyValue);
	}

	AbstractPropertyType* value()
	{
		return mValue;
	}
	
protected: ///methods:

	void updateValue();
	
protected:
	
	boost::any mAnyValue;
	string mPropertyName;
	string mSignature;
	SetterFunc mSetterFunc;
	Access mAccess;
	double mTimestamp;
	AbstractPropertyType* mValue;
	AbstractDBusInterface* mInterface;
};

#endif // ABSTRACTPROPERTY_H
