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
#include <QTimer>

Q_SCRIPT_DECLARE_QMETAOBJECT(QTimer, QObject*)

extern "C" AbstractSinkManager * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new BluemonkeySinkManager(routingengine, config);
}

QVariant gvariantToQVariant(GVariant *value)
{
	GVariantClass c = g_variant_classify(value);
	if(c == G_VARIANT_CLASS_BOOLEAN)
		return QVariant((bool) g_variant_get_boolean(value));

	else if(c == G_VARIANT_CLASS_BYTE)
		return QVariant((char) g_variant_get_byte(value));

	else if(c == G_VARIANT_CLASS_INT16)
		return QVariant((int) g_variant_get_int16(value));

	else if(c == G_VARIANT_CLASS_UINT16)
		return QVariant((unsigned int) g_variant_get_uint16(value));

	else if(c == G_VARIANT_CLASS_INT32)
		return QVariant((int) g_variant_get_int32(value));

	else if(c ==  G_VARIANT_CLASS_UINT32)
		return QVariant((unsigned int) g_variant_get_uint32(value));

	else if(c == G_VARIANT_CLASS_INT64)
		return QVariant((long long) g_variant_get_int64(value));

	else if(c == G_VARIANT_CLASS_UINT64)
		return QVariant((unsigned long long) g_variant_get_uint64(value));

	else if(c == G_VARIANT_CLASS_DOUBLE)
		return QVariant(g_variant_get_double(value));

	else if(c == G_VARIANT_CLASS_STRING)
		return QVariant(g_variant_get_string(value, NULL));

	else if(c == G_VARIANT_CLASS_ARRAY)
	{
		gsize dictsize = g_variant_n_children(value);
		QVariantList list;
		for (int i=0;i<dictsize;i++)
		{
			GVariant *childvariant = g_variant_get_child_value(value,i);
			GVariant *innervariant = g_variant_get_variant(childvariant);
			list.append(gvariantToQVariant(innervariant));
		}
		return list;
	}

	else
		return QVariant::Invalid;

}

BluemonkeySink::BluemonkeySink(AbstractRoutingEngine* e, map<string, string> config): QObject(0), AbstractSink(e, config), engine(nullptr)
{
	irc = new IrcCommunication(config, this);

	reloadEngine();

	auth = new Authenticate(config, this);

	connect(irc, &IrcCommunication::message, [&](QString sender, QString prefix, QString codes ) {

		if(codes.contains("authenticate"))
		{

			int i = codes.indexOf("authenticate");
			QString pin = codes.mid(i+13);
			pin = pin.trimmed();


			if(!auth->authorize(prefix, pin))
				irc->respond(sender,"failed");
			qDebug()<<sender;

		}
		else if(codes.startsWith("bluemonkey"))
		{
			if(!auth->isAuthorized(prefix))
			{
				irc->respond(sender, "denied");
				return;
			}

			QString bm("bluemonkey");

			codes = codes.mid(bm.length()+1);

			irc->respond(sender, engine->evaluate(codes).toString());
		}
	});



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
	configsToLoad.append(str);
	QTimer::singleShot(1,this,SLOT(loadConfigPriv()));
}

void BluemonkeySink::loadConfigPriv()
{
	if(!configsToLoad.count()) return;

	QString str = configsToLoad.first();
	configsToLoad.pop_front();

	QFile file(str);
	if(!file.open(QIODevice::ReadOnly))
	{
		qDebug()<<"failed to open config file: "<<str;
		return;
	}

	QString script = file.readAll();

	file.close();

	QScriptValue val = engine->evaluate(script);

	qDebug()<<val.toString();
}

void BluemonkeySink::reloadEngine()
{
	if(engine)
		engine->deleteLater();

	engine = new QScriptEngine(this);

	QScriptValue value = engine->newQObject(this);
	engine->globalObject().setProperty("bluemonkey", value);

	QScriptValue qtimerClass = engine->scriptValueFromQMetaObject<QTimer>();
	engine->globalObject().setProperty("QTimer", qtimerClass);



	loadConfig(configuration["config"].c_str());
}

void BluemonkeySink::writeProgram(QString program)
{
	QFile file(configuration["customPrograms"].c_str());

	file.open(QIODevice::ReadWrite | QIODevice::Append);

	file.write(program.toUtf8());
	file.write("\n");

	file.close();
}


QVariant Property::value()
{
	return gvariantToQVariant(mValue->toVariant());
}

void Property::setValue(QVariant v)
{
	if(v.type() == QVariant::List || v.type() == QVariant::Map)
	{

		QJsonDocument doc = QJsonDocument::fromVariant(v);

		QString json = doc.toJson();

		mValue->fromString(json.toStdString());
	}



	else
	{
		QString tempVal = v.toString();
		mValue->fromString(tempVal.toStdString());
	}

	AsyncSetPropertyRequest request;
	request.property = mValue->name;
	request.value = mValue;
	request.completed = [&](AsyncPropertyReply* reply)
	{
		if(reply->success)
		{
			propertyChanged(reply->property,reply->value,reply->value->sourceUuid);
		}
		delete reply;
	};
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

void Property::propertyChanged(VehicleProperty::Property property, AbstractPropertyType *value, string uuid)
{
	mValue = value->copy();

	changed(gvariantToQVariant(mValue->toVariant()));
}
