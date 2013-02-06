#include "varianttype.h"
#include "abstractroutingengine.h"

VariantType::VariantType(AbstractRoutingEngine* re, std::string signature, std::string propertyName,  Access access, AbstractDBusInterface *interface)
	:AbstractProperty(propertyName, signature, access, interface),routingEngine(re)
{
	//set default value:
	setValue(VehicleProperty::getPropertyTypeForPropertyNameValue(propertyName));

	AsyncPropertyRequest request;
	request.property = mPropertyName;

	using namespace std::placeholders;
	request.completed = std::bind(&VariantType::asyncReply, this, _1);

	re->getPropertyAsync(request);

}

GVariant *VariantType::toGVariant()
{
	if(!value())
	{

		AbstractPropertyType* v = VehicleProperty::getPropertyTypeForPropertyNameValue(mPropertyName);

		setValue(v);

		delete v;
	}

	return value()->toVariant();
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
		delete reply;
	};

	routingEngine->setProperty(request);
}

void VariantType::asyncReply(AsyncPropertyReply * reply)
{
	setValue(reply->value);

	delete reply;
}
