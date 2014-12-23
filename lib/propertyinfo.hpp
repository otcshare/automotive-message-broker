#ifndef PROPERTYINFO_H_
#define PROPERTYINFO_H_

#include "abstractpropertytype.h"

class PropertyInfo
{
public:


	/** PropertyInfo
	 *
	 **/
	PropertyInfo(): mUpdateFrequency(0), mIsValid(false) {}

	/** PropertyInfo
	 * @arg updateFrequency
	 * @arg zonesList
	 */
	PropertyInfo(uint updateFreq, Zone::ZoneList zonesList)
		:mUpdateFrequency(updateFreq), mZones(zonesList), mIsValid(true)
	{

	}


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
	Zone::ZoneList zones()
	{
		return mZones;
	}

	/** isValid
	 * returns whether this PropertyInfo is valid
	 *
	 * default when you construct a PropertyInfo is false
	 **/
	bool isValid()
	{
		return mIsValid;
	}


	/** invalid()
	 * returns instance of PropertyInfo that isn't valid
	 **/
	static PropertyInfo invalid()
	{
		return PropertyInfo();
	}

private:

	uint mUpdateFrequency;
	Zone::ZoneList mZones;
	bool mIsValid;
};


#endif
