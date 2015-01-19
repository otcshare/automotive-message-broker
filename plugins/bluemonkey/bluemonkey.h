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

#include <abstractsource.h>
#include <ambpluginimpl.h>
#include <uuidhelper.h>

#include <map>

#include <QObject>
#include <QVariant>
#include <QJsonDocument>
#include <QDateTime>
#include <QJSValue>
#include <QThread>

#include "authenticate.h"

class QJSEngine;

class Property: public QObject, public AbstractSink
{
	Q_OBJECT
	Q_PROPERTY(QString name READ name)
	Q_PROPERTY(QString source READ source)
	Q_PROPERTY(double timestamp READ timestamp)
	Q_PROPERTY(QVariant value READ value WRITE setValue)
	Q_PROPERTY(int zone READ zone)

public:
	Property(VehicleProperty::Property, QString srcFilter, AbstractRoutingEngine * re, Zone::Type zone = Zone::None, QObject * parent = 0);

	QString name();
	void setType(QString t);

	QString source()
	{
		return mValue->sourceUuid.c_str();
	}

	double timestamp()
	{
		return mValue->timestamp;
	}

	virtual PropertyList subscriptions() { return PropertyList(); }
	virtual void supportedChanged(const PropertyList &)
	{
		DebugOut()<<"Bluemonkey Property Supported Changed"<<endl;
	}

	virtual void propertyChanged(AbstractPropertyType* value);

	virtual const std::string uuid() { return mUuid; }

	QVariant value();
	void setValue(QVariant v);

	void getHistory(QDateTime begin, QDateTime end, QJSValue cbFunction);

	Zone::Type zone() { return mZone; }

Q_SIGNALS:

	void changed(QVariant val);

private:
	AbstractPropertyType* mValue;
	const std::string mUuid;
	Zone::Type mZone;

};

class BluemonkeySink : public QObject, public AmbPluginImpl
{
Q_OBJECT

public:
	using AmbPluginImpl::setProperty;
	using QObject::setProperty;

	BluemonkeySink(AbstractRoutingEngine* e, map<string, string> config,  AbstractSource& parent);
	~BluemonkeySink();
	virtual PropertyList subscriptions();
	virtual void supportedChanged(const PropertyList & supportedProperties);
	virtual void propertyChanged(AbstractPropertyType* value);
	virtual const std::string uuid() const;

	virtual int supportedOperations();

private: //source privates

	PropertyList mSupported;
	std::list<AbstractPropertyType*> propertyValueCache;


public Q_SLOTS:

	QObject* subscribeTo(QString str);
	QObject* subscribeTo(QString str, int zone);
	QObject* subscribeTo(QString str, int zone, QString srcFilter);

	QStringList sourcesForProperty(QString property);
	QVariant zonesForProperty(QString property, QString src);

	QStringList supportedProperties();

	bool authenticate(QString pass);

	void loadConfig(QString str);

	bool loadModule(QString path);

	void reloadEngine();

	void writeProgram(QString program);

	void log(QString str);

	QObject* createTimer();
	QObject* createQObject();

	void getHistory(QStringList properties, QDateTime begin, QDateTime end, QJSValue cbFunction);

	void setSilentMode(bool m)
	{
		mSilentMode = m;
	}

	void createCustomProperty(QString name, QJSValue defaultValue)
	{
		createCustomProperty(name, defaultValue, Zone::None);
	}

	void createCustomProperty(QString name, QJSValue defaultValue, int zone);

	void exportInterface(QString name, QJSValue properties);

private:
	QList<void*> modules;
	QJSEngine* engine;
	QStringList configsToLoad;

	Authenticate* auth;
	bool mSilentMode;
};


#endif // BluemonkeySink_H
