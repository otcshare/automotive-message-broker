#ifndef _AMB_BLUEMONKEY_PLUGIN_H_
#define _AMB_BLUEMONKEY_PLUGIN_H_

#include "bluemonkey.h"

#include <abstractsource.h>
#include <ambpluginimpl.h>
#include <uuidhelper.h>

#include <map>


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
	void init();
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
	Bluemonkey* bluemonkey;
	Authenticate* auth;
	bool mSilentMode;
};

#endif
