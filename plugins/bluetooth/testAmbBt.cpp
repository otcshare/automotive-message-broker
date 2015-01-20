#include <bluetooth5.h>
#include <debugout.h>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QStringList>
#include <QDebug>

#include "bluetoothplugin.h"

class BluetoothSerialClient: public AbstractBluetoothSerialProfile
{
public:
	BluetoothSerialClient(): AbstractBluetoothSerialProfile("client") {}

	void connected()
	{

	}

	void disconnected()
	{

	}

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

	BluetoothSerialClient client;

	client.connect(addy.toStdString());

	app.exec();

	return 1;
}
