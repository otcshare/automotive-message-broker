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
	Q_PROPERTY(QVariant value READ value NOTIFY valueChanged)
	Q_PROPERTY(QString interfaceName READ interfaceName WRITE setInterfaceName)
	AUTOPROPERTY(QString, interfaceName, InterfaceName)
	Q_PROPERTY(QString objectPath READ objectPath WRITE setObjectPath)
	AUTOPROPERTY(QString, objectPath, ObjectPath)

	public:

		AmbProperty():mDBusInterface(NULL) { }

	AmbProperty(QString op, QString iface, QString propName);

	QVariant value()
	{
		if(!mDBusInterface || !mDBusInterface->isValid())
		{
			qDebug()<<"error Interface is not valid";
			return QVariant::Invalid;
		}

		QVariant value = mDBusInterface->property(propertyName().toAscii().data());

		return value;
	}

signals:
	void propertyChanged(QVariant, double);
	void valueChanged(QVariant);

public slots:
	void propertyChangedSlot(QDBusVariant val, double ts);
	void connect();

private:
	QDBusInterface* mDBusInterface;
};

#endif // AMBQT_H
