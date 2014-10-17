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
#include "ambplugin.h"
#include "debugout.h"

#include <QJsonDocument>
#include <QJSEngine>
#include <QDateTime>
#include <QString>
#include <QFile>
#include <QTimer>
#include <QtQml>

#include <dlfcn.h>

#define foreach Q_FOREACH

typedef std::map<std::string, QObject*> create_bluemonkey_module_t(std::map<std::string, std::string> config, QObject* parent);

extern "C" AbstractSource * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	auto plugin = new AmbPlugin<BluemonkeySink>(routingengine, config);
	plugin->init();

	return plugin;
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

BluemonkeySink::BluemonkeySink(AbstractRoutingEngine* e, map<string, string> config, AbstractSource &parent): QObject(0), AmbPluginImpl(e, config, parent), engine(nullptr), mSilentMode(false)
{
	QTimer::singleShot(1,this,SLOT(reloadEngine()));

	auth = new Authenticate(config, this);

	qmlRegisterType<QTimer>("", 1, 0, "QTimer");
}


PropertyList BluemonkeySink::subscriptions()
{

}

void BluemonkeySink::supportedChanged(const PropertyList & supportedProperties)
{
	DebugOut()<<"supported changed"<<endl;
}

void BluemonkeySink::propertyChanged(AbstractPropertyType * value)
{

}

const string BluemonkeySink::uuid() const
{
	return "bluemonkey";
}

int BluemonkeySink::supportedOperations()
{
	return AbstractSource::Get | AbstractSource::Set;
}

QObject *BluemonkeySink::subscribeTo(QString str)
{
	return new Property(str.toStdString(), "", routingEngine, Zone::None, this);
}

QObject *BluemonkeySink::subscribeToSource(QString str, QString srcFilter)
{
	return new Property(str.toStdString(), srcFilter, routingEngine, Zone::None, this);
}

QObject* BluemonkeySink::subscribeToZone(QString str, int zone)
{
	return new Property(str.toStdString(), "", routingEngine, zone, this);
}


QStringList BluemonkeySink::sourcesForProperty(QString property)
{
	std::list<std::string> list = routingEngine->sourcesForProperty(property.toStdString());
	QStringList strList;
	for(auto itr = list.begin(); itr != list.end(); itr++)
	{
		strList<<(*itr).c_str();
	}

	return strList;
}

QStringList BluemonkeySink::supportedProperties()
{
	PropertyList props = routingEngine->supported();
	QStringList strList;
	for(auto p : props)
	{
		strList<<p.c_str();
	}

	return strList;
}


bool BluemonkeySink::authenticate(QString pass)
{

}

void BluemonkeySink::loadConfig(QString str)
{
	QFile file(str);
	if(!file.open(QIODevice::ReadOnly))
	{
		DebugOut()<<"failed to open config file: "<<str.toStdString()<<endl;
		return;
	}

	QString script = file.readAll();

	file.close();

	DebugOut()<<"evaluating script: "<<script.toStdString()<<endl;

	QJSValue val = engine->evaluate(script);

	DebugOut()<<val.toString().toStdString()<<endl;
}

bool BluemonkeySink::loadModule(QString path)
{
	void* handle = dlopen(path.toUtf8().data(), RTLD_LAZY);

	if(!handle)
	{
		DebugOut(DebugOut::Warning) << "bluemonkey load module failed: " << dlerror() << endl;
		return false;
	}

	void* c = dlsym(handle, "create");

	if(!c)
	{
		DebugOut(DebugOut::Warning) << "bluemonkey load module failed: " << path.toStdString() << " " << dlerror() << endl;
		return false;
	}

	create_bluemonkey_module_t* create = (create_bluemonkey_module_t*)(c);

	std::map<std::string, QObject*> exports = create(configuration, this);

	for(auto i : exports)
	{
		QJSValue val = engine->newQObject(i.second);
		engine->globalObject().setProperty(i.first.c_str(), val);
	}

	return true;
}

void BluemonkeySink::reloadEngine()
{
	if(engine)
		engine->deleteLater();

	engine = new QJSEngine(this);

	QJSValue value = engine->newQObject(this);
	engine->globalObject().setProperty("bluemonkey", value);

	loadConfig(configuration["config"].c_str());
}

void BluemonkeySink::writeProgram(QString program)
{

	QJSEngine temp;
	QJSValue result = temp.evaluate(program);
	if(result.isError())
	{
		DebugOut(DebugOut::Error)<<"Syntax error in program: "<<result.toString().toStdString()<<endl;
		return;
	}

	QFile file(configuration["customPrograms"].c_str());

	if(!file.open(QIODevice::ReadWrite | QIODevice::Append))
	{
		DebugOut(DebugOut::Error)<<"failed to open file: "<<file.fileName().toStdString()<<endl;
		return;
	}

	file.write(program.toUtf8());
	file.write("\n");

	file.close();
}

void BluemonkeySink::log(QString str)
{
	DebugOut()<<str.toStdString()<<endl;
}

QObject *BluemonkeySink::createTimer()
{
	return new QTimer(this);
}

