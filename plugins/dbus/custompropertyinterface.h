#ifndef CUSTOMPROPERTYINTERFACE_H_
#define CUSTOMPROPERTYINTERFACE_H_

#include "dbusplugin.h"

class CustomPropertyInterface: public DBusSink
{
public:
	CustomPropertyInterface(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection);
};


#endif
