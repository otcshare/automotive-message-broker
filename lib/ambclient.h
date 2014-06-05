#ifndef AMBCLIENT_H_
#define AMBCLIENT_H_

#include <functional>

#include "abstractpropertytype.h"
#include "vehicleproperty.h"

typedef std::function<void (AbstractPropertyType*)> PropertyReplyCbType;

typedef std::function<void (bool)> SetReplyType;



class AbstractAmbClient {
public:

	enum ClientType {
		DBus,
		Bluetooth,
		Websockets
	};

	AbstractAmbClient() {}

	virtual PropertyList supported()=0;

	virtual void get(VehicleProperty property, PropertyReplyCbType reply) = 0;

	virtual void set(AbstractPropertyType* value, SetReplyType reply) = 0;

	virtual int subscribe(VehicleProperty property, PropertyReplyCbType callback) = 0;

	virtual void unsubscribe(int handle) = 0;

	static AbstractAmbClient* create(ClientType type = DBus);
};

#endif
