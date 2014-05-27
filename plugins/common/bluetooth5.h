#ifndef BLUETOOTH_5_H_
#define BLUETOOOTH_5_H_

#include <string>
#include <functional>

typedef std::function<void(int)> ConnectedCallback;

class Bluetooth5
{
public:
	Bluetooth5();

	void getDeviceForAddress(std::string address,  ConnectedCallback onnectedCallback);
	void disconnect(std::string address, std::string adapterAddy = "");

	void connected(int fd);

private:
	ConnectedCallback mConnected;
};

#endif
