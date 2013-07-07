#include "varianttype.h"
#include "abstractroutingengine.h"
#include "debugout.h"

VariantType::VariantType(AbstractRoutingEngine* re, std::string signature, std::string propertyName,  Access access, AbstractDBusInterface *interface)
	:AbstractProperty(propertyName, signature, access, interface), routingEngine(re), mInitialized(false)
{
	//set default value:
	setValue(VehicleProperty::getPropertyTypeForPropertyNameValue(propertyName));
}

void VariantType::initialize()
{
	if(mInitialized) return;

	AsyncPropertyRequest request;
	request.property = mPropertyName;
	request.sourceUuidFilter = mSourceFilter;
	request.zoneFilter = mZoneFilter;

	using namespace std::placeholders;
	request.completed = [this](AsyncPropertyReply* reply)
	{
		if(reply->success)
			setValue(reply->value);
		else
			DebugOut(DebugOut::Error)<<"get request unsuccessful for "<<reply->property<<" : "<<reply->error<<endl;

		mInitialized = true;

		delete reply;
	};

	routingEngine->getPropertyAsync(request);
}

GVariant *VariantType::toGVariant()
{
	if(!value())
	{
		AbstractPropertyType* v = VehicleProperty::getPropertyTypeForPropertyNameValue(mPropertyName);

		setValue(v);

		delete v;
	}

	auto v = value();

	return v->toVariant();
}

void VariantType::fromGVariant(GVariant *val)
{
	AbstractPropertyType* v = VehicleProperty::getPropertyTypeForPropertyNameValue(mPropertyName);
	v->fromVariant( val );

	AsyncSetPropertyRequest request;
	request.property = mPropertyName;
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

