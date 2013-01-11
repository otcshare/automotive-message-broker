#ifndef CUSTOMPROPERTYINTERFACE_H_
#define CUSTOMPROPERTYINTERFACE_H_

#include "dbusplugin.h"

class CustomPropertyInterface: DBusSink
{
public:
	CustomPropertyInterface(AbstractRoutingEngine* re, GDBusConnection* connection);
};


#endif
