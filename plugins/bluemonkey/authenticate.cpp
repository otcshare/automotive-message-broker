#include "authenticate.h"
#include <QSettings>
#include <QtScript>

Authenticate::Authenticate(std::map<std::string, std::string> config, QObject *parent) :
    QObject(parent)
{
	QSettings settings;
	authorized = settings.value("authorized").toStringList();

	QScriptEngine engine;

	QScriptValue eventEngineValue = engine.newQObject(this);
	engine.globalObject().setProperty("auth", eventEngineValue);

	QString str = config["authSettings"].c_str();

	QFile file(str);
	if(!file.open(QIODevice::ReadOnly))
	{
		qDebug()<<"failed to open config file: "<<str;
		return;
	}

	QString script = file.readAll();

	file.close();

	engine.evaluate(script);

}

bool Authenticate::isAuthorized(QString prefix)
{
	return(authorized.contains(prefix));
}

bool Authenticate::authorize(QString prefix, QString password)
{
	if(password == mPin && !authorized.contains(prefix))
	{
		authorized.append(prefix);
		QSettings settings;
		settings.setValue("authorized", authorized);
		qDebug("authorized");

		return true;
	}

	qDebug("failed to authorize or already authorized");
	return false;
}
