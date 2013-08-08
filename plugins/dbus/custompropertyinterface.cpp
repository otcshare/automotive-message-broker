#include "custompropertyinterface.h"
#include "vehicleproperty.h"
#include "varianttype.h"
#include "listplusplus.h"

CustomPropertyInterface::CustomPropertyInterface(VehicleProperty::Property prop, AbstractRoutingEngine *re, GDBusConnection *connection)
	:DBusSink("org.automotive."+prop,"/org/automotive/custom/"+prop, re, connection, map<string, string>())
{
	PropertyList list = VehicleProperty::customProperties();

	if(ListPlusPlus<VehicleProperty::Property>(&list).contains(prop))
	{
		AbstractPropertyType* temp = VehicleProperty::getPropertyTypeForPropertyNameValue(prop);

		if(!temp)
		{
			throw std::runtime_error("Cannot create custom property: " + prop);
		}

		std::string signature = g_variant_get_type_string(temp->toVariant());

		propertyDBusMap[prop] = new VariantType(re, signature, prop, prop, VariantType::ReadWrite, this);

		delete temp;
	}

	supportedChanged(re->supported());
}
