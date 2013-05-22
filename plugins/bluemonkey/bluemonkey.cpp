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


extern "C" AbstractSinkManager * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new BluemonkeySinkManager(routingengine, config);
}

BluemonkeySink::BluemonkeySink(AbstractRoutingEngine* engine, map<string, string> config): QObject(0),AbstractSink(engine, config)
{
	irc = new IrcCommunication(this);
	irc->connect("chat.freenode.com",8001,"","tripzero","bluemoney","");
	connect(irc,&IrcCommunication::connected, [&]() {
		irc->join("#linuxice");
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


QVariant Property::value()
{
	QJsonDocument doc;

	doc.fromJson(mValue->toString().c_str());

	return doc.toVariant();
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
	:QObject(parent), AbstractSink(re, std::map<std::string,std::string>())
{
	setType(prop.c_str());
}

QString Property::type()
{
	return mValue->name.c_str();
}

void Property::setType(QString t)
{
	mValue = VehicleProperty::getPropertyTypeForPropertyNameValue(t.toStdString());

	AsyncPropertyRequest request;
	request.property = mValue->name;
	request.completed = [this](AsyncPropertyReply* reply)
	{
		propertyChanged(reply->property, reply->value,uuid());
		delete reply;
	};
}
