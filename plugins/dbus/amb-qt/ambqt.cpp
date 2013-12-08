#include "ambqt.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QtDebug>
#include <QDBusReply>

AmbProperty::AmbProperty(QString op, QString iface, QString propName)
	:QObject(), mPropertyName(propName),mInterfaceName(iface), mObjectPath(op),mDBusInterface(NULL),mZone(0)
{
	connect();
}


void AmbProperty::propertyChangedSlot(QString, QVariantMap values, QVariantMap )
{
	valueChanged(values);
}

void AmbProperty::propertyChanged1(QDBusVariant val, double t)
{
	mTime = t;
	mValue = val.variant();

	signalChanged(mValue);
}

void AmbProperty::connect()
{
	if(mDBusInterface)
	{
		delete mDBusInterface;
	}

	if(mObjectPath.isEmpty())
		getObjectPath();

	if(mInterfaceName.isEmpty())
		mInterfaceName = "org.automotive."+mPropertyName;

	mDBusInterface = new QDBusInterface("org.automotive.message.broker",objectPath(), interfaceName(), QDBusConnection::systemBus(),this);

	if(!mDBusInterface->isValid())
	{
		qDebug()<<"Failed to create dbus interface for property "<<propertyName();
		qDebug()<<"Path: "<<objectPath();
		qDebug()<<"Interface: "<<interfaceName();
		qDebug()<<"Error: "<<QDBusConnection::systemBus().lastError().message();
	}

	QString signalName = propertyName() + "Changed";


	if(!QDBusConnection::systemBus().connect("org.automotive.message.broker", objectPath(), "org.freedesktop.DBus.Properties",
											 "PropertiesChanged", this, SLOT(propertyChangedSlot(QString,QVariantMap,QVariantMap))))
	{
		qDebug()<<"Failed to connect to signal";
		qDebug()<<"path: "<<objectPath();
		qDebug()<<"interface: "<<interfaceName();
		qDebug()<<"signal: "<<propertyName();
		qDebug()<<"Error: "<<QDBusConnection::systemBus().lastError().message();
	}

	///TODO: only use PropertiesChanged...  treat AmbProperty like an object rather than a representation of just a single property in the object

	if(!QDBusConnection::systemBus().connect("org.automotive.message.broker", objectPath(), mInterfaceName,
											 signalName, this, SLOT(propertyChanged1(QDBusVariant,double))))
	{
		qDebug()<<"Failed to connect to signal";
		qDebug()<<"path: "<<objectPath();
		qDebug()<<"interface: "<<interfaceName();
		qDebug()<<"signal: "<<propertyName();
		qDebug()<<"Error: "<<QDBusConnection::systemBus().lastError().message();
	}
}

void AmbProperty::getObjectPath()
{
	QDBusInterface managerIface("org.automotive.message.broker","/","org.automotive.Manager", QDBusConnection::systemBus(), this);

	if(!managerIface.isValid())
	{
		qDebug()<<"Failed to create manager interface";
		return;
	}

	QDBusReply<QDBusObjectPath> reply = managerIface.call("FindObjectForZone", mPropertyName, mZone);

	if(reply.isValid())
	{
		mObjectPath = reply.value().path();
	}
}
