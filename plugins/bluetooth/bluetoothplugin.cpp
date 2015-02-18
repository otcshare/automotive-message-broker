/*
Copyright (C) 2012 Intel Corporation

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "bluetoothplugin.h"

#include <timestamp.h>
#include <listplusplus.h>
#include <bluetooth5.h>

#include <iostream>
#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <glib.h>

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QSocketNotifier>

using namespace std;

#include "debugout.h"


BluetoothSinkPlugin::BluetoothSinkPlugin(AbstractRoutingEngine* re, map<string, string> config)
:AbstractSink(re, config)
{

}

extern "C" void create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	new BluetoothSinkPlugin(routingengine, config);
}

const string BluetoothSinkPlugin::uuid()
{
	return "47ec4ed0-dc45-11e3-9c1a-0800200c9a66";
}

void BluetoothSinkPlugin::propertyChanged(AbstractPropertyType *value)
{

}

void BluetoothSinkPlugin::newConnection(string, QDBusUnixFileDescriptor fd, QVariantMap)
{
	SerialPort *bluetoothDev = new SerialPort();
	bluetoothDev->setDescriptor(fd.fileDescriptor());
	auto client = amb::make_shared(new amb::AmbRemoteServer(bluetoothDev, routingEngine));
	client->disconnected = [this, client]() {
		removeOne(&clients, client);
	};

	clients.push_back(client);
}

void BluetoothSinkPlugin::requestDisconnection(string path)
{

}

AbstractBluetoothSerialProfile::AbstractBluetoothSerialProfile(QString r)
	:role(r)
{
	new BtProfileAdaptor(this);

	if(!QDBusConnection::systemBus().registerService("org.automotive.message.broker.bluetooth"))
		DebugOut(DebugOut::Error)<<"Failed to register DBus service name: "<<QDBusConnection::systemBus().lastError().message().toStdString()<<endl;

	if(!QDBusConnection::systemBus().registerObject("/org/bluez/spp", this))
		DebugOut(DebugOut::Error)<<"Failed to register DBus object"<<endl;

	QDBusInterface profileManagerIface("org.bluez", "/org/bluez", "org.bluez.ProfileManager1", QDBusConnection::systemBus());

	QVariantMap options;
	options["Name"] = "AMB spp";
	options["Role"] = role;
	options["Channel"] = qVariantFromValue(uint16_t(23));
	options["AutoConnect"] = true;

	QDBusReply<void> reply = profileManagerIface.call("RegisterProfile", qVariantFromValue(QDBusObjectPath("/org/bluez/spp")), "00001101-0000-1000-8000-00805F9B34FB", options);

	if(!reply.isValid())
	{
		DebugOut(DebugOut::Error)<<"RegisterProfile call failed: "<<reply.error().message().toStdString()<<endl;
	}

}

void AbstractBluetoothSerialProfile::connect(string hwaddy)
{
	std::string device = findDevice(hwaddy);

	if(device.empty())
	{
		DebugOut(DebugOut::Error) << "could not find device with address: " << hwaddy << endl;
		return;
	}

	QDBusInterface deviceManager("org.bluez", device.c_str(), "org.bluez.Device1", QDBusConnection::systemBus());

	if(!deviceManager.isValid())
	{
		DebugOut(DebugOut::Error) << "could not create DBus interface for device with address '"
								  << hwaddy <<"' " << deviceManager.lastError().message().toStdString() << endl;
		return;
	}

	QDBusPendingCall reply = deviceManager.asyncCall("Connect");

	QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);

	QObject::connect(watcher, &QDBusPendingCallWatcher::finished, [](auto call)
	{
		QDBusPendingReply<void> reply = *call;
		if(reply.isError())
		{
			DebugOut(DebugOut::Error) << "Connecting: " << reply.error().message().toStdString() << endl;
		}
	});

}

void AbstractBluetoothSerialProfile::release()
{
	DebugOut()<<"release called."<<endl;
}

void AbstractBluetoothSerialProfile::newConnection(string path, QDBusUnixFileDescriptor fd, QVariantMap props)
{
	DebugOut()<<"new Connection! Path: "<<path<<" fd: "<<fd.fileDescriptor()<<endl;
}

void AbstractBluetoothSerialProfile::requestDisconnection(string path)
{
	DebugOut()<<"requestDisconnection called.  Path: "<<path<<endl;
}

BtProfileAdaptor::BtProfileAdaptor(AbstractBluetoothSerialProfile *parent)
	:QDBusAbstractAdaptor(parent), mParent(parent)
{

}

void BtProfileAdaptor::Release()
{
	mParent->release();
}

void BtProfileAdaptor::NewConnection(QDBusObjectPath device, QDBusUnixFileDescriptor fd, QVariantMap fd_properties)
{
	mParent->newConnection(device.path().toStdString(), fd, fd_properties);
}

void BtProfileAdaptor::RequestDisconnection(QDBusObjectPath device)
{
	mParent->requestDisconnection(device.path().toStdString());
}
