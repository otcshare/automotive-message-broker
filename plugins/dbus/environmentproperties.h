#ifndef ENVIRONMENTPROPERTIES_H_
#define ENVIRONMENTPROPERTIES_H_

#include "dbusplugin.h"
#include "abstractdbusinterface.h"
#include "abstractroutingengine.h"


class ExteriorBrightnessProperty: public DBusSink
{
public:
	ExteriorBrightnessProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.exteriorBrightness","/org/automotive/environment/exteriorBrightness", re, connection, map<string, string>())
	{
		wantProperty<uint16_t>(VehicleProperty::ExteriorBrightness,"ExteriorBrightness", "y", AbstractProperty::Read);
		supportedChanged(re->supported());
	}


};

#endif
