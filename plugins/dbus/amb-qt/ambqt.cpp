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
	}

	QString signalName = "2" + propertyName.toLatin1() + "(QVariant)";

/*	if(!connect(mInterface, signalName.toAscii().data(), this, SIGNAL(propertyChanged(QVariant))))
	{
		qDebug()<<"fail fail failed2 to connect to signal";
	}*/


	if(!QDBusConnection::systemBus().connect("org.automotive.message.broker", objectPath, interface, propertyName, this, SLOT(propertyChangedSlot(QDBusVariant))))
	{
		qDebug()<<"Failed to connect to signal";
		qDebug()<<"path: "<<objectPath;
		qDebug()<<"interface: "<<interface;
		qDebug()<<"signal: "<<propertyName;
		qDebug()<<"Error: "<<QDBusConnection::systemBus().lastError().message();
	}

}


void AmbProperty::propertyChangedSlot(QDBusVariant val)
{
	propertyChanged(val.variant());
}
