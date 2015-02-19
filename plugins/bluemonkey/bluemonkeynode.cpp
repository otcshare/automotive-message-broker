#include "bluemonkey.h"

#include <QCoreApplication>
#include <QtDebug>

int main(int argc, char ** argv)
{
	QCoreApplication app(argc, argv);

	if(app.arguments().count() < 2)
	{
		qDebug() << "Error: must run with path to file.js";
		qDebug() << "ie: " << app.applicationName() << " /path/to/file.js";
		return -1;
	}

	std::string path = app.arguments().at(1).toStdString();

	std::map<std::string, std::string> config;

	config["config"] = path;

	Bluemonkey bluemonkey(config);

	app.exec();
}
