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

template <typename T>
class BasicProperty: public AbstractProperty
{
public:
	BasicProperty(AbstractRoutingEngine* re, string ambPropertyName, string propertyName, string signature, Access access, AbstractDBusInterface *interface)
		:AbstractProperty(propertyName, signature, access, interface)
	{
		mAmbPropertyName = ambPropertyName;
	}

	void setValue(T val)
	{
		AbstractProperty::setValue<T>(val);
	}

	T value()
	{
		return AbstractProperty::value<T>();
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

		routingEngine->setProperty(mAmbPropertyName, apt);

		///delete this because we should be done
		delete apt;
	}

private:
	VehicleProperty::Property mAmbPropertyName;
	AbstractRoutingEngine* routingEngine;
};

#endif
