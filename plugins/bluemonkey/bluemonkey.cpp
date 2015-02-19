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
				"");


Bluemonkey::Bluemonkey(std::map<std::string, std::string> config, QObject *parent)
	:QObject(parent), engine(nullptr), configuration(config)
{
	QTimer::singleShot(1,this,SLOT(reloadEngine()));
}

Bluemonkey::~Bluemonkey()
{
	Q_FOREACH(void* module, modules)
	{
		dlclose(module);
	}

	engine->deleteLater();
}

void Bluemonkey::loadConfig(QString str)
{
	QFile file(str);
	if(!file.open(QIODevice::ReadOnly))
	{
		qDebug()<< "failed to open config file: "<< str;
		return;
	}

	QString script = file.readAll();

	file.close();

	qDebug() << "evaluating script: "<< script;

	QJSValue val = engine->evaluate(script);

	qDebug()<< val.toString();

	if(val.isError())
	{
		qDebug() <<  val.property("name").toString() <<  endl;
		qDebug() <<  val.property("message").toString() <<  endl;
		qDebug() <<  str <<  ":" << val.property("lineNumber").toString() <<  endl;
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
	create(configuration, exports, js, nullptr);

	for(auto i : exports)
	{
		QObject* obj = i.second;
		loadModule(i.first.c_str(), obj);
	}

	QJSValue val = engine->evaluate(js);

	qDebug() << "evalutating module js result: " << val.toString();

	if(val.isError())
	{
		qDebug() << "Script: " << js;
		qCritical() << "Error in module javascript: " << val.toString();
	}

	return true;
}

void Bluemonkey::reloadEngine()
{
	if(engine)
		engine->deleteLater();

	engine = new QJSEngine();

	QJSValue value = engine->newQObject(this);
	engine->globalObject().setProperty("bluemonkey", value);

	if(engine->evaluate(bmJS).isError())
	{
		qCritical() << "Failed to load bluemonkey javascript extensions";
		return;
	}

	ready();

	loadConfig(configuration["config"].c_str());

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


bool Bluemonkey::loadModule(const QString &name, QObject *module)
{
	if(!engine->globalObject().hasProperty(name))
	{
		QJSValue val = engine->newQObject(module);
		engine->globalObject().setProperty(name, val);
	}
}


void Bluemonkey::assertIsTrue(bool isTrue, const QString & msg)
{
	if(!isTrue)
		log(msg);

	Q_ASSERT(isTrue);
}
