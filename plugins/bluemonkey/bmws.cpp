#include "bmws.h"

#include <map>
#include <string>
#include <QObject>
#include <QString>
#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>

#include <QCoreApplication>

extern "C" void create(std::map<std::string, std::string> config, std::map<std::string, QObject*> &exports, QString &js, QObject* parent)
{
	exports["websockets"] = new Websockets(nullptr);

	js.append("function WebSocket(url, protocols)"
			  "{"
			  "  this.client = websockets.createClient(url, protocols);"
			  "  this._binaryType = 'blob';"
			  "  Object.defineProperty(this, 'binaryType', { enumerable : false, get : function() { return this._binaryType; }, set : function (val)"
			  "  {"
			  "    if(val !=='blob' || val !== 'arraybuffer')"
			  "      throw \"Invalid binaryType.  Must be 'blob' or 'arraybuffer'\";"
			  "    this._binaryType = val;"
			  "  } });"
			  "  Object.defineProperty(this, 'bufferedAmount', { value : 0 });"
			  "  Object.defineProperty(this, 'extensions', { value : ''});"
			  "  Object.defineProperty(this, 'protocol', { value : ''});"
			  "  Object.defineProperty(this, 'readyState', { enumberable : false, get : function ()"
			  "  { "
			  "    ready = this.client.readyState; "
			  "    if(ready == -1)"
			  "      return undefined;"
			  "    return ready;"
			  "  } });"
			  "  Object.defineProperty(this, 'url', { get : function() { return this.client.url; }});"
			  "  temporaryMe = this;"
			  "  this.client.connected.connect(function()"
			  "  {"
			  "    if(temporaryMe.onopen !== undefined)"
			  "      temporaryMe.onopen();"
			  "    else"
			  "      console.log('onopen is like, totally undefined');"
			  "  });"
			  ""
			  "  this.client.disconnected.connect(function()"
			  "  {"
			  "    if(temporaryMe.onclose !== undefined)"
			  "      temporaryMe.onclose()"
			  "  });"
			  ""
			  "  this.client.error.connect(function(err)"
			  "  {"
			  "    if(temporaryMe.onerror !== undefined)"
			  "      temporaryMe.onerror(temporaryMe.client.getErrorString());"
			  "  });"
			  ""
			  "  this.client.textMessageReceived.connect(function(msg)"
			  "  {"
			  "    if(temporaryMe.onmessage !== undefined)"
			  "    {"
			  "      temporaryMe._binaryType = 'blob';"
			  "      temporaryMe.onmessage(msg);"
			  "    }"
			  "  });"
			  ""
			  "  this.client.binaryMessageReceived.connect(function(msg)"
			  "  {"
			  "    if(temporaryMe.onmessage !== undefined)"
			  "    {"
			  "      temporaryMe._binaryType = 'arraybuffer';"
			  "      temporaryMe.onmessage(msg);"
			  "    }"
			  "  });"
			  "}"
			  ""
			  "WebSocket.prototype.send = function(msg)"
			  "{"
			  "  if(this._binaryType === 'blob')"
			  "    this.client._sendTextMessage(msg);"
			  "  else if(this._binaryType === 'arraybuffer')"
			  "    this.client._sendBinaryMessage(msg);"
			  "};"
			  ""
			  "WebSocket.prototype.close = function() { this.client.close(); };"
			  "");

}




Websockets::Websockets(QObject *parent)
	:QObject(parent)
{

}

QObject *Websockets::createClient(const QString &url, const QStringList &protocols)
{
	return new WebsocketClient(url, nullptr);
}

QObject *Websockets::createServer()
{

}


WebsocketClient::WebsocketClient(const QString &url, QObject *parent)
	:QWebSocket(QString(), QWebSocketProtocol::VersionLatest, parent), mUrl(url)
{
	qDebug() << "Constructed websocket client";

	connect(this, &WebsocketClient::connected, []()
	{
		qDebug() << "connected";
	});

	connect(this, &WebsocketClient::disconnected, []()
	{
		qDebug() << "disconnected";
	});

	connect(this, &WebsocketClient::stateChanged, [](QAbstractSocket::SocketState state)
	{
		qDebug() << "state changed: " << state;
	});

	open(QUrl(url));
}

int WebsocketClient::readyState()
{
	if(state() == QAbstractSocket::UnconnectedState)
	{
		return 3;
	}
	else if(state() == QAbstractSocket::ConnectingState)
	{
		return 0;
	}
	else if(state() == QAbstractSocket::ClosingState)
	{
		return 2;
	}
	else if(state() == QAbstractSocket::ConnectedState)
	{
		return 1;
	}
	return -1;
}

int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);
	WebsocketClient client("ws://echo.websocket.org");

	QObject::connect(&client, &WebsocketClient::connected, [&client]()
	{
		qDebug() << "Can has connection!";

		client.sendTextMessage("Hello world!");
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
}

int WebsocketServer::secureMode()
{
	return mServer->secureMode();
}

void WebsocketServer::listen(quint16 port)
{

}
