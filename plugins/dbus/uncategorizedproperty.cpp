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

	GVariant* var = temp->toVariant();
	std::string signature = g_variant_get_type_string(var);
	g_variant_unref(var);

	propertyDBusMap[prop] = new VariantType(re, signature, prop, prop, VariantType::ReadWrite);

	delete temp;


}
