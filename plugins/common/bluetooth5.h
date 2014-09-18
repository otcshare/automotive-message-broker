#ifndef BLUETOOTH_5_H_
#define BLUETOOOTH_5_H_

#include <string>
#include <functional>

typedef std::function<void(int)> ConnectedCallback;

class Bluetooth5
{
public:
	Bluetooth5();

	bool setDevice(std::string address);
	void getDeviceForAddress(std::string address,  ConnectedCallback onnectedCallback);

	void connected_(int fd);

	void connect(ConnectedCallback onconnectedCallback);
	void disconnect();


private:
	ConnectedCallback mConnected;
	std::string mPath;
};

#endif
