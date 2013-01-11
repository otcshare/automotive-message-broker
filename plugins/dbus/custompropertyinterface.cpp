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
		propertyDBusMap[prop] = new VariantType(re, prop, VariantType::ReadWrite, this);
	}

	supportedChanged(re->supported());
}
