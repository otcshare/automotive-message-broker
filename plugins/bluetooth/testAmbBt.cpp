#include <bluetooth.hpp>
#include <debugout.h>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QStringList>
#include <QSocketNotifier>

#include "bluetooth5.h"
#include <serialport.hpp>
#include <QSocketNotifier>

int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);

	QString addy = app.arguments().at(1);

	Bluetooth5 btdev;
	btdev.getDeviceForAddress(addy.toStdString(),[](int fd){
		DebugOut(0)<<"I am connected"<<endl;

		SerialPort *s = new SerialPort();
		s->setDescriptor(fd);

		QSocketNotifier* sock = new QSocketNotifier(fd, QSocketNotifier::Read);

		QObject::connect(sock, &QSocketNotifier::activated, [&s](){
			DebugOut(0)<<"data: "<<s->read()<<endl;
		});



	});

	return app.exec();
}
