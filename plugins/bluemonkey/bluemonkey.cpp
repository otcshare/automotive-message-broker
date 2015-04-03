/*
  Copyright (C) 2012  Intel Corporation

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "bluemonkey.h"

#include <QJsonDocument>
#include <QJSEngine>
#include <QDateTime>
#include <QString>
#include <QFile>
#include <QTimer>
#include <QtQml>

#include <dlfcn.h>

#define foreach Q_FOREACH

typedef void create_bluemonkey_module_t(std::map<std::string, std::string> config, std::map<std::string, QObject*> &exports, QString &javascript, QObject* parent);

QString bmJS = (""
				"console = { };"
				"console.log = function(msg)"
				"{"
				"  bluemonkey.log(msg);"
				"}"
				""
				"function Application(args)"
				"{"
				"  this.app = bluemonkey.createCoreApplication();"
				"  bluemonkey.assertIsTrue(this.app !== undefined, 'Need to instantiate QCoreApplication in the bluemonkey app before calling createCoreApplication()');"
				"  this._connectEverything(this);"
				""
				"  this.startTimer = bluemonkey.createTimer();"
				"  this.startTimer.singleShot = true;"
				"  this.startTimer.timeout.connect(bluemonkey.ready);"
				"  this.startTimer.start(1);"
				"}"
				"Application.prototype._connectEverything = function(obj)"
				"{"
				"  bluemonkey.ready.connect(function()"
				"  {"
				"    if(obj.main !== undefined)"
				"      obj.main();"
				"  });"
				"};"
				"Application.prototype.run = function()"
				"{"
				"  return bluemonkey.run(this.app);"
				"};");


Bluemonkey::Bluemonkey(QObject *parent)
	:QObject(parent), engine(nullptr), configuration(std::map<std::string, std::string>())
{
	engine = new QJSEngine(this);

	QJSValue value = engine->newQObject(this);
	engine->globalObject().setProperty("bluemonkey", value);

	QJSValue val = engine->evaluate(bmJS);

	if(val.isError())
	{
		qCritical() << "Failed to load bluemonkey javascript extensions";
		qCritical() << "Error: ";
		qCritical() <<  val.property("name").toString();
		qCritical() <<  val.property("message").toString();
		qCritical() << "line: " << val.property("lineNumber").toString();

		int line = val.property("lineNumber").toInt();
		QStringList lines = bmJS.split("\n");

		if(line - 1 >= 0)
			qWarning() << lines.at(line-1);

		qWarning() << lines.at(line) << "<--";

		if(lines.size() > line + 1)
			qWarning() << lines.at(line+1);

		qCritical() << "Aborting";
		throw std::runtime_error("Die die die");
	}
}

Bluemonkey::~Bluemonkey()
{
	Q_FOREACH(void* module, modules)
	{
		dlclose(module);
	}

	engine->deleteLater();
}

void Bluemonkey::loadScript(QString str)
{
	QFile file(str);
	if(!file.open(QIODevice::ReadOnly))
	{
		qDebug()<< "failed to open config file: "<< str;
		return;
	}

	QString firstLine = file.readLine();

	if(firstLine.startsWith("#!"))
		firstLine = "";

	QString script = firstLine + "\n" + file.readAll();

	file.close();

	QJSValue val = engine->evaluate(script);

	if(val.isError())
	{
		qDebug() << "Error: ";
		qDebug() <<  val.property("name").toString();
		qDebug() <<  val.property("message").toString();
		qDebug() <<  str <<  ":" << val.property("lineNumber").toString();

		int line = val.property("lineNumber").toInt();
		QStringList lines = script.split("\n");

		if(line - 1 >= 0)
			qWarning() << lines.at(line-1);

		qWarning() << lines.at(line) << "<--";

		if(lines.size() > line + 1)
			qWarning() << lines.at(line+1);

		throw std::runtime_error("JavaScript Error");
	}
}

bool Bluemonkey::loadModule(QString path)
{
	void* handle = dlopen(path.toUtf8().data(), RTLD_LAZY);

	if(!handle)
	{
		qDebug() <<  "bluemonkey load module failed: " <<  dlerror() <<  endl;
		return false;
	}

	if(modules.contains(handle))
		return false;

	modules.push_back(handle);

	void* c = dlsym(handle, "create");

	if(!c)
	{
		qDebug() <<  "bluemonkey load module failed: " <<  path <<  " " <<  dlerror() <<  endl;
		return false;
	}

	create_bluemonkey_module_t* create = reinterpret_cast<create_bluemonkey_module_t*>(c);

	if(!create)
	{
		qCritical() << "Failed to call create() on module "<< path << ". Check signature.";
	}

	std::map<std::string, QObject*> exports;
	QString js;
	create(configuration, exports, js, this);

	for(auto i : exports)
	{
		QObject* obj = i.second;
		loadModule(i.first.c_str(), obj);
	}

	QJSValue val = engine->evaluate(js);

	if(val.isError())
	{
		qWarning() << "Error running script in module: " << path;
		qWarning() <<  val.property("name").toString();
		qWarning() <<  val.property("message").toString();
		qWarning() << "line: " << val.property("lineNumber").toString();

		int line = val.property("lineNumber").toInt();
		QStringList lines = js.split("\n");

		if(line - 1 >= 0)
			qWarning() << lines.at(line-1);

		qWarning() << lines.at(line) << "<--";

		if(lines.size() > line + 1)
			qWarning() << lines.at(line+1);

		qCritical() << "Aborting";
		throw std::runtime_error("Failed to load module");
	}

	return true;
}

void Bluemonkey::writeProgram(QString program)
{

	QJSEngine temp;
	QJSValue result = temp.evaluate(program);
	if(result.isError())
	{
		qDebug()<< "Syntax error in program: "<< result.toString();
		return;
	}

	QFile file(configuration["customPrograms"].c_str());

	if(!file.open(QIODevice::ReadWrite | QIODevice::Append))
	{
		qDebug() <<  "failed to open file: " <<  file.fileName() <<  endl;
		return;
	}

	file.write(program.toUtf8());
	file.write("\n");

	file.close();
}

void Bluemonkey::log(QJSValue str)
{
	qDebug()<< str.toString();
}

QObject *Bluemonkey::createTimer()
{
	return new QTimer(this);
}

QObject *Bluemonkey::createQObject()
{
	return new QObject(this);
}

QObject *Bluemonkey::createCoreApplication()
{
	return QCoreApplication::instance();
}


bool Bluemonkey::loadModule(const QString &name, QObject *module)
{
	if(!engine->globalObject().hasProperty(name))
	{
		QJSValue val = engine->newQObject(module);
		engine->globalObject().setProperty(name, val);
	}
}

void Bluemonkey::setArguments(int len, char **args)
{
	QStringList a;
	for(int i = 0; i < len; i++)
	{
		a.append(args[i]);
	}

	setArguments(a);
}

void Bluemonkey::assertIsTrue(bool isTrue, const QString & msg)
{
	if(!isTrue)
		log(msg);

	Q_ASSERT(isTrue);
}


int Bluemonkey::run(QCoreApplication *app)
{
	return app->exec();
}
