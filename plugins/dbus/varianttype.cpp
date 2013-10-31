#include "varianttype.h"
#include "abstractroutingengine.h"
#include "debugout.h"
#include "listplusplus.h"

VariantType::VariantType(AbstractRoutingEngine* re, std::string signature, VehicleProperty::Property ambPropertyName, std::string propertyName,  Access access, AbstractDBusInterface *interface)
	:AbstractProperty(propertyName, signature, access, interface), mInitialized(false)
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
	request.completed = [this](AsyncPropertyReply* reply)
	{
		if(reply->success)
			setValue(reply->value->copy());
		else
			DebugOut(DebugOut::Error)<<"get request unsuccessful for "<<reply->property<<" : "<<reply->error<<endl;

		mInitialized = true;

		delete reply;
	};

	/// do not request if not supported:
	PropertyList proplist = routingEngine->supported();

	if(contains(proplist,mAmbPropertyName))
		routingEngine->getPropertyAsync(request);
}

GVariant *VariantType::toGVariant()
{
	if(!value())
	{
		AbstractPropertyType* v = VehicleProperty::getPropertyTypeForPropertyNameValue(mAmbPropertyName);

		setValue(v);

		delete v;
	}

	auto v = value();

	return v->toVariant();
}

void VariantType::fromGVariant(GVariant *val)
{
	AbstractPropertyType* v = VehicleProperty::getPropertyTypeForPropertyNameValue(mAmbPropertyName);
	v->fromVariant( val );

	AsyncSetPropertyRequest request;
	request.property = mAmbPropertyName;
	request.value = v;
	request.completed = [](AsyncPropertyReply* reply)
	{
		/// TODO: throw dbus exception
		if(!reply->success)
		{
			DebugOut(DebugOut::Error)<<"setProperty fail: "<<reply->error<<endl;
		}
		delete reply;
	};

	routingEngine->setProperty(request);
}

