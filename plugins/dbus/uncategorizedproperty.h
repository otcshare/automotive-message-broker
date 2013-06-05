#ifndef UncategorizedPropertyInterface_H_
#define UncategorizedPropertyInterface_H_

#include "dbusplugin.h"

class UncategorizedPropertyInterface: DBusSink
{
public:
	UncategorizedPropertyInterface(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection);
};


#endif
