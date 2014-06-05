#include "dbusclient.h"


PropertyList DBusClient::supported()
{
}

void DBusClient::get(VehicleProperty property, PropertyReplyCbType reply)
{
}

void DBusClient::set(AbstractPropertyType *value, SetReplyType reply)
{
}

int DBusClient::subscribe(VehicleProperty property, PropertyReplyCbType callback)
{
}

void DBusClient::unsubscribe(int handle)
{
}
