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
#include "abstractroutingengine.h"
#include "debugout.h"
#include "irccoms.h"

#include <QJsonDocument>
#include <QScriptEngine>
#include <QString>
#include <QFile>

extern "C" AbstractSinkManager * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new BluemonkeySinkManager(routingengine, config);
}

QVariant gvariantToQVariant(GVariant *value)
{
	switch (g_variant_classify(value)) {
		case G_VARIANT_CLASS_BOOLEAN:
			return QVariant((bool) g_variant_get_boolean(value));

		case G_VARIANT_CLASS_BYTE:
			return QVariant((char) g_variant_get_byte(value));

		case G_VARIANT_CLASS_INT16:
			return QVariant((int) g_variant_get_int16(value));

		case G_VARIANT_CLASS_UINT16:
			return QVariant((unsigned int) g_variant_get_uint16(value));

		case G_VARIANT_CLASS_INT32:
			return QVariant((int) g_variant_get_int32(value));

		case G_VARIANT_CLASS_UINT32:
			return QVariant((unsigned int) g_variant_get_uint32(value));

		case G_VARIANT_CLASS_INT64:
			return QVariant((long long) g_variant_get_int64(value));

		case G_VARIANT_CLASS_UINT64:
			return QVariant((unsigned long long) g_variant_get_uint64(value));

		case G_VARIANT_CLASS_DOUBLE:
			return QVariant(g_variant_get_double(value));

		case G_VARIANT_CLASS_STRING:
			return QVariant(g_variant_get_string(value, NULL));

		default:
			return QVariant::Invalid;
	}
}

BluemonkeySink::BluemonkeySink(AbstractRoutingEngine* e, map<string, string> config): QObject(0), AbstractSink(e, config)
{
	irc = new IrcCommunication(this);
	irc->connect("chat.freenode.com",8001,"","tripzero","bluemonkey","");
	connect(irc,&IrcCommunication::connected, [&]() {
		irc->join("#linuxice");
	});

	engine = new QScriptEngine(this);

	auth = new Authenticate(this);

	QScriptValue value = engine->newQObject(this);
	engine->globalObject().setProperty("bluemonkey", value);

	connect(irc, &IrcCommunication::message, [&](QString sender, QString prefix, QString codes ) {

		if(codes.contains("authenticate"))
		{

			int i = codes.indexOf("authenticate");
			QString pin = codes.mid(i+10);
			pin = pin.trimmed();


			if(!auth->authorize(prefix, pin))
				irc->respond(sender,"failed");

		}
		else if(codes.startsWith("bluemonkey"))
		{
			if(!auth->isAuthorized(prefix))
			{
				irc->respond(sender, "denied");
				return;
			}
			irc->respond(sender, engine->evaluate(codes).toString());
		}
	});

	loadConfig("config.js");

}


PropertyList BluemonkeySink::subscriptions()
{

}

void BluemonkeySink::supportedChanged(PropertyList supportedProperties)
{

}

void BluemonkeySink::propertyChanged(VehicleProperty::Property property, AbstractPropertyType* value, std::string uuid)
{

}

std::string BluemonkeySink::uuid()
{
	return "bluemonkey";
}

QObject *BluemonkeySink::subscribeTo(QString str)
{
	return new Property(str.toStdString(), routingEngine, this);
}

bool BluemonkeySink::authenticate(QString pass)
{

}

void BluemonkeySink::loadConfig(QString str)
{
	QFile file(str);
	if(!file.open(QIODevice::ReadOnly))
	{
		qDebug()<<"failed to open config file: "<<str;
		return;
	}

	QString script = file.readAll();

	file.close();

	engine->evaluate(script);
}


QVariant Property::value()
{
	return gvariantToQVariant(mValue->toVariant());
}

void Property::setValue(QVariant v)
{
	QJsonDocument doc;
	doc.fromVariant(v);

	mValue->fromString(doc.toJson().data());

	AsyncSetPropertyRequest request;
	request.property = mValue->name;
	request.value = mValue;
	request.completed = [](AsyncPropertyReply* reply) { delete reply; };
	routingEngine->setProperty(request);
}

Property::Property(VehicleProperty::Property prop, AbstractRoutingEngine* re, QObject *parent)
	:QObject(parent), AbstractSink(re, std::map<std::string,std::string>()),mValue(nullptr)
{
	setType(prop.c_str());
}

QString Property::type()
{
	return mValue->name.c_str();
}

void Property::setType(QString t)
{
	if(mValue && type() != "")
		routingEngine->unsubscribeToProperty(type().toStdString(),this);

	routingEngine->subscribeToProperty(t.toStdString(),this);

	mValue = VehicleProperty::getPropertyTypeForPropertyNameValue(t.toStdString());

	AsyncPropertyRequest request;
	request.property = mValue->name;
	request.completed = [this](AsyncPropertyReply* reply)
	{
		propertyChanged(reply->property, reply->value,uuid());
		delete reply;
	};

	routingEngine->getPropertyAsync(request);
}
