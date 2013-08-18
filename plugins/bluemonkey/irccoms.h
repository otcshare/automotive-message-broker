#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include <QObject>
#include <QVariant>
#include <QStringList>
#include <map>

#include <IrcSession>
#include <IrcMessage>

class IrcCommunication: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QStringList channels READ channels WRITE setChannels)
	Q_PROPERTY(bool ssl WRITE setSsl)
	Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectedChanged)
public:
	IrcCommunication(std::map<std::string, std::string> config, QObject* parent=0);
	QStringList channels() { return mChannels; }
	void setChannels(QStringList c) { mChannels = c; }

	bool isConnected();

public Q_SLOTS:
	void respond(QString target, QString msg);
	void announce(QString);
	void connect(QString host,int port, QString proxy, QString user, QString nick, QString pass);
	void setSsl(bool use);
	void setIgnoreInvalidCert(bool ignore);
	void join(QString channel);

	void reconnect();

private Q_SLOTS:
	void messageReceived(IrcMessage*);
//	void sslError(QList<QSslError>);
	void socketError(QAbstractSocket::SocketError);

Q_SIGNALS:
	void message(QString sender, QString prefix, QString codes);
	void connecting();
	void connected();
	void disconnected();
	void connectedChanged();

private:
	bool mSsl;
	QStringList mChannels;
	IrcSession *session;
};

#endif // COMMUNICATION_H
