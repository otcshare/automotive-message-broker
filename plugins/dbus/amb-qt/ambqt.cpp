#include "ambqt.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QtDebug>

AmbProperty::AmbProperty(QString op, QString iface, QString propName)
	:QObject(), mProperty(propName),mInterface(iface), mObjectPath(op)
{
	connect();
}


void AmbProperty::propertyChangedSlot(QDBusVariant val, double ts)
{
	propertyChanged(val.variant(), ts);
}

void AmbProperty::connect()
{
	if(mDBusInterface)
	{
		delete mDBusInterface;
	}
	mDBusInterface = new QDBusInterface("org.automotive.message.broker",objectPath(), interface(), QDBusConnection::systemBus(),this);

	if(!mDBusInterface->isValid())
	{
		qDebug()<<"Failed to create dbus interface for property "<<property();
		qDebug()<<"Path: "<<objectPath();
		qDebug()<<"Interface: "<<interface();
		qDebug()<<"Error: "<<QDBusConnection::systemBus().lastError().message();
	}

	QString signalName = property() + "Changed";


	if(!QDBusConnection::systemBus().connect("org.automotive.message.broker", objectPath(), interface(), signalName, this, SLOT(propertyChangedSlot(QDBusVariant,double))))
	{
		qDebug()<<"Failed to connect to signal";
		qDebug()<<"path: "<<objectPath();
		qDebug()<<"interface: "<<interface();
		qDebug()<<"signal: "<<property();
		qDebug()<<"Error: "<<QDBusConnection::systemBus().lastError().message();
	}
}
