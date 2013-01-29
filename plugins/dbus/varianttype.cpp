#include "varianttype.h"
#include "abstractroutingengine.h"

VariantType::VariantType(AbstractRoutingEngine* re, std::string signature, std::string propertyName,  Access access, AbstractDBusInterface *interface)
	:AbstractProperty(propertyName, signature, access, interface),routingEngine(re)
{
	//set default value:
	setValue(VehicleProperty::getPropertyTypeForPropertyNameValue(propertyName));

	AsyncPropertyRequest request;
	request.property = mPropertyName;

	request.completed = [this](AsyncPropertyReply* reply)
	{
		setValue(reply->value);
	};

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
