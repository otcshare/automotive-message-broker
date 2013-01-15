#include "varianttype.h"

VariantType::VariantType(AbstractRoutingEngine* re, std::string signature, std::string propertyName,  Access access, AbstractDBusInterface *interface)
	:AbstractProperty(propertyName, signature, access, interface),routingEngine(re)
{
//VehicleProperty::getPropertyTypeForPropertyNameValue(propertyName,"")->toVariant()->get_type_string()
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
