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

#ifndef _BASICPROPERTY_H_
#define _BASICPROPERTY_H_

#include "abstractproperty.h"
#include "vehicleproperty.h"
#include "abstractroutingengine.h"
#include <functional>

template <typename T>
class BasicProperty: public AbstractProperty
{
public:
	BasicProperty(AbstractRoutingEngine* re, string ambPropertyName, string propertyName, string signature, Access access, AbstractDBusInterface *interface)
		:AbstractProperty(propertyName, signature, access, interface), routingEngine(re)
	{
		mAmbPropertyName = ambPropertyName;
		//set default value:
		AbstractProperty::setValue(VehicleProperty::getPropertyTypeForPropertyNameValue(ambPropertyName));

		using namespace std::placeholders;

		AsyncPropertyRequest request;
		request.property = ambPropertyName;
		request.completed = std::bind(&BasicProperty<T>::asyncReply,this,_1);
		routingEngine->getPropertyAsync(request);

	}

	void asyncReply(AsyncPropertyReply* reply)
	{
		if(reply->success)
			AbstractProperty::setValue(reply->value);

		delete reply;
	}

	void setValue(T val)
	{
		AbstractProperty::setValue<T>(val);
	}

	T value()
	{
		return AbstractProperty::anyValue<T>();
	}

	virtual GVariant* toGVariant()
	{
		return g_variant_new(signature().c_str(), value());
	}

	virtual void fromGVariant(GVariant *value)
	{
		T val;
		g_variant_get(value,signature().c_str(), &val);

		AbstractPropertyType* apt = VehicleProperty::getPropertyTypeForPropertyNameValue(mAmbPropertyName,"");
		apt->setValue(val);

		AsyncSetPropertyRequest request;
		request.property = mAmbPropertyName;
		request.value = apt;
		request.completed = [apt](AsyncPropertyReply* reply)
		{
			if(!reply->success) {
				//TODO: throw DBus exception
			}
			delete apt;
		};

		routingEngine->setProperty(request);
	}

private:
	VehicleProperty::Property mAmbPropertyName;
	AbstractRoutingEngine* routingEngine;
};


class StringDBusProperty: public AbstractProperty
{
public:

	StringDBusProperty(AbstractRoutingEngine* re, string ambPropertyName, string propertyName, string signature, Access access, AbstractDBusInterface *interface)
		:AbstractProperty(propertyName, signature, access, interface), routingEngine(re), mAmbPropertyName(ambPropertyName)
	{
		//set default value:
		AbstractProperty::setValue(VehicleProperty::getPropertyTypeForPropertyNameValue(ambPropertyName));

		using namespace std::placeholders;

		AsyncPropertyRequest request;
		request.property = ambPropertyName;
		request.completed = std::bind(&StringDBusProperty::asyncReply, this, _1);

		routingEngine->getPropertyAsync(request);
	}

	void asyncReply(AsyncPropertyReply* reply)
	{
		if(reply->success)
			AbstractProperty::setValue(reply->value);

		delete reply;
	}

	void setValue(std::string val)
	{
		AbstractProperty::setValue<std::string>(val);
	}

	virtual GVariant* toGVariant()
	{
		return g_variant_new(signature().c_str(), value()->toString().c_str());
	}

	virtual void fromGVariant(GVariant *value)
	{
		char *val;
		g_variant_get(value,signature().c_str(), &val);

		AbstractPropertyType* apt = VehicleProperty::getPropertyTypeForPropertyNameValue(mAmbPropertyName,val);

		AsyncSetPropertyRequest request;
		request.property = mAmbPropertyName;
		request.value = apt;
		request.completed = [apt](AsyncPropertyReply* reply)
		{
			if(!reply->success) {
				//TODO: throw DBus exception
			}
			delete apt;
		};

		routingEngine->setProperty(request);
	}

private:
	VehicleProperty::Property mAmbPropertyName;
	AbstractRoutingEngine* routingEngine;
};

#endif
