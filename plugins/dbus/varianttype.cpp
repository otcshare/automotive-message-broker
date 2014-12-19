#include "varianttype.h"
#include "abstractroutingengine.h"
#include "debugout.h"
#include "listplusplus.h"

VariantType::VariantType(AbstractRoutingEngine* re, VehicleProperty::Property ambPropertyName, std::string propertyName,  Access access)
	:AbstractProperty(propertyName, access), mInitialized(false)
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

GVariant *VariantType::toGVariant()
{
	if(!value())
	{
		setValue(VehicleProperty::getPropertyTypeForPropertyNameValue(mAmbPropertyName));
	}

	auto v = value();

	return v->toVariant();
}

void VariantType::fromGVariant(GVariant *val)
{
	AbstractPropertyType *v = VehicleProperty::getPropertyTypeForPropertyNameValue(mAmbPropertyName);
	v->fromVariant(val);

	AsyncSetPropertyRequest request;
	request.property = mAmbPropertyName;
	request.value = v;
	request.zoneFilter = mZoneFilter;
	request.completed = [&](AsyncPropertyReply* r)
	{
		auto reply = amb::make_unique(r);
		/// TODO: throw dbus exception
		if(!reply->success)
		{
			DebugOut(DebugOut::Error)<<"SetProperty fail: "<<reply->error<<endl;
		}
	};

	routingEngine->setProperty(request);
}

