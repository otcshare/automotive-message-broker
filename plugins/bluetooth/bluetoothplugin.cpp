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
#include "timestamp.h"
#include "listplusplus.h"

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

bool readCallback(GIOChannel *source, GIOCondition condition, gpointer data)
{
//	DebugOut(5) << "Polling..." << condition << endl;

	if(condition & G_IO_ERR)
	{
		DebugOut(DebugOut::Error)<<"GpsNmeaSource polling error."<<endl;
	}

	if (condition & G_IO_HUP)
	{
		//Hang up. Returning false closes out the GIOChannel.
		//printf("Callback on G_IO_HUP\n");
		DebugOut(DebugOut::Warning)<<"socket hangup event..."<<endl;
		return false;
	}

	BluetoothSinkPlugin* p = static_cast<BluetoothSinkPlugin*>(data);

	p->canHasData();

	return true;
}

BluetoothSinkPlugin::BluetoothSinkPlugin(AbstractRoutingEngine* re, map<string, string> config)
:AbstractSink(re, config)
{

}

extern "C" AbstractSink * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new BluetoothSinkPlugin(routingengine, config);

}

const string BluetoothSinkPlugin::uuid()
{
	return "47ec4ed0-dc45-11e3-9c1a-0800200c9a66";
}

void BluetoothSinkPlugin::propertyChanged(AbstractPropertyType *value)
{

}

void BluetoothSinkPlugin::dataReceived(QByteArray data)
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

void AbstractBluetoothSerialProfile::release()
{
	DebugOut()<<"release called."<<endl;
}

void AbstractBluetoothSerialProfile::newConnection(string path, QDBusUnixFileDescriptor fd, QVariantMap props)
{
	DebugOut()<<"new Connection! Path: "<<path<<" fd: "<<fd.fileDescriptor()<<endl;

	socket.setDescriptor(fd.fileDescriptor());
	socket.write("ping");

	GIOChannel *chan = g_io_channel_unix_new(socket.fileDescriptor());
	g_io_add_watch(chan, GIOCondition(G_IO_IN | G_IO_HUP | G_IO_ERR),(GIOFunc)readCallback, this);
	g_io_channel_set_close_on_unref(chan, true);
	g_io_channel_unref(chan);
}

void AbstractBluetoothSerialProfile::requestDisconnection(string path)
{
	DebugOut()<<"requestDisconnection called.  Path: "<<path<<endl;
	socket.close();
}

void AbstractBluetoothSerialProfile::canHasData()
{
	QByteArray data = socket.read().c_str();

	DebugOut()<<"data read: "<<data.constData()<<endl;

	dataReceived(data);
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
