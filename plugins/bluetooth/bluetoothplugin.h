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

#ifndef BT_PLUGIN_H_
#define BT_PLUGIN_H_

#include <abstractsink.h>
#include <serialport.hpp>
#include <string>

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>
#include <QVariantMap>
#include <QTcpSocket>
#include <QDBusUnixFileDescriptor>

class BluetoothSinkPlugin;

class BtProfileAdaptor : public QDBusAbstractAdaptor
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.bluez.Profile1")

public:

	BtProfileAdaptor(BluetoothSinkPlugin* parent);

public Q_SLOTS:

	void Release();

	void NewConnection(QDBusObjectPath device, QDBusUnixFileDescriptor fd, QVariantMap fd_properties);

	void RequestDisconnection(QDBusObjectPath device);

private:

	BluetoothSinkPlugin* mParent;

};

class BluetoothSinkPlugin: public QObject, public AbstractSink
{
Q_OBJECT

public:
	BluetoothSinkPlugin(AbstractRoutingEngine* re, map<string, string> config);
	
	const string uuid();

	void supportedChanged(const PropertyList &) {}

	void propertyChanged(AbstractPropertyType* value);

	void release();

	void newConnection(std::string path, QDBusUnixFileDescriptor fd, QVariantMap props);

	void requestDisconnection(std::string path);

	void canHasData();


private:
	 SerialPort socket;
};



#endif // EXAMPLEPLUGIN_H
