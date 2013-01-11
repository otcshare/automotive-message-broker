#include "varianttype.h"

VariantType::VariantType(AbstractRoutingEngine* re, std::string propertyName,  Access access, AbstractDBusInterface *interface)
	:AbstractProperty(propertyName, "v", access, interface)
{
//VehicleProperty::getPropertyTypeForPropertyNameValue(propertyName,"")->toVariant()->get_type_string()
}

GVariant *VariantType::toGVariant()
{
	if(!value())
		return g_variant_new("v",g_variant_new_int16(0));

	return value()->toVariant()->gobj();
}

void VariantType::fromGVariant(GVariant *value)
{

}
