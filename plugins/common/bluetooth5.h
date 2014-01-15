#ifndef BLUETOOTH_5_H_
#define BLUETOOOTH_5_H_

#include <string>

class Bluetooth5
{
public:
	std::string getDeviceForAddress(std::string address, std::string adapterAddy = "");
	void disconnect(std::string address, std::string adapterAddy = "");
};

#endif
