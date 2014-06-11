#ifndef DBUSCLIENT_H_
#define DBUSCLIENT_H_

#include "ambclient.h"

class DBusClient : public AbstractAmbClient
{
public:


	// AbstractAmbClient interface
public:
	PropertyList supported();
	void get(VehicleProperty property, PropertyReplyCbType reply);
	void set(AbstractPropertyType *value, SetReplyType reply);
	int subscribe(VehicleProperty property, PropertyReplyCbType callback);
	void unsubscribe(int handle);
};

#endif
