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

class BMDBus : public QObject
{
	Q_OBJECT
	Q_ENUMS(Connection)

public:

	enum Connection{
		System,
		Session
	};

	BMDBus(QObject * parent = nullptr);

public Q_SLOTS:

	QObject* createInterface(const QString & service, const QString & path, const QString & interface, Connection bus);

	bool exportObject(const QString & path, const QString & interface, Connection bus, QObject *obj);

private: ///methods:


};


#endif
