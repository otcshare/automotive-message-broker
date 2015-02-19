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

#ifndef BM_DBUS_H_
#define BM_DBUS_H_

#include <QObject>
#include <QJSValue>
#include <QDBusConnection>

class BMDBus : public QObject
{
	Q_OBJECT
	Q_PROPERTY(int Session READ sessionBus)
	Q_PROPERTY(int System READ systemBus)

public:

	enum Connection{
		System=0,
		Session
	};

	Q_ENUMS(Connection)

	BMDBus(QObject * parent = nullptr);

	Connection sessionBus() { return BMDBus::Session; }
	Connection systemBus() { return BMDBus::System; }

public Q_SLOTS:

	QObject* createInterface(const QString & service, const QString & path, const QString & interface, Connection bus);

	bool registerService(const QString & service, Connection bus);

	bool unregisterService(const QString & service, Connection bus);

	bool exportObject(const QString & path, const QString & interface, Connection bus, const QJSValue &obj);

	QString errorMessage(const Connection bus = Session);

private: ///methods:

	QDBusConnection getConnection(const Connection bus);
};

class BareQObject : public QObject
{
	Q_OBJECT
public Q_SLOTS:
	void becauseImHappy() { }
};

class TestQObject : public QObject
{
	Q_OBJECT
public:
	TestQObject(QObject * parent = nullptr) : QObject(parent) {}
public Q_SLOTS:
	QString awesomeMethod() { return "awesome"; }
};

#endif
