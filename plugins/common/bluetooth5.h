#ifndef BLUETOOTH_5_H_
#define BLUETOOOTH_5_H_

#include <string>
#include <functional>
#include <superptr.hpp>

typedef std::function<void(int)> ConnectedCallback;

std::string findDevice(std::string address, std::string adapterPath="");

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
	amb::super_ptr<GDBusConnection> mConnection;
	std::string mPath;
};

#endif
