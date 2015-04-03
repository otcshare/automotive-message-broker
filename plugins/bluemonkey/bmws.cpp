#include "bmws.h"

#include <map>
#include <string>
#include <QObject>
#include <QString>
#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>

#include <QCoreApplication>

QString BinaryType::Blob = "blob";
QString BinaryType::ArrayBuffer = "arraybuffer";

bool BinaryType::validType(const QString &t)
{
	return t == Blob || t == ArrayBuffer;
}

extern const char bmws_api[];

extern "C" void create(std::map<std::string, std::string> config, std::map<std::string, QObject*> &exports, QString &js, QObject* parent)
{
	exports["websockets"] = new Websockets(parent);

	js.append(bmws_api);
}

Websockets::Websockets(QObject *parent)
	:QObject(parent)
{

}

QObject *Websockets::createClient(const QString &url, const QStringList &protocols)
{
	return new WebsocketClient(url, nullptr);
}

QObject *Websockets::createServer(bool useSsl)
{
	return new WebsocketServer(useSsl, this);
}


WebsocketClient::WebsocketClient(const QString &url, QObject *parent, QWebSocket * ws)
	:QObject(parent), mUrl(url), mBinaryType("blob"), mSocket(ws)
{
	if(!mSocket)
		mSocket = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);
	else //reparent to this
		mSocket->setParent(this);

	connect(mSocket, &QWebSocket::connected, this, &WebsocketClient::connected);
	connect(mSocket, &QWebSocket::disconnected, this, &WebsocketClient::disconnected);
	connect(mSocket, &QWebSocket::stateChanged, this, &WebsocketClient::stateChanged);
	connect(mSocket, &QWebSocket::textMessageReceived, [this](const QString & msg)
	{
		mBinaryType = BinaryType::Blob;
		textMessageReceived(msg);
	});
	connect(mSocket, &QWebSocket::binaryMessageReceived, [this](const QByteArray & msg)
	{
		mBinaryType = BinaryType::ArrayBuffer;
		binaryMessageReceived(msg);
	});
}

WebsocketClient::WebsocketClient(QWebSocket *other, QObject* parent)
	:WebsocketClient("", parent, other)
{

}

WebsocketClient::~WebsocketClient()
{
	mSocket->close();
}

int WebsocketClient::readyState()
{
	if(mSocket->state() == QAbstractSocket::UnconnectedState)
	{
		return 3;
	}
	else if(mSocket->state() == QAbstractSocket::ConnectingState)
	{
		return 0;
	}
	else if(mSocket->state() == QAbstractSocket::ClosingState)
	{
		return 2;
	}
	else if(mSocket->state() == QAbstractSocket::ConnectedState)
	{
		return 1;
	}
	return -1;
}

void WebsocketClient::open()
{
	mSocket->open(mUrl);
}

void WebsocketClient::send(const QByteArray &msg)
{
	if(mBinaryType == BinaryType::Blob)
	{
		mSocket->sendTextMessage(msg);
	}
	else
	{
		mSocket->sendBinaryMessage(msg);
	}
}

int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);
	WebsocketClient client("ws://echo.websocket.org");

	QObject::connect(&client, &WebsocketClient::connected, [&client]()
	{
		qDebug() << "Can has connection!";
		client.send("Hello world!");
	});

	QObject::connect(&client, &WebsocketClient::textMessageReceived, [](const QString & msg)
	{
		qDebug() << "Can has message: " << msg;
	});

	/*QObject::connect(&client, &WebsocketClient::error, [&client](QAbstractSocket::SocketError)
	{
		qDebug() << "Error: " << client.errorString();
	});*/

	QObject::connect(&client, &WebsocketClient::disconnected, []()
	{
		qDebug() << "has disconnected :(";
	});

	return app.exec();
}


WebsocketServer::WebsocketServer(bool ssl, QObject *parent)
	:QObject(parent), mServer(nullptr)
{
	mServer = new QWebSocketServer("", ssl ? QWebSocketServer::SecureMode : QWebSocketServer::NonSecureMode, this);

	connect(mServer, &QWebSocketServer::newConnection, [this]()
	{
		QWebSocket * socket = mServer->nextPendingConnection();

		auto client = new WebsocketClient(socket, this);

		newConnection(client);
	});
}

WebsocketServer::~WebsocketServer()
{
	close();
}

int WebsocketServer::secureMode()
{
	return mServer->secureMode();
}

void WebsocketServer::listen(quint16 port)
{
	mServer->listen(QHostAddress::Any, port);
}

void WebsocketServer::close()
{
	mServer->close();
}




