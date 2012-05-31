/*
    Copyright 2012 Kevron Rees <email>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/


#ifndef ABSTRACTPROPERTY_H
#define ABSTRACTPROPERTY_H

#include <string>
#include <functional>
#include <boost/any.hpp>
#include <glib.h>


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

	AbstractProperty(string propertyName, string signature, Access access);
	
	virtual void setSetterFunction(SetterFunc setterFunc)
	{
		mSetterFunc = setterFunc;
	}
	
	virtual void setValue(boost::any val)
	{
		mValue = val;
	}

	boost::any value() { return mValue; }

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

protected:
	string mPropertyName;
	string mSignature;
	SetterFunc mSetterFunc;
	boost::any mValue;
	Access mAccess;
};

#endif // ABSTRACTPROPERTY_H
