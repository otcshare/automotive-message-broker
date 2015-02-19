/*
	Copyright (C) 2012  Intel Corporation

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

#include "bmdbus.h"

#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QCoreApplication>
#include <QtDebug>
extern "C" void create(std::map<std::string, std::string> config, std::map<std::string, QObject*> &exports, QString &js, QObject* parent)
{
	exports["dbus"] = new BMDBus(parent);
	exports["testExport"] = new TestQObject(parent);

	js.append("dbus.defineMethodSignature = function(obj, methodName, retType, args)"
		  "{"
		  "  metadata = obj['_metadata'];"
		  "  if(!metadata)"
		  "    metadata = new Object;"
		  "  metadata[methodName] = {'retType' : retType, 'signature' : args};"
		  "  obj['_metadata'] = metadata;"
		  "}");

}

BMDBus::BMDBus(QObject *parent)
	:QObject(parent)
{

}

QObject *BMDBus::createInterface(const QString &service, const QString &path, const QString &interface, BMDBus::Connection bus)
{
	return new QDBusInterface(service, path, interface, getConnection(bus), this);
}

bool BMDBus::registerService(const QString &service, BMDBus::Connection bus)
{
	QDBusConnection con = getConnection(bus);

	return con.registerService(service);
}

bool BMDBus::unregisterService(const QString &service, BMDBus::Connection bus)
{
	QDBusConnection con = getConnection(bus);

	return con.unregisterService(service);
}


bool BMDBus::exportObject(const QString &path, const QString &interface, BMDBus::Connection bus, const QJSValue &object)
{
	if(!object.isObject())
		return false;
/*
	QDBusConnection con = getConnection(bus);
/// TODO: should be qt 5.5:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 1))
	return con.registerObject(path, interface, object, QDBusConnection::ExportAllContents);
#else
	qDebug() << "BMDBus::exportObject() interface is ignored in qt 5.4 and lower" ;
	return con.registerObject(path, object, QDBusConnection::ExportAllContents);
#endif
*/
}

QString BMDBus::errorMessage(const BMDBus::Connection bus)
{
	QDBusConnection con = getConnection(bus);

	QDBusError err = con.lastError();

	return err.name() + "(" +QString::number(err.type()) + ") : " + err.message();
}

QDBusConnection BMDBus::getConnection(const BMDBus::Connection bus)
{
	QDBusConnection con = bus == BMDBus::Session ? QDBusConnection::sessionBus() : QDBusConnection::systemBus();

	return con;
}

int main(int argc, char** argv)
{
	qDebug() << "Started BMDBus test " PROJECT_VERSION ;

	QCoreApplication app(argc, argv);

	BMDBus dbus;

	dbus.registerService("org.bluemonkey.test", BMDBus::Session);
/*	dbus.exportObject("/testQObject", "org.awesome.interface", BMDBus::Session, new TestQObject(&dbus));
	dbus.exportObject("/bareQObject", "org.awesome.interface2", BMDBus::Session, new BareQObject());

	QDBusInterface* iface = static_cast<QDBusInterface*>(dbus.createInterface("org.bluemonkey.test", "/testQObject", "org.awesome.interface", BMDBus::Session));

	QDBusReply<QString> reply = iface->call("awesomeMethod");

	qDebug() << "Reply value: " << reply.value().toStdString() ;
*/
	app.exec();
}
