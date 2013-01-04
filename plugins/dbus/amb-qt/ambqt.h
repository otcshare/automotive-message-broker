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
		if(!mInterface->isValid())
		{
			qDebug()<<"error Interface is not valid";
			return QVariant::Invalid;
		}

		QVariant value = mInterface->property(mPropertyName.toAscii().data());

		return value;
	}

signals:
	void propertyChanged(QVariant, double);

public slots:
	void propertyChangedSlot(QDBusVariant val, double ts);

private:
	QString mPropertyName;
	QDBusInterface* mInterface;
};

#endif // AMBQT_H
