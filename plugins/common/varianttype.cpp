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


#include "varianttype.h"
#include "abstractdbusinterface.h"
#include <listplusplus.h>

VariantType::VariantType(AbstractRoutingEngine* re, VehicleProperty::Property ambPropertyName, std::string propertyName,  Access access)
	:mPropertyName(propertyName), mAccess(access), mValue(nullptr), mZoneFilter(Zone::None), mUpdateFrequency(0), mInitialized(false)
{
	mAmbPropertyName = ambPropertyName;
	routingEngine = re;
	//set default value:
	setValue(VehicleProperty::getPropertyTypeForPropertyNameValue(mAmbPropertyName));
}

void VariantType::initialize()
{
	if(mInitialized) return;
	AsyncPropertyRequest request;
	request.property = mAmbPropertyName;
	request.sourceUuidFilter = mSourceFilter;
	request.zoneFilter = mZoneFilter;

	using namespace std::placeholders;
	request.completed = [this](AsyncPropertyReply* r)
	{
		auto reply = amb::make_unique(r);
		if(reply->success)
			setValue(reply->value->copy());
		else
			DebugOut(DebugOut::Error)<<"get request unsuccessful for "<<reply->property<<" : "<<reply->error<<endl;

		mInitialized = true;
	};

	/// do not request if not supported:
	PropertyList proplist = routingEngine->supported();

	if(contains(proplist, mAmbPropertyName))
		routingEngine->getPropertyAsync(request);
}

VariantType::~VariantType()
{
	if(mValue){
		delete mValue;
		mValue = nullptr;
	}
}

GVariant *VariantType::toVariant()
{
	if(!value())
	{
		setValue(VehicleProperty::getPropertyTypeForPropertyNameValue(name()));
	}

	auto v = value();

	return v->toVariant();
}

void VariantType::fromVariant(GVariant *val, std::function<void (bool, AsyncPropertyReply::Error)> callback)
{
	AbstractPropertyType *v = VehicleProperty::getPropertyTypeForPropertyNameValue(ambPropertyName());

	if(!v)
	{
		DebugOut(DebugOut::Error) << "could not get AbstractPropertyType for: " << ambPropertyName() << endl;
		return;
	}

	v->fromVariant(val);

	AsyncSetPropertyRequest request;
	request.property = ambPropertyName();
	request.value = v;
	request.zoneFilter = zoneFilter();
	request.completed = [&](AsyncPropertyReply* r)
	{
		auto reply = amb::make_unique(r);

		if(callback)
			callback(reply->success, reply->error);
	};

	routingEngine->setProperty(request);
}
