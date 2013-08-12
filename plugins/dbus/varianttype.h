#ifndef _VARIANT_TYPE_H_
#define _VARIANT_TYPE_H_

#include "abstractproperty.h"
#include "abstractroutingengine.h"

class VariantType: public AbstractProperty
{
public:

	VariantType(AbstractRoutingEngine* re, string signature, string propertyName, string ambPropName, Access access, AbstractDBusInterface* interface);
	GVariant* toGVariant();
	void fromGVariant(GVariant *value);

private:
	void asyncReply(AsyncPropertyReply*);
	AbstractRoutingEngine* routingEngine;

};

#endif
