#include "ambqt.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QtDebug>

AmbProperty::AmbProperty(QString op, QString iface, QString propName)
	:QObject(), mPropertyName(propName),mInterfaceName(iface), mObjectPath(op),mDBusInterface(NULL)
{
	connect();
}


void AmbProperty::propertyChangedSlot(QDBusVariant val, double ts)
{
	valueChanged(val.variant());
	propertyChanged(val.variant(), ts);
}

void AmbProperty::connect()
{
	if(mDBusInterface)
	{
		delete mDBusInterface;
	}
	mDBusInterface = new QDBusInterface("org.automotive.message.broker",objectPath(), interfaceName(), QDBusConnection::systemBus(),this);

	if(!mDBusInterface->isValid())
	{
		qDebug()<<"Failed to create dbus interface for property "<<propertyName();
		qDebug()<<"Path: "<<objectPath();
		qDebug()<<"Interface: "<<interfaceName();
		qDebug()<<"Error: "<<QDBusConnection::systemBus().lastError().message();
	}

	QString signalName = propertyName() + "Changed";


	if(!QDBusConnection::systemBus().connect("org.automotive.message.broker", objectPath(), interfaceName(), signalName, this, SLOT(propertyChangedSlot(QDBusVariant,double))))
	{
		qDebug()<<"Failed to connect to signal";
		qDebug()<<"path: "<<objectPath();
		qDebug()<<"interface: "<<interfaceName();
		qDebug()<<"signal: "<<propertyName();
		qDebug()<<"Error: "<<QDBusConnection::systemBus().lastError().message();
	}
}
