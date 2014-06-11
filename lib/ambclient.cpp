#include "ambclient.h"

#include "dbusclient.h"

#ifdef websocket_plugin
//#include "websocketclient.h"
#endif

#ifdef bluetooth_plugin
//#include "bluetoothclient.h"
#endif

AbstractAmbClient *AbstractAmbClient::create(AbstractAmbClient::ClientType type)
{
	if(type == AbstractAmbClient::DBus)
	{
		return new DBusClient();
	}
	else if(type == AbstractAmbClient::Bluetooth)
	{
#ifdef bluetooth_plugin

#endif
	}
	else if(type == AbstractAmbClient::Websockets)
	{
#ifdef websocket_plugin

#endif
	}

	return nullptr;
}
