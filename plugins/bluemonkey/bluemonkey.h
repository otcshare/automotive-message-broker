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


#ifndef Bluemonkey_H
#define Bluemonkey_H

#include <map>

#include <QObject>
#include <QVariant>
#include <QJsonDocument>
#include <QDateTime>
#include <QJSValue>
#include <QCoreApplication>

#include "authenticate.h"

class QJSEngine;

class Bluemonkey : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QStringList arguments READ arguments)

public:
	Bluemonkey(QObject * parent = nullptr);

	~Bluemonkey();

	bool loadModule(const QString &name, QObject* module);

	QStringList arguments() { return mArguments;}
	void setArguments(const QStringList & args) { mArguments = args; }
	void setArguments(int len, char **args);

public Q_SLOTS:

	void assertIsTrue(bool isTrue, const QString &msg="");

	void loadScript(QString str);

	bool loadModule(QString path);

	void writeProgram(QString program);

	void log(QJSValue str);

	QObject* createTimer();
	QObject* createQObject();

	QObject* createCoreApplication();

	int run(QCoreApplication *app);

Q_SIGNALS:

	void ready();

private:
	QList<void*> modules;
	QJSEngine* engine;
	QStringList configsToLoad;
	QStringList mArguments;
	std::map<std::string, std::string> configuration;
};

#endif // Bluemonkey_H
