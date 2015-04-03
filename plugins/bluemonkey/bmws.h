#ifndef BM_WS_H_
#define BM_WS_H_

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>

#include <memory>

class Websockets : public QObject
{
	Q_OBJECT
public:
	Websockets(QObject* parent = nullptr);

public Q_SLOTS:
	QObject * createClient(const QString & url, const QStringList  &protocols);
	QObject * createServer(bool useSsl = false);
};

namespace BinaryType
{
	extern QString Blob;
	extern QString ArrayBuffer;

	extern bool validType(const QString & t);
}

class WebsocketClient : public QObject
{
	Q_OBJECT
	Q_PROPERTY(int readyState READ readyState)
	Q_PROPERTY(QString url READ getUrl)
	Q_PROPERTY(QString binaryType READ binaryType WRITE setBinaryType)

public:
	WebsocketClient(const QString & url, QObject* parent = nullptr, QWebSocket *ws = nullptr);
	WebsocketClient(QWebSocket * other, QObject *parent = nullptr);

	~WebsocketClient();

	int readyState();
	QString getUrl() { return mUrl; }

	QString binaryType() { return mBinaryType; }
	void setBinaryType(const QString & bt)
	{
		if(!BinaryType::validType(bt))
			return;

		mBinaryType = bt;
	}

public Q_SLOTS:
	void open();
	void send(const QByteArray &msg);
	void close() { mSocket->close(); }

	QString errorString() { return mSocket->errorString(); }

Q_SIGNALS:

	void connected();
	void disconnected();
	void stateChanged(QAbstractSocket::SocketState state);
	void textMessageReceived(const QString &message);
	void binaryMessageReceived(const QByteArray &message);
	void error(QAbstractSocket::SocketError error);

private:
	QString mUrl;
	QString mBinaryType;
	QWebSocket * mSocket;
};

class WebsocketServer : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString serverName READ serverName WRITE setServerName)
	Q_PROPERTY(int secureMode READ secureMode)

public:
	WebsocketServer(bool ssl = false, QObject * parent = nullptr);
	~WebsocketServer();

	QString serverName() { return mServer->serverName(); }
	void setServerName(const QString & n) { mServer->setServerName(n); }

	int secureMode();
	void setSecureMode(int mode);

public Q_SLOTS:

	void listen(quint16 port);
	void close();

Q_SIGNALS:
	void newConnection(QObject * socket);

private:
	QWebSocketServer * mServer;
};

#endif
