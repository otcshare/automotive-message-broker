#ifndef AMBQT_H
#define AMBQT_H

#include <QObject>
#include <QDBusInterface>
#include <QtDebug>

class QDBusInterface;

class AmbProperty: public QObject
{
	Q_OBJECT
public:
	AmbProperty(QString objectPath, QString interface, QString propertyName);

	QVariant operator()() const
	{
		QVariant value = mInterface->property(mPropertyName.toAscii().data());

		qDebug()<<mInterface->lastError().message();

		return value;
	}

signals:
	void propertyChanged(QVariant);

public slots:
	void propertyChangedSlot(QDBusVariant val);

private:
	QString mPropertyName;
	QDBusInterface* mInterface;
};

#endif // AMBQT_H
