#include <bluetooth.hpp>
#include <debugout.h>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QStringList>
#include <QDebug>

#include "bluetooth5.h"
#include <serialport.hpp>

SerialPort *s = new SerialPort();

bool readCallback(GIOChannel *source, GIOCondition condition, gpointer data)
{
//	DebugOut(5) << "Polling..." << condition << endl;

	if(condition & G_IO_ERR)
	{
		DebugOut(DebugOut::Error)<<"GpsNmeaSource polling error."<<endl;
	}

	if (condition & G_IO_HUP)
	{
		//Hang up. Returning false closes out the GIOChannel.
		//printf("Callback on G_IO_HUP\n");
		DebugOut(DebugOut::Warning)<<"socket hangup event..."<<endl;
		return false;
	}

	DebugOut(0)<<"Data: "<<s->read();

	return true;
}

int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);

	QString addy = app.arguments().at(1);

	DebugOut::setDebugThreshhold(6);

	qDebug()<<"connecting to: "<<addy;

	Bluetooth5 btdev;
	btdev.getDeviceForAddress(addy.toStdString(),[](int fd)
	{
		qDebug()<<"I am connected "<<fd;
		s->setDescriptor(fd);

		GIOChannel *chan = g_io_channel_unix_new(s->fileDescriptor());
		g_io_add_watch(chan, GIOCondition(G_IO_IN | G_IO_HUP | G_IO_ERR),(GIOFunc)readCallback, nullptr);
		g_io_channel_set_close_on_unref(chan, true);
		g_io_channel_unref(chan);

		s->write("ping\r\n");
	});

	app.exec();

	s->close();

	return 1;
}
