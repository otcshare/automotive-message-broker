#include "custompropertyinterface.h"
#include "vehicleproperty.h"
#include "varianttype.h"

CustomPropertyInterface::CustomPropertyInterface(AbstractRoutingEngine *re, GDBusConnection *connection)
	:DBusSink("org.automotive.custom","/org/automotive/custom", re, connection, map<string, string>())
{
	PropertyList list = VehicleProperty::customProperties();
	for (auto itr = list.begin(); itr != list.end(); itr++)
	{
		VehicleProperty::Property prop = *itr;

		propertyDBusMap[prop] = new VariantType(re, prop, VariantType::ReadWrite, this);
	}

	supportedChanged(re->supported());
}
