#include "uncategorizedproperty.h"
#include "vehicleproperty.h"
#include "varianttype.h"
#include "listplusplus.h"

UncategorizedPropertyInterface::UncategorizedPropertyInterface(VehicleProperty::Property prop, AbstractRoutingEngine *re, GDBusConnection *connection)
	:DBusSink(prop, re, connection, map<string, string>())
{
	AbstractPropertyType* temp = VehicleProperty::getPropertyTypeForPropertyNameValue(prop);

	if(!temp)
	{
		throw std::runtime_error("Cannot create uncategorized property: " + prop);
	}

	std::string signature = g_variant_get_type_string(temp->toVariant());

	propertyDBusMap[prop] = new VariantType(re, signature, prop, VariantType::ReadWrite, this);

	delete temp;

	supportedChanged(re->supported());
}
