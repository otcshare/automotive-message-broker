#include "varianttype.h"
#include "abstractroutingengine.h"
#include "abstractdbusinterface.h"
#include "debugout.h"
#include "listplusplus.h"

VariantType::VariantType(AbstractRoutingEngine* re, VehicleProperty::Property ambPropertyName, std::string propertyName,  Access access)
	:AbstractPropertyType(ambPropertyName), mInitialized(false), mAccess(access), mPropertyName(propertyName),
	  routingEngine(re)
{
	name = ambPropertyName;
	//set default value:
	setValue(VehicleProperty::getPropertyTypeForPropertyNameValue(name));
}

void VariantType::initialize()
{
	if(mInitialized) return;
	AsyncPropertyRequest request;
	request.property = name;
	request.sourceUuidFilter = sourceUuid;
	request.zoneFilter = zone;

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

	if(contains(proplist, name))
		routingEngine->getPropertyAsync(request);
}

GVariant *VariantType::toVariant()
{
	if(!value())
	{
		setValue(VehicleProperty::getPropertyTypeForPropertyNameValue(name));
	}

	auto v = value();

	return v->toVariant();
}

void VariantType::fromVariant(GVariant *val)
{
	AbstractPropertyType *v = VehicleProperty::getPropertyTypeForPropertyNameValue(name);
	v->fromVariant(val);

	AsyncSetPropertyRequest request;
	request.property = name;
	request.value = v;
	request.zoneFilter = zone;
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

