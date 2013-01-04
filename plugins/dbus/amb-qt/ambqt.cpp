#include "ambqt.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QtDebug>

AmbProperty::AmbProperty(QString objectPath, QString interface, QString propertyName)
	:QObject(),mPropertyName(propertyName)
{

	mInterface = new QDBusInterface("org.automotive.message.broker",objectPath, interface,QDBusConnection::systemBus(),this);

	if(!mInterface->isValid())
	{
		qDebug()<<"Failed to create dbus interface for property "<<propertyName;
		qDebug()<<"Path: "<<objectPath;
		qDebug()<<"Interface: "<<interface;
		qDebug()<<"Error: "<<QDBusConnection::systemBus().lastError().message();
	}

	QString signalName = propertyName+"Changed";


	if(!QDBusConnection::systemBus().connect("org.automotive.message.broker", objectPath, interface, signalName, this, SLOT(propertyChangedSlot(QDBusVariant,double))))
	{
		qDebug()<<"Failed to connect to signal";
		qDebug()<<"path: "<<objectPath;
		qDebug()<<"interface: "<<interface;
		qDebug()<<"signal: "<<propertyName;
		qDebug()<<"Error: "<<QDBusConnection::systemBus().lastError().message();
	}

}


void AmbProperty::propertyChangedSlot(QDBusVariant val, double ts)
{
	propertyChanged(val.variant(), ts);
}
