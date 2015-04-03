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
#include <jsonprotocol.h>

#include <string>

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>
#include <QVariantMap>
#include <QTcpSocket>
#include <QDBusUnixFileDescriptor>

class AbstractBluetoothSerialProfile;

class BtProfileAdaptor : public QDBusAbstractAdaptor
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.bluez.Profile1")

public:

	BtProfileAdaptor(AbstractBluetoothSerialProfile* parent);

public Q_SLOTS:

	void Release();

	void NewConnection(QDBusObjectPath device, QDBusUnixFileDescriptor fd, QVariantMap fd_properties);

	void RequestDisconnection(QDBusObjectPath device);

private:

	AbstractBluetoothSerialProfile* mParent;

};

class AbstractBluetoothSerialProfile: public QObject
{
	Q_OBJECT

public:
	AbstractBluetoothSerialProfile(QString role = "server");

	virtual void connect(std::string hwaddy);

	virtual void release();

	virtual void newConnection(std::string path, QDBusUnixFileDescriptor fd, QVariantMap props);

	virtual void requestDisconnection(std::string path);

protected:
	virtual void connected() {}
	virtual void disconnected() {}

private:
	QString role;
};


class BluetoothSinkPlugin: public AbstractBluetoothSerialProfile, public AbstractSink
{
	Q_OBJECT

public:
	BluetoothSinkPlugin(AbstractRoutingEngine* re, map<string, string> config);

	const string uuid();

	void supportedChanged(const PropertyList &) {}

	void propertyChanged(AbstractPropertyType* value);

private:
	std::vector<amb::AmbRemoteServer::Ptr> clients;

	// AbstractBluetoothSerialProfile interface
public:
	void newConnection(string path, QDBusUnixFileDescriptor fd, QVariantMap props);
	void requestDisconnection(string path);
};



#endif // EXAMPLEPLUGIN_H
