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

#include <debugout.h>

#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QCoreApplication>
#include <QMetaMethod>

#include "bmdbus_p.h"

extern "C" std::map<std::string, QObject*> create(std::map<std::string, std::string> config, std::map<std::string, QObject*> &exports, QString &js, QObject* parent)
{

	exports["dbus"] = new BMDBus(parent);
	exports["testExport"] = new TestQObject(parent);

	js += "dbus.defineMethodSignature = function(obj, methodName, retType, args)"
		  "{"
		  "  metadata = obj['_metadata'];"
		  "  if(!metadata)"
		  "    metadata = new Object;"
		  "  metadata[methodName] = {'retType' : retType, 'signature' : args};"
		  "  obj['_metadata'] = metadata;"
		  "}";

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
	DebugOut(DebugOut::Warning) << "BMDBus::exportObject() interface is ignored in qt 5.4 and lower" << endl;
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



BluemonkeyQObjectPrivate::BluemonkeyQObjectPrivate(BluemonkeyQObject *iface)
{

}

QVariant BluemonkeyQObjectPrivate::property(const QByteArray &property)
{

}

bool BluemonkeyQObjectPrivate::setProperty(const QByteArray &property, const QVariant &value)
{

}

void BluemonkeyQObjectPrivate::createFrom(const QJSValue &value)
{
	if(!value.isObject() || !value.hasProperty("_metadata"))
	{
		DebugOut(DebugOut::Warning) << "No metadata or not object" << endl;
		return;
	}

	QVariantMap metaData = value.property("_metadata").toVariant().toMap();

	Q_FOREACH(QString key, metaData.keys())
	{
		QString methodName = key;
		QVariantMap metaMethod = metaData[key].toMap();

		QStringList args = metaMethod["args"].toStringList();
		QString returnType = metaMethod["retType"].toString();
		metaObject->addMethod();
	}

}


BluemonkeyMetaObject::BluemonkeyMetaObject(BluemonkeyQObject *qq, BluemonkeyQObjectPrivate *dd, const QMetaObject *mo)
	:q(qq), d(dd)
{
	m_builder.setSuperClass(mo);
	m_builder.setClassName(mo->className());
	m_builder.setFlags(QMetaObjectBuilder::DynamicMetaObject);

	m_metaObject = m_builder.toMetaObject();

	QObjectPrivate* op = QObjectPrivate::get(q);
	m_parent = static_cast<QAbstractDynamicMetaObject*>(op->metaObject);
	*static_cast<QMetaObject*>(this) = *m_metaObject;
	op->metaObject = this;
}

BluemonkeyMetaObject::~BluemonkeyMetaObject()
{
	delete m_metaObject;
}

void BluemonkeyMetaObject::updateProperties(const QVariantMap &data)
{
	Q_FOREACH(QString key, data.keys())
	{
		int i = m_propertyIdLookup.value(key.toUtf8());
		activate(q, i + m_metaObject->methodOffset(), 0);
	}
}

void BluemonkeyMetaObject::addProperty(const QByteArray& name, const QJSValue& property)
{
	int id = m_builder.propertyCount();
	m_builder.addSignal("__"+QByteArray::number(id)+"()");
	m_propertyIdLookup.insert(name, id);
	QMetaType::Type propertyType = jsType(property);
	m_propertyTypeLookup.insert(name, propertyType);
}

void BluemonkeyMetaObject::addMethod(const QByteArray& name, const QString & returnType, const QStringList &args)
{
	if(!method.isCallable()) return;

	if(returnType.isEmpty())
		returnType = "void";


}

QMetaType::Type BluemonkeyMetaObject::jsType(const QJSValue &value)
{
	int type = 0;
	if(value.isArray())
	{
		type = qMetaTypeId<QVariantList>();
	}
	else if(value.isBool())
	{
		type = qMetaTypeId<bool>();
	}
	else if(value.isDate())
	{
		type = qMetaTypeId<QDateTime>();
	}
	else if(value.isNumber())
	{
		type = qMetaTypeId<double>();
	}
	else if(value.isObject())
	{
		type = qMetaTypeId<BluemonkeyQObject*>();
	}
	else if(value.isQObject())
	{
		type = qMetaTypeId<QObject*>();
	}
	else if(value.isString())
	{
		type = qMetaTypeId<QString>();
	}
	else if(value.isVariant())
	{
		type = qMetaTypeId<QVariant>();
	}
}

int BluemonkeyMetaObject::metaCall(QMetaObject::Call _c, int _id, void **_a)
{

}

int BluemonkeyMetaObject::createProperty(const char *, const char *)
{

}


BluemonkeyQObject::BluemonkeyQObject(QObject *parent)
	:QObject(*new BluemonkeyQObjectPrivate(this), parent)
{
	Q_D(BluemonkeyQObject);
	d->metaObject = new BluemonkeyMetaObject(this, d, metaObject());
}

int main(int argc, char** argv)
{
	DebugOut::setDebugThreshhold(7);

	DebugOut() << "Started BMDBus test " PROJECT_VERSION << endl;

	QCoreApplication app(argc, argv);

	BMDBus dbus;

	dbus.registerService("org.bluemonkey.test", BMDBus::Session);
/*	dbus.exportObject("/testQObject", "org.awesome.interface", BMDBus::Session, new TestQObject(&dbus));
	dbus.exportObject("/bareQObject", "org.awesome.interface2", BMDBus::Session, new BareQObject());

	QDBusInterface* iface = static_cast<QDBusInterface*>(dbus.createInterface("org.bluemonkey.test", "/testQObject", "org.awesome.interface", BMDBus::Session));

	QDBusReply<QString> reply = iface->call("awesomeMethod");

	DebugOut() << "Reply value: " << reply.value().toStdString() << endl;
*/
	app.exec();
}
