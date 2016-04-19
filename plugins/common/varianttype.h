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

#ifndef VARIANTTYPE_H_
#define VARIANTTYPE_H_

#include <string>
#include <functional>
#include <boost/any.hpp>
#include <glib.h>

#include "debugout.h"
#include "abstractpropertytype.h"
#include "abstractroutingengine.h"
#include "vehicleproperty.h"

class AbstractDBusInterface;

class VariantType
{

public:

	enum Access {
		Read,
		Write,
		ReadWrite
	};

	VariantType(AbstractRoutingEngine* re, VehicleProperty::Property ambPropertyName, std::string propertyName,  Access access);
	virtual ~VariantType();

	bool operator == (VariantType & other)
	{
		return (other.name() == name()
				&& other.ambPropertyName() == ambPropertyName()
				&& other.sourceFilter() == sourceFilter()
				&& other.zoneFilter() == zoneFilter());
	}

	virtual const string signature()
	{
		GVariant* var = toVariant();
		if(!var) return "";

		const string s = g_variant_get_type_string(var);
		g_variant_unref(var);
		return s;
	}

	virtual string name()
	{
		return mPropertyName;
	}

	virtual VehicleProperty::Property ambPropertyName()
	{
		return mAmbPropertyName;
	}

	virtual Access access()
	{
		return mAccess;
	}

	void setSourceFilter(std::string filter) { mSourceFilter = filter; }
	void setZoneFilter(Zone::Type zone)
	{
		if(mValue)
			mValue->zone = zone;

		mZoneFilter = zone;
	}

	std::string sourceFilter() { return mSourceFilter; }
	Zone::Type zoneFilter() { return mZoneFilter; }

	virtual GVariant* toVariant();
	virtual void fromVariant(GVariant *value, std::function<void (bool, AsyncPropertyReply::Error)> callback);

	double timestamp()
	{
		if(mValue)
			return mValue->timestamp;
		return 0;
	}

	int sequence()
	{
		if(mValue)
			return mValue->sequence;
		return 0;
	}

	virtual void setValue(AbstractPropertyType* val)
	{
		if(!val)
			return;

		if(!mValue)
		{
			mValue = val->copy();
		}
		else
		{
			mValue->quickCopy(val);
		}

		if(mUpdateFrequency == 0)
		{
			PropertyInfo info = routingEngine->getPropertyInfo(mValue->name, mValue->sourceUuid);

			if(info.isValid())
				mUpdateFrequency = info.updateFrequency();
			else
				mUpdateFrequency = -1;
		}
	}

	virtual void updateValue(AbstractPropertyType* val)
	{
		setValue(val);
	}

	int updateFrequency()
	{
		return mUpdateFrequency;
	}

	AbstractPropertyType* value()
	{
		return mValue;
	}

	void initialize();

protected:

	std::string mSourceFilter;
	Zone::Type mZoneFilter;
	int mUpdateFrequency;
	AbstractRoutingEngine* routingEngine;
	string mPropertyName;
	VehicleProperty::Property mAmbPropertyName;
	Access mAccess;
	AbstractPropertyType* mValue;
	AbstractDBusInterface* mInterface;

private:
	bool mInitialized;
};

#endif // ABSTRACTPROPERTY_H
