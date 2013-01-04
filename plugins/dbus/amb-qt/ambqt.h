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
	Q_PROPERTY(QString property READ property WRITE setProperty)
	AUTOPROPERTY(QString, property, Property)
	Q_PROPERTY(QVariant value READ value NOTIFY propertyChanged)
	Q_PROPERTY(QString interface READ interface WRITE setInterface)
	AUTOPROPERTY(QString, interface, Interface)
	Q_PROPERTY(QString objectPath READ objectPath WRITE setObjectPath)
	AUTOPROPERTY(QString, objectPath, ObjectPath)

	public:

	AmbProperty() { }

	AmbProperty(QString op, QString iface, QString propName);

	QVariant value()
	{
		if(!mDBusInterface->isValid())
		{
			qDebug()<<"error Interface is not valid";
			return QVariant::Invalid;
		}

		QVariant value = mDBusInterface->property(property().toAscii().data());

		return value;
	}

signals:
	void propertyChanged(QVariant, double);

public slots:
	void propertyChangedSlot(QDBusVariant val, double ts);
	void connect();

private:
	QDBusInterface* mDBusInterface;
};

#endif // AMBQT_H
