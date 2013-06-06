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

#include "abstractsink.h"
#include <QObject>
#include <QVariant>
#include <QJsonDocument>
#include "uuidhelper.h"

#include "authenticate.h"

class IrcCommunication;
class QScriptEngine;

class Property: public QObject, public AbstractSink
{
	Q_OBJECT
	Q_PROPERTY(QString type READ type)
	Q_PROPERTY(QVariant value READ value WRITE setValue)

public:
	Property(VehicleProperty::Property, AbstractRoutingEngine* re, QObject *parent = 0);

	QString type();
	void setType(QString t);

	virtual PropertyList subscriptions() { PropertyList list; list.push_back(type().toStdString()); return list; }
	virtual void supportedChanged(PropertyList ){	}

	virtual void propertyChanged(VehicleProperty::Property property, AbstractPropertyType* value, std::string uuid);

	virtual std::string uuid() { return amb::createUuid(); }

	QVariant value();
	void setValue(QVariant v);

Q_SIGNALS:

	void changed(QVariant val);

private:
	AbstractPropertyType* mValue;

};

class BluemonkeySink : public QObject, public AbstractSink
{
Q_OBJECT
public:
	BluemonkeySink(AbstractRoutingEngine* e, map<string, string> config);
	virtual PropertyList subscriptions();
	virtual void supportedChanged(PropertyList supportedProperties);
	virtual void propertyChanged(VehicleProperty::Property property, AbstractPropertyType* value, std::string uuid);
	virtual std::string uuid();

public Q_SLOTS:

	QObject* subscribeTo(QString str);

	bool authenticate(QString pass);

	void loadConfig(QString str);

Q_SIGNALS:


private:
	IrcCommunication* irc;
	QScriptEngine* engine;
	Authenticate* auth;
};

class BluemonkeySinkManager: public AbstractSinkManager
{
public:
	BluemonkeySinkManager(AbstractRoutingEngine* engine, map<string, string> config)
	:AbstractSinkManager(engine, config)
	{
		new BluemonkeySink(routingEngine, config);
	}
};

#endif // BluemonkeySink_H
