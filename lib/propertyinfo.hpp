#ifndef PROPERTYINFO_H_
#define PROPERTYINFO_H_

#include "abstractpropertytype.h"
#include <list>

class PropertyInfo
{
public:
	/** PropertyInfo
	 *
	 **/
	PropertyInfo(): mUpdateFrequency(0) {}


	/** updateFrequency
	 * Maximum times per second a property is expected to update.
	 *
	 **/
	uint updateFrequency()
	{
		return mUpdateFrequency;
	}

	/** zones
	 * Number of different zones supported by this property.
	 *
	 */
	std::list<Zone::Type> zones()
	{
		return mZones;
	}

private:

	uint mUpdateFrequency;
	std::list<Zone::Type> mZones;
};


#endif
