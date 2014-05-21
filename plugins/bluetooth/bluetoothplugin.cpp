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

QString SPP_RECORD = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
														   "<record>"
															 "<attribute id=\"0x0001\">"
															   "<sequence>"
																 "<uuid value=\"0x1101\"/>"
															   "</sequence>"
															 "</attribute>"
															 "<attribute id=\"0x0004\">"
															   "<sequence>"
																 "<sequence>"
																   "<uuid value=\"0x0100\"/>"
																 "</sequence>"
																 "<sequence>"
																   "<uuid value=\"0x0003\"/>"
																   "<uint8 value=\"23\" name=\"channel\"/>"
																 "</sequence>"
															   "</sequence>"
															 "</attribute>"
															 "<attribute id=\"0x0100\">"
															   "<text value=\"COM5\" name=\"name\"/>"
															 "</attribute>"
														   "</record>";

BluetoothSinkPlugin::BluetoothSinkPlugin(AbstractRoutingEngine* re, map<string, string> config)
:AbstractSink(re, config)
{
	new BtProfileAdaptor(this);

	if(!QDBusConnection::systemBus().registerService("org.automotive.message.broker.bluetooth"))
		DebugOut(DebugOut::Error)<<"Failed to register DBus service name: "<<QDBusConnection::systemBus().lastError().message().toStdString()<<endl;

	if(!QDBusConnection::systemBus().registerObject("/org/bluez/spp", this))
		DebugOut(DebugOut::Error)<<"Failed to register DBus object"<<endl;

	QDBusInterface profileManagerIface("org.bluez", "/org/bluez", "org.bluez.ProfileManager1", QDBusConnection::systemBus());

	QVariantMap options;
	options["Name"] = "AMB spp server";
	options["Role"] = "server";
	options["ServiceRecord"] = SPP_RECORD;
	options["Channel"] = qVariantFromValue(uint16_t(23));

	QDBusReply<void> reply = profileManagerIface.call("RegisterProfile", qVariantFromValue(QDBusObjectPath("/org/bluez/spp")), "00001101-0000-1000-8000-00805F9B34FB", options);

	if(!reply.isValid())
	{
		DebugOut(DebugOut::Error)<<"RegisterProfile call failed: "<<reply.error().message().toStdString()<<endl;
	}
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

void BluetoothSinkPlugin::release()
{
	DebugOut()<<"release called."<<endl;
}

void BluetoothSinkPlugin::newConnection(string path, int fd, QVariantMap props)
{
	DebugOut()<<"new Connection! Path: "<<path<<endl;

	socket.setDescriptor(fd);

	QSocketNotifier *notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);

	connect(notifier,&QSocketNotifier::activated,this, &BluetoothSinkPlugin::canHasData);
}

void BluetoothSinkPlugin::requestDisconnection(string path)
{
	DebugOut()<<"requestDisconnection called.  Path: "<<path<<endl;
	socket.close();
}

void BluetoothSinkPlugin::canHasData()
{
	QByteArray data = socket.read().c_str();

	DebugOut()<<"data read: "<<data.constData()<<endl;
}

BtProfileAdaptor::BtProfileAdaptor(BluetoothSinkPlugin *parent)
	:QDBusAbstractAdaptor(parent), mParent(parent)
{

}

void BtProfileAdaptor::Release()
{
	mParent->release();
}

void BtProfileAdaptor::NewConnection(QDBusObjectPath device, int fd, QVariantMap fd_properties)
{
	mParent->newConnection(device.path().toStdString(), fd, fd_properties);
}

void BtProfileAdaptor::RequestDisconnection(QDBusObjectPath device)
{
	mParent->requestDisconnection(device.path().toStdString());
}
