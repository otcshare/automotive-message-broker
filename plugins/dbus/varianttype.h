#ifndef _VARIANT_TYPE_H_
#define _VARIANT_TYPE_H_

#include "abstractproperty.h"
#include "abstractroutingengine.h"

class VariantType: public AbstractProperty
{
public:

	VariantType(AbstractRoutingEngine* re, string signature, string propertyName, Access access, AbstractDBusInterface* interface);

	void initialize();

	GVariant* toGVariant();
	void fromGVariant(GVariant *value);

private:
	void asyncReply(AsyncPropertyReply*);
	bool mInitialized;


};

#endif
