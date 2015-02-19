#ifndef BM_WS_H_
#define BM_WS_H_

#include <QObject>
#include <QtWebSockets/QWebSocket>

class Websockets : public QObject
{
	Q_OBJECT
public:
	Websockets(QObject* parent = nullptr);

public Q_SLOTS:
	QObject * createClient(const QString & url, const QStringList  &protocols);
	QObject * createServer();
};

class WebsocketClient : public QWebSocket
{
	Q_OBJECT
	Q_PROPERTY(int readyState READ readyState)
	Q_PROPERTY(QString url READ getUrl)
public:
	WebsocketClient(const QString & url, QObject* parent = nullptr);

	int readyState();
	QString getUrl() { return mUrl; }

public Q_SLOTS:

	QString getErrorString() { return errorString(); }
	void _sendTextMessage(const QString & msg) { sendTextMessage(msg); }
	void _sendBinaryMessage(const QByteArray & msg) { sendBinaryMessage(msg); }

private:
	QString mUrl;
};

#endif
