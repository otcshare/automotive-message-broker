#include "irccoms.h"
#include <QtDebug>
#include <IrcCommand>
#include <IrcMessage>
#include <QSslSocket>
#include <QSslError>
#include <QFile>
#include <QNetworkProxy>
#include <QTimer>
#include <QScriptEngine>
#include <debugout.h>

#define foreach Q_FOREACH

IrcCommunication::IrcCommunication(std::map<std::string, std::string> config, QObject* parent)
	:IrcSession(parent)
{

	QObject::connect(this, &IrcCommunication::connected, [this](){
		announceDequeue();
		connectedChanged(true);
	});

	QObject::connect(this, &IrcCommunication::disconnected, [this](){
		connectedChanged(false);
		reconnect();
	});

	QObject::connect(this,SIGNAL(socketError(QAbstractSocket::SocketError)),this,SLOT(socketError(QAbstractSocket::SocketError)));
	QObject::connect(this,SIGNAL(messageReceived(IrcMessage*)),this,SLOT(onMessageReceived(IrcMessage*)));

	QScriptEngine *engine = new QScriptEngine(this);

	QScriptValue eventEngineValue = engine->newQObject(this);
	engine->globalObject().setProperty("irc", eventEngineValue);

	QString str = config["ircSettings"].c_str();

	QFile file(str);
	if(!file.open(QIODevice::ReadOnly))
	{
		DebugOut(DebugOut::Error)<<"failed to open irc config file: "<<str.toStdString()<<endl;
		return;
	}

	QString script = file.readAll();

	file.close();

	QScriptValue response = engine->evaluate(script);

	DebugOut()<<response.toString().toStdString()<<endl;

}

void IrcCommunication::announceDequeue()
{
	foreach(QString s, announceQueue)
	{
		announce(s);
	}
	announceQueue.clear();
}

void IrcCommunication::announce(QString s)
{
	if(!isConnected())
	{
		announceQueue.append(s);
		return;
	}

	qDebug()<<"channels: "<<mChannels;
	foreach(QString channel, mChannels)
	{
		qDebug()<<"sending "<<s<<" to channel: "<<channel;
		IrcCommand command;
		command.setType(IrcCommand::Message);
		command.setParameters(QStringList()<<channel<<s);
		sendCommand(&command);
	}
}

void IrcCommunication::respond(QString target, QString s)
{
	IrcCommand *command = IrcCommand::createMessage(target,s);
	sendCommand(command);

	delete command;

}

void IrcCommunication::onMessageReceived(IrcMessage *msg)
{
	qDebug()<<"message received "<<msg->type()<<" prefix: "<<msg->sender().prefix()<<" params:"<<msg->parameters();

	if(msg->type() == IrcMessage::Private)
	{
		if(msg->parameters().count() > 1)
		{
			QString sender = msg->parameters().at(0);
			QString m = msg->parameters().at(1);

			message(sender, msg->sender().prefix(), m);
		}

	}
}

void IrcCommunication::connect(QString host, int port, QString proxy, QString user, QString nick, QString pass)
{
	setHost(host);
	setPort(port);
	setUserName(user);
	setNickName(nick);
	setRealName(nick);

	if(!proxy.isEmpty())
	{
		if(!proxy.contains(":"))
		{
			qDebug("proxy format must be 'address:port'");
			return;
		}
		QString host = proxy.split(":").at(0);
		int port = proxy.split(":").at(1).toInt();

		QNetworkProxy netproxy;
		netproxy.setType(QNetworkProxy::Socks5Proxy);
		netproxy.setHostName(host);
		netproxy.setPort(port);

		QNetworkProxy::setApplicationProxy(netproxy);
	}

	qDebug()<<"opening irc session";
	open();
}

void IrcCommunication::setSsl(bool use)
{
	if(use)
	{
		setSecure(true);
	}
}

void IrcCommunication::join(QString channel)
{
	if(!mChannels.contains(channel))
		mChannels.append(channel);

	IrcCommand command;
	command.setType(IrcCommand::Join);
	command.setParameters(QStringList()<<channel);
	sendCommand(&command);
}

void IrcCommunication::reconnect()
{
	if(socket()->state() == QAbstractSocket::ConnectedState)
		return;

	if(socket()->state() == QAbstractSocket::ConnectingState && socket()->state() != QAbstractSocket::ConnectedState)
		QTimer::singleShot(5000,this,SLOT(reconnect()));
	else
		QTimer::singleShot(5000,this,SLOT(open()));
}

void IrcCommunication::sslError(QList<QSslError> )
{
	qDebug()<<"some ssl errors!! trying to ignore them";
	QSslSocket* sock = qobject_cast<QSslSocket*>(socket());
	if(sock)
	{
		sock->ignoreSslErrors();
	}
}

void IrcCommunication::socketError(QAbstractSocket::SocketError error)
{
	qDebug()<<"Socket error.  attempting to reconnect..."<<error;
	reconnect();
}

void IrcCommunication::setIgnoreInvalidCert(bool ignore)
{
	if(ignore)
		QObject::connect(socket(),SIGNAL(sslErrors(QList<QSslError>)),this,SLOT(sslError(QList<QSslError>)));
}
