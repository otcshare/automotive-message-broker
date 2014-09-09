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


#ifndef BluemonkeySink_H
#define BluemonkeySink_H

#include "abstractsource.h"
#include "ambpluginimpl.h"

#include <QObject>
#include <QVariant>
#include <QJsonDocument>
#include <QDateTime>
#include <QScriptValue>
#include "uuidhelper.h"

#include "authenticate.h"
#include "agent.h"

class IrcCommunication;
class QScriptEngine;

class Property: public QObject, public AbstractSink
{
	Q_OBJECT
	Q_PROPERTY(QString type READ type)
	Q_PROPERTY(QVariant value READ value WRITE setValue)

public:
	Property(VehicleProperty::Property, QString srcFilter, AbstractRoutingEngine* re, QObject *parent = 0);

	QString type();
	void setType(QString t);

	virtual PropertyList subscriptions() { return PropertyList(); }
	virtual void supportedChanged(const PropertyList &)
	{
		DebugOut()<<"Bluemonkey Property Supported Changed"<<endl;
	}

	virtual void propertyChanged(AbstractPropertyType* value);

	virtual const std::string uuid() { return mUuid; }

	QVariant value();
	void setValue(QVariant v);

	void getHistory(QDateTime begin, QDateTime end, QScriptValue cbFunction);
Q_SIGNALS:

	void changed(QVariant val);

private:
	AbstractPropertyType* mValue;
	const std::string mUuid;

};

class BluemonkeySink : public QObject, public AmbPluginImpl
{
Q_OBJECT
public:
	BluemonkeySink(AbstractRoutingEngine* e, map<string, string> config,  AbstractSource& parent);
	virtual PropertyList subscriptions();
	virtual void supportedChanged(const PropertyList & supportedProperties);
	virtual void propertyChanged(AbstractPropertyType* value);
	virtual const std::string uuid() const;

	QScriptEngine* engine;

	virtual int supportedOperations();

private: //source privates

	PropertyList mSupported;
	std::list<AbstractPropertyType*> propertyValueCache;


public Q_SLOTS:

	QObject* subscribeTo(QString str);
	QObject* subscribeTo(QString str, QString srcFilter);

	QStringList sourcesForProperty(QString property);

	QStringList supportedProperties();

	bool authenticate(QString pass);

	void loadConfig(QString str);

	void reloadEngine();

	void writeProgram(QString program);

	void log(QString str);

	void getHistory(QStringList properties, QDateTime begin, QDateTime end, QScriptValue cbFunction);

	void setSilentMode(bool m)
	{
		mSilentMode = m;
	}

	void createCustomProperty(QString name, QScriptValue defaultValue);

private:
	QStringList configsToLoad;
	IrcCommunication* irc;

	Authenticate* auth;
	BluemonkeyAgent* agent;
	bool mSilentMode;
};


#endif // BluemonkeySink_H
