#include <bluetooth.hpp>
#include <debugout.h>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QStringList>
#include <QDebug>

#include "bluetoothplugin.h"

class Bluetooth5: public AbstractBluetoothSerialProfile
{
public:
	Bluetooth5(): AbstractBluetoothSerialProfile("client") {}
	void dataReceived(QByteArray d)
	{
		qDebug()<<"data: "<< d;
	}
};

int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);

	QString addy = app.arguments().at(1);

	DebugOut::setDebugThreshhold(6);

	qDebug()<<"connecting to: "<<addy;

	Bluetooth5 client;

	app.exec();

	return 1;
}
