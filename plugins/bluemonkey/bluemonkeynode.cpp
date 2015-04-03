#include "bluemonkey.h"

#include <QCoreApplication>
#include <QtDebug>

int main(int argc, char ** argv)
{
	QCoreApplication app(argc, argv);
	if(argc < 2)
	{
		qCritical() << "usage: bluemonkey <path/to/script>";
		return -1;
	}

	Bluemonkey bluemonkey;

	bluemonkey.setArguments(argc, argv);
	QString path = bluemonkey.arguments().at(1);
	bluemonkey.loadScript(path);

	return 0;
}
