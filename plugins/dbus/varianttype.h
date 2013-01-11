#ifndef _VARIANT_TYPE_H_
#define _VARIANT_TYPE_H_

#include "abstractproperty.h"
#include "abstractroutingengine.h"

class VariantType: public AbstractProperty
{
public:

	VariantType(AbstractRoutingEngine* re,string propertyName, Access access, AbstractDBusInterface* interface);
	GVariant* toGVariant();
	void fromGVariant(GVariant *value);

private:
	AbstractRoutingEngine* routingEngine;
};

#endif
