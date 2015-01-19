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

extern "C" std::map<std::string, QObject*> create(std::map<std::string, std::string> config, QObject* parent)
{
	std::map<std::string, QObject*> moduleInstances;
	moduleInstances["dbus"] = new BMDBus(parent);
	return moduleInstances;
}

BMDBus::BMDBus(QObject *parent)
	:QObject(parent)
{

}

QObject *BMDBus::createInterface(const QString &service, const QString &path, const QString &interface, BMDBus::Connection bus)
{
	return new QDBusInterface(service, path, interface, bus == Session ? QDBusConnection::sessionBus() : QDBusConnection::systemBus(), this);
}
