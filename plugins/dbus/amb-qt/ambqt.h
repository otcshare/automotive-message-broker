#ifndef AMBQT_H
#define AMBQT_H

#include <QObject>
#include <QDBusInterface>
#include <QtDebug>

#define AUTOPROPERTY(type, name, Name) \
	public: \
	void set ## Name(type s) { m ## Name = s; } \
	type name() { return m ## Name; } \
	private: \
	type m ## Name;

class QDBusInterface;

class AmbProperty: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString propertyName READ propertyName WRITE setPropertyName)
	AUTOPROPERTY(QString, propertyName, PropertyName)
	Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
	Q_PROPERTY(QString interfaceName READ interfaceName WRITE setInterfaceName)
	AUTOPROPERTY(QString, interfaceName, InterfaceName)
	Q_PROPERTY(QString objectPath READ objectPath WRITE setObjectPath)
	AUTOPROPERTY(QString, objectPath, ObjectPath)
	Q_PROPERTY(int zone READ zone WRITE setZone)
	AUTOPROPERTY(int, zone, Zone)
	Q_PROPERTY(double time READ time)


	public:

		AmbProperty():mDBusInterface(NULL),mZone(0),mTime(0) { }

	AmbProperty(QString op, QString iface, QString propName);

	QVariant value()
	{
		return mValue;
	}

	void setValue(QVariant v)
	{
		if(!mDBusInterface || !mDBusInterface->isValid())
		{
			qDebug()<<"error Interface is not valid "<<interfaceName();
		}

		mDBusInterface->setProperty(propertyName().toUtf8(), v);
	}

	double time(){ return mTime; }

Q_SIGNALS:	
	void valueChanged(QVariant val);

	///TODO: remove
	void signalChanged(QVariant val);

public Q_SLOTS:
	void connect();

private Q_SLOTS:
	void propertyChangedSlot(QString, QVariantMap values, QVariantMap);
	void propertyChanged1(QDBusVariant, double);

private:
	void getObjectPath();
	QDBusInterface* mDBusInterface;
	double mTime;
	QVariant mValue;
};

#endif // AMBQT_H
