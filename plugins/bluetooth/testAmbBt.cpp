#include <bluetooth.hpp>
#include <debugout.h>
#include <QFile>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QStringList>

int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);

	QString addy = app.arguments().at(1);

	BluetoothDevice dev;
	std::string devPath = dev.getDeviceForAddress(addy.toStdString());

	QFile f(devPath.c_str());
	f.open(QIODevice::ReadWrite);

	QObject::connect(&f, &QFile::readyRead, [&f](){
		QByteArray d = f.readAll();

		DebugOut(DebugOut::Warning)<<"Data read: "<<d.constData()<<endl;
	});


	return app.exec();
}
