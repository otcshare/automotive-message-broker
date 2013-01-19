#include "varianttype.h"
#include "abstractroutingengine.h"

VariantType::VariantType(AbstractRoutingEngine* re, std::string signature, std::string propertyName,  Access access, AbstractDBusInterface *interface)
	:AbstractProperty(propertyName, signature, access, interface),routingEngine(re)
{
	AsyncPropertyRequest request;
	request.property = mPropertyName;

	VariantType* foo = this;

	request.completed = [foo](AsyncPropertyReply* reply)
	{
		foo->setValue(reply->value);
	};

	re->getPropertyAsync(request);

}

GVariant *VariantType::toGVariant()
{
	if(!value())
	{

		AbstractPropertyType* v = VehicleProperty::getPropertyTypeForPropertyNameValue(mPropertyName);

		return v->toVariant()->gobj();
	}

	return value()->toVariant()->gobj();
}

void VariantType::fromGVariant(GVariant *val)
{
	AbstractPropertyType* v = VehicleProperty::getPropertyTypeForPropertyNameValue(mPropertyName);
	v->fromVariant( new Glib::VariantBase(val) );

	AsyncSetPropertyRequest request;
	request.property = mPropertyName;
	request.value = v;
	request.completed = [](AsyncPropertyReply* reply)
	{
		delete reply;
	};

	routingEngine->setProperty(request);
}