void BluemonkeySink::getHistory(QStringList properties, QDateTime begin, QDateTime end, QJSValue cbFunction)
{
	double b = (double)begin.toMSecsSinceEpoch() / 1000.0;
	double e = (double)end.toMSecsSinceEpoch() / 1000.0;
	AsyncRangePropertyRequest request;
	request.timeBegin = b;
	request.timeEnd = e;

	PropertyList reqlist;

	foreach(QString prop, properties)
	{
		reqlist.push_back(prop.toStdString());
	}

	request.properties = reqlist;
	request.completed = [&cbFunction](AsyncRangePropertyReply* reply)
	{
		if(!reply->success)
		{
			DebugOut(DebugOut::Error)<<"bluemoney get history call failed"<<endl;
			return;
		}

		if(cbFunction.isCallable())
		{
			QVariantList list;

			for(auto itr = reply->values.begin(); itr != reply->values.end(); itr++)
			{
				AbstractPropertyType *val = *itr;

				list.append(gvariantToQVariant(val->toVariant()));
			}

			QJSValue val = cbFunction.engine()->toScriptValue<QVariantList>(list);

			cbFunction.call(QJSValueList()<<val);

		}

		delete reply;
	};

	routingEngine->getRangePropertyAsync(request);
}

void BluemonkeySink::createCustomProperty(QString name, QJSValue defaultValue, int zone = Zone::None)
{

	auto create = [defaultValue, name]() -> AbstractPropertyType*
	{
			QVariant var = defaultValue.toVariant();

			if(!var.isValid())
				return nullptr;

			if(var.type() == QVariant::UInt)
				return new BasicPropertyType<uint>(name.toStdString(), var.toUInt());
			else if(var.type() == QVariant::Double)
				return new BasicPropertyType<double>(name.toStdString(), var.toDouble());
			else if(var.type() == QVariant::Bool)
				return new BasicPropertyType<bool>(name.toStdString(), var.toBool());
			else if(var.type() == QVariant::Int)
				return new BasicPropertyType<int>(name.toStdString(), var.toInt());
			else if(var.type() == QVariant::String)
				return new StringPropertyType(name.toStdString(), var.toString().toStdString());


			return nullptr;
	};

	addPropertySupport(zone, create);

	routingEngine->updateSupported(supported(), PropertyList(), &source);
}


QVariant Property::value()
{
	return mValue ? gvariantToQVariant(mValue->toVariant()) : QVariant::Invalid;
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
	request.value = mValue->copy();
	request.completed = [&](AsyncPropertyReply* reply)
	{
		if(reply->success)
		{
			propertyChanged(reply->value);
		}
		delete reply;
	};
	routingEngine->setProperty(request);
}

void Property::getHistory(QDateTime begin, QDateTime end, QJSValue cbFunction)
{
	double b = (double)begin.toMSecsSinceEpoch() / 1000.0;
	double e = (double)end.toMSecsSinceEpoch() / 1000.0;
	AsyncRangePropertyRequest request;
	request.timeBegin = b;
	request.timeEnd = e;

	PropertyList reqlist;
	reqlist.push_back(mValue->name);

	request.properties = reqlist;
	request.completed = [&cbFunction](AsyncRangePropertyReply* reply)
	{
		if(!reply->success)
		{
			DebugOut(DebugOut::Error)<<"bluemoney get history call failed"<<endl;
			return;
		}

		if(cbFunction.isCallable())
		{
			QVariantList list;

			for(auto itr = reply->values.begin(); itr != reply->values.end(); itr++)
			{
				AbstractPropertyType *val = *itr;

				list.append(gvariantToQVariant(val->toVariant()));
			}
			QJSValue val = cbFunction.engine()->toScriptValue<QVariantList>(list);
			cbFunction.call(QJSValueList()<<val);

		}

		delete reply;
	};

	routingEngine->getRangePropertyAsync(request);
}

Property::Property(VehicleProperty::Property prop, QString srcFilter, AbstractRoutingEngine* re, Zone::Type zone, QObject *parent)
	:QObject(parent), AbstractSink(re, std::map<std::string,std::string>()),mValue(nullptr), mUuid(amb::createUuid()), mZone(zone)
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
		routingEngine->unsubscribeToProperty(type().toStdString(), this);

	routingEngine->subscribeToProperty(t.toStdString(), this);

	mValue = VehicleProperty::getPropertyTypeForPropertyNameValue(t.toStdString());

	if(!mValue)
		return;

	AsyncPropertyRequest request;
	request.property = mValue->name;
	request.completed = [this](AsyncPropertyReply* reply)
	{
		if(reply->success)
			propertyChanged(reply->value);

		delete reply;
	};

	routingEngine->getPropertyAsync(request);
}

void Property::propertyChanged(AbstractPropertyType *value)
{
	if(value->zone != mZone)
		return;

	if(mValue)
	{
		delete mValue;
	}
	mValue = value->copy();

	changed(gvariantToQVariant(mValue->toVariant()));
}


QVariant BluemonkeySink::zonesForProperty(QString prop, QString src)
{
	PropertyInfo info = routingEngine->getPropertyInfo(prop.toStdString(), src.toStdString());

	QVariantList list;

	for(auto i : info.zones())
	{
		list << i;
	}

	return list;
}
