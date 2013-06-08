#ifndef AUTHENTICATE_H
#define AUTHENTICATE_H

#include <QObject>
#include <QStringList>
#include <map>
#include <string>

class Authenticate : public QObject
{
    Q_OBJECT
	Q_PROPERTY(QString pin READ pin WRITE setPin)
public:
	explicit Authenticate(std::map<std::string, std::string> config, QObject *parent = 0);
	void setPin(QString p) { mPin = p; }
	QString pin() { return mPin; }


public Q_SLOTS:
	bool isAuthorized(QString prefix);
	bool authorize(QString prefix, QString pin);

private:
	QStringList authorized;
	QString mPin;
};

#endif // AUTHENTICATE_H
