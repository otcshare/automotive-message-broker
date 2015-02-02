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

#include "dbusplugin.h"
#include "dbusexport.h"

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

extern "C" void create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	auto plugin = new AmbPlugin<BluemonkeySink>(routingengine, config);
	plugin->init();
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
		for (int i=0; i<dictsize; i++)
		{
			GVariant *childvariant = g_variant_get_child_value(value, i);
			GVariant *innervariant = g_variant_get_variant(childvariant);
			list.append(gvariantToQVariant(innervariant));
		}
		return list;
	}

	else
		return QVariant::Invalid;

}

AbstractPropertyType* qVariantToAbstractPropertyType(QString name, QVariant var)
{
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
	else if(var.type() == QVariant::List && var.toList().count())
	{
		QVariant subVariant = var.toList().at(0);
		if(subVariant.type() == QVariant::UInt)
			return new ListPropertyType<uint>(name.toStdString(), subVariant.toUInt());
		else if(subVariant.type() == QVariant::Double)
			return new ListPropertyType<double>(name.toStdString(), subVariant.toDouble());
		else if(subVariant.type() == QVariant::Bool)
			return new ListPropertyType<bool>(name.toStdString(), subVariant.toBool());
		else if(subVariant.type() == QVariant::Int)
			return new ListPropertyType<int>(name.toStdString(), subVariant.toInt());
		else if(subVariant.type() == QVariant::String)
			return new ListPropertyType<std::string>(name.toStdString(), subVariant.toString().toStdString());
	}
	return nullptr;
}

QVariant toQVariant(AbstractPropertyType* val)
{
	QVariantMap value;

	value["name"] = val->name.c_str();
	value["zone"] = val->zone;
	value["source"] = val->sourceUuid.c_str();
	value["timestamp"] = val->timestamp;
	value["sequence"] = val->sequence;
	value["value"] = gvariantToQVariant(val->toVariant());

	return value;
}

class BluemonkeyDBusInterface: public DBusSink
{
public:
	BluemonkeyDBusInterface(std::string ifaceName, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink(ifaceName, re, connection)
	{

	}
};

BluemonkeySink::BluemonkeySink(AbstractRoutingEngine* e, map<string, string> config, AbstractSource &parent)
	: QObject(0), AmbPluginImpl(e, config, parent), engine(nullptr), mSilentMode(false)
{
	QTimer::singleShot(1,this,SLOT(reloadEngine()));

	auth = new Authenticate(config, this);
}

BluemonkeySink::~BluemonkeySink()
{
	Q_FOREACH(void* module, modules)
	{
		dlclose(module);
	}

	engine->deleteLater();
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

QObject *BluemonkeySink::subscribeTo(QString str, int zone, QString srcFilter)
{
	return new Property(str.toStdString(), srcFilter, routingEngine, zone, this);
}

QObject* BluemonkeySink::subscribeTo(QString str, int zone)
{
	return new Property(str.toStdString(), "", routingEngine, zone, this);
}


QStringList BluemonkeySink::sourcesForProperty(QString property)
{
	std::vector<std::string> list = routingEngine->sourcesForProperty(property.toStdString());
	QStringList strList;
	for(auto itr : list)
	{
		strList<<itr.c_str();
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
		DebugOut(DebugOut::Error)<<"failed to open config file: "<<str.toStdString()<<endl;
		return;
	}

	QString script = file.readAll();

	file.close();

	DebugOut(7)<<"evaluating script: "<<script.toStdString()<<endl;

	QJSValue val = engine->evaluate(script);

	DebugOut()<<val.toString().toStdString()<<endl;

	if(val.isError())
	{
		DebugOut(DebugOut::Error) << val.property("name").toString().toStdString() << endl;
		DebugOut(DebugOut::Error) << val.property("message").toString().toStdString() << endl;
		DebugOut(DebugOut::Error) << str.toStdString() << ":" <<val.property("lineNumber").toString().toStdString() << endl;
	}
}

bool BluemonkeySink::loadModule(QString path)
{
	void* handle = dlopen(path.toUtf8().data(), RTLD_LAZY);

	if(!handle)
	{
		DebugOut(DebugOut::Warning) << "bluemonkey load module failed: " << dlerror() << endl;
		return false;
	}

	if(modules.contains(handle))
		return false;

	modules.push_back(handle);

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
		std::string obj = i.first;
		if(!engine->globalObject().hasProperty(obj.c_str()))
		{
			QJSValue val = engine->newQObject(i.second);
			engine->globalObject().setProperty(obj.c_str(), val);
		}
	}

	return true;
}

void BluemonkeySink::reloadEngine()
{
	if(engine)
		engine->deleteLater();

	engine = new QJSEngine();

	QJSValue value = engine->newQObject(this);
	engine->globalObject().setProperty("bluemonkey", value);

	QThread* thread = new QThread(this);

	engine->moveToThread(thread);

	thread->start();

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

QObject *BluemonkeySink::createQObject()
{
	return new QObject(this);
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

			for(auto val : reply->values)
			{
				list.append(toQVariant(val));
			}

			QJSValue val = cbFunction.engine()->toScriptValue<QVariantList>(list);

			cbFunction.call(QJSValueList()<<val);

		}

		delete reply;
	};

	routingEngine->getRangePropertyAsync(request);
}

void BluemonkeySink::createCustomProperty(QString name, QJSValue defaultValue, int zone)
{
	QVariant var = defaultValue.toVariant();

	DebugOut() << "Variant value for: " << name.toStdString() << " is " << defaultValue.toString().toStdString() << endl;

	auto create = [name, var]() -> AbstractPropertyType*
	{
		return qVariantToAbstractPropertyType(name, var);
	};

	addPropertySupport(zone, create);

	AsyncSetPropertyRequest request;
	request.property = name.toStdString();
	request.zoneFilter = zone;
	request.value = VehicleProperty::getPropertyTypeForPropertyNameValue(name.toStdString(), defaultValue.toString().toStdString());

	routingEngine->updateSupported(supported(), PropertyList(), &source);
	routingEngine->setProperty(request);
}

void BluemonkeySink::exportInterface(QString name, QJSValue properties)
{
	std::unordered_map<std::string, std::string> propertiesMap;

	QVariantList tempProps = properties.toVariant().toList();

	DebugOut() << "num keys: " << tempProps.size() << endl;

	Q_FOREACH(QVariant i, tempProps)
	{
		QVariantMap obj = i.toMap();
		propertiesMap[obj.firstKey().toStdString()] = obj.first().toString().toStdString();
	}

	DebugOut() << "exporting new dbus interface: " << name.toStdString() << endl;
	auto exporter = amb::Exporter::instance();
	exporter->exportProperty<DBusSink>(name.toStdString(), propertiesMap, routingEngine);
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
		else
		{
			DebugOut(DebugOut::Warning) << "Error, trying to set value: " << reply->error << endl;
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

			for(auto val : reply->values)
			{
				list.append(toQVariant(val));
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

QString Property::name()
{
	return mValue->name.c_str();
}

void Property::setType(QString t)
{
	if(mValue && name() != "")
		routingEngine->unsubscribeToProperty(name().toStdString(), this);

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
