#include "ambqt.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QtDebug>

AmbProperty::AmbProperty(QString objectPath, QString interface, QString propertyName)
	:QObject(),mPropertyName(propertyName)
{

	mInterface = new QDBusInterface("org.automotive.message.broker",objectPath, interface,QDBusConnection::systemBus(),this);

	if(!QDBusConnection::systemBus().connect("org.automotive.message.broker", objectPath, interface, propertyName, this, SIGNAL(propertyChanged(QVariant))))
	{
		qDebug()<<"Failed to connect to signal";
		qDebug()<<"path: "<<objectPath;
		qDebug()<<"interface: "<<interface;
		qDebug()<<"signal: "<<propertyName;
		qDebug()<<"Error: "<<QDBusConnection::systemBus().lastError().message();
	}

}
