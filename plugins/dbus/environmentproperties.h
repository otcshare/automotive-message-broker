#ifndef ENVIRONMENTPROPERTIES_H_
#define ENVIRONMENTPROPERTIES_H_

#include "dbusplugin.h"
#include "abstractdbusinterface.h"
#include "abstractroutingengine.h"

/** @interface ExteriorBrightness : VehiclePropertyType **/
class ExteriorBrightnessProperty: public DBusSink
{
public:
	ExteriorBrightnessProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.ExteriorBrightness","/org/automotive/environment/ExteriorBrightness", re, connection, map<string, string>())
	{
		/**
		 * @attributeName ExteriorBrightness
		 * @type unsigned long
		 * @access readonly
		 * @attributeComment \brief Must return the brightness outside the vehicle in lux.
		 */
		wantProperty<uint16_t>(VehicleProperty::ExteriorBrightness,"ExteriorBrightness", "q", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

#endif
