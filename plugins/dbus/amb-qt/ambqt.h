#ifndef AMBQT_H
#define AMBQT_H

#include <QObject>
#include <QDBusInterface>

class QDBusInterface;

class AmbProperty: public QObject
{
	Q_OBJECT
public:
	AmbProperty(QString objectPath, QString interface, QString propertyName);

	QVariant operator()() const
	{
		return mInterface->property(mPropertyName.toAscii().data());
	}

signals:
	void propertyChanged(QVariant);

private:
	QString mPropertyName;
	QDBusInterface* mInterface;
};

#endif // AMBQT_H
