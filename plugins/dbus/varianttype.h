#ifndef _VARIANT_TYPE_H_
#define _VARIANT_TYPE_H_

#include "abstractpropertytype.h"
#include "abstractroutingengine.h"

class AbstractDBusInterface;

class VariantType: public AbstractPropertyType
{
public:

	typedef function<void (boost::any)> SetterFunc;

	enum Access {
		Read,
		Write,
		ReadWrite
	};

	VariantType(AbstractRoutingEngine* re, VehicleProperty::Property ambPropertyName, string propertyName, Access access);

	void initialize();

	GVariant* toVariant();
	void fromVariant(GVariant *value);

	bool operator == (VariantType & other)
	{
		return (other.dbusName() == dbusName()
				&& other.ambPropertyName() == ambPropertyName()
				&& other.sourceFilter() == sourceFilter()
				&& other.zoneFilter() == zoneFilter());
	}

	virtual void setSetterFunction(SetterFunc setterFunc)
	{
		mSetterFunc = setterFunc;
	}

	virtual std::string dbusName()
	{
		return mPropertyName;
	}

	virtual std::string ambPropertyName()
	{
		return name;
	}

	virtual Access access()
	{
		return mAccess;
	}

	void setSourceFilter(std::string filter)
	{
		sourceUuid = filter;
	}
	void setZoneFilter(Zone::Type z)
	{
		zone = z;
	}

	std::string sourceFilter() { return sourceUuid; }
	Zone::Type zoneFilter() { return zone; }

	virtual void setValue(AbstractPropertyType* val)
	{
		if(!val)
			return;

		mValue->quickCopy(val);

		if(mUpdateFrequency == 0)
		{
			PropertyInfo info = routingEngine->getPropertyInfo(mValue->name, mValue->sourceUuid);

			if(info.isValid())
				mUpdateFrequency = info.updateFrequency();
			else
				mUpdateFrequency = -1;
		}
	}

	virtual void updateValue(AbstractPropertyType* val)
	{
		setValue(val);
	}

	int updateFrequency()
	{
		return mUpdateFrequency;
	}

	AbstractPropertyType* value()
	{
		return mValue;
	}

	std::string toString() const
	{
		return "";
	}

	void fromString(std::string str)
	{

	}

	AbstractPropertyType * copy()
	{
		return new VariantType(routingEngine, name, mPropertyName, mAccess);
	}

protected:
	int mUpdateFrequency;
	AbstractRoutingEngine* routingEngine;
	string mPropertyName;
	SetterFunc mSetterFunc;
	Access mAccess;
	AbstractPropertyType* mValue;
	AbstractDBusInterface* mInterface;

private:
	void asyncReply(AsyncPropertyReply*);
	bool mInitialized;


};

typedef VariantType AbstractProperty;

#endif
