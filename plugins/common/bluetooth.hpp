#ifndef OBDBLUETOOTH
#define OBDBLUETOOTH

#include <string>
#include "bluetoothmanagerproxy.h"
#include "bluetoothadapterproxy.h"
#include "bluetoothserialproxy.h"


class GpsBluetoothDevice
{
public:

	std::string getDeviceForAddress( std::string address,std::string adapterAddy = "")
	{
		GError* error = NULL;
		OrgBluezManager* manager = org_bluez_manager_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM,
																				 G_DBUS_PROXY_FLAGS_NONE,
																				 "org.bluez","/",NULL, &error);

		if(!manager)
		{
			DebugOut(0)<<"Error getting bluetooth manager proxy: "<<error->message<<endl;
			g_error_free(error);
			return "";
		}

		error = NULL;

		gchar* adapterPath;

		if(adapterAddy != "")
		{
			if(!org_bluez_manager_call_find_adapter_sync(manager,adapterAddy.c_str(), &adapterPath, NULL, &error))
			{
				DebugOut(0)<<"Error getting bluetooth adapter ("<<adapterAddy<<"): "<<error->message<<endl;
				g_error_free(error);
				return "";
			}

			error = NULL;
		}

		else
		{
			if(!org_bluez_manager_call_default_adapter_sync(manager,&adapterPath, NULL, &error))
			{
				DebugOut(0)<<"Error getting bluetooth default adapter: "<<error->message<<endl;
				g_error_free(error);
				return "";
			}

			error = NULL;
		}

		OrgBluezAdapter* adapter = org_bluez_adapter_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM,
																				 G_DBUS_PROXY_FLAGS_NONE,
																				 "org.bluez",adapterPath,NULL,&error);
		if(!adapter)
		{
			DebugOut(0)<<"Error getting bluetooth adapter proxy: "<<error->message<<endl;
			g_error_free(error);
			return "";
		}

		error = NULL;

		gchar* devicePath;
		if(!org_bluez_adapter_call_find_device_sync(adapter,address.c_str(),&devicePath,NULL,&error) ||
				std::string(devicePath) == "")
		{
			DebugOut(0)<<"Error finding bluetooth device: "<<address<<error->message<<endl;
			g_error_free(error);
			return "";
		}

		error = NULL;

		OrgBluezSerial* serialDevice = org_bluez_serial_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM,
																					G_DBUS_PROXY_FLAGS_NONE,
																					"org.bluez",devicePath,NULL,&error);

		if(!serialDevice)
		{
			DebugOut(0)<<"Error getting bluetooth serial device proxy: "<<error->message<<endl;
			g_error_free(error);
			return "";
		}

		gchar* serialDeviceName;
		if(!org_bluez_serial_call_connect_sync(serialDevice,"spp",&serialDeviceName,NULL,&error))
		{
			DebugOut(0)<<"Error connecting bluetooth serial device: "<<address<<" - "<<error->message<<endl;
			g_error_free(error);
			return "";
		}

		return serialDeviceName;
	}

	void disconnect(std::string address, std::string adapterAddy = "")
	{
		GError* error = NULL;
		OrgBluezManager* manager = org_bluez_manager_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM,
																				 G_DBUS_PROXY_FLAGS_NONE,
																				 "org.bluez","/",NULL, &error);

		if(!manager)
		{
			DebugOut(0)<<"Error getting bluetooth manager proxy: "<<error->message<<endl;
			g_error_free(error);
			return ;
		}

		error = NULL;

		gchar* adapterPath;

		if(adapterAddy != "")
		{
			if(!org_bluez_manager_call_find_adapter_sync(manager,adapterAddy.c_str(), &adapterPath, NULL, &error))
			{
				DebugOut(0)<<"Error getting bluetooth adapter ("<<adapterAddy<<"): "<<error->message<<endl;
				g_error_free(error);
				return ;
			}

			error = NULL;
		}

		else
		{
			if(!org_bluez_manager_call_default_adapter_sync(manager,&adapterPath, NULL, &error))
			{
				DebugOut(0)<<"Error getting bluetooth default adapter: "<<error->message<<endl;
				g_error_free(error);
				return ;
			}

			error = NULL;
		}

		OrgBluezAdapter* adapter = org_bluez_adapter_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM,
																				 G_DBUS_PROXY_FLAGS_NONE,
																				 "org.bluez",adapterPath,NULL,&error);
		if(!adapter)
		{
			DebugOut(0)<<"Error getting bluetooth adapter proxy: "<<error->message<<endl;
			g_error_free(error);
			return ;
		}

		error = NULL;

		gchar* devicePath;
		if(!org_bluez_adapter_call_find_device_sync(adapter,address.c_str(),&devicePath,NULL,&error) ||
				std::string(devicePath) == "")
		{
			DebugOut(0)<<"Error finding bluetooth device: "<<address<<error->message<<endl;
			g_error_free(error);
			return ;
		}

		error = NULL;

		OrgBluezSerial* serialDevice = org_bluez_serial_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM,
																					G_DBUS_PROXY_FLAGS_NONE,
																					"org.bluez",devicePath,NULL,&error);

		if(!serialDevice)
		{
			DebugOut(0)<<"Error getting bluetooth serial device proxy: "<<error->message<<endl;
			g_error_free(error);
			return ;
		}

		gchar* serialDeviceName;
		if(!org_bluez_serial_call_disconnect_sync(serialDevice,"spp",NULL,&error))
		{
			DebugOut(0)<<"Error disconnecting bluetooth serial device: "<<address<<" - "<<error->message<<endl;
			g_error_free(error);
			return ;
		}
	}

};


#endif
