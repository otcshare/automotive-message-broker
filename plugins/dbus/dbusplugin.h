#include "abstractsink.h"


class DBusSink : public AbstractSink
{

public:
	DBusSink(AbstractRoutingEngine* engine);
	virtual PropertyList subscriptions();
	virtual void supportedChanged(PropertyList supportedProperties);
	virtual void propertyChanged(VehicleProperty::Property property, boost::any value, std::string uuid);
	virtual std::string uuid();
};

class DBusSinkManager: public AbstractSinkManager
{
public:
	DBusSinkManager(AbstractRoutingEngine* engine)
	:AbstractSinkManager(engine)
	{
		new DBusSink(engine);
	}
};

