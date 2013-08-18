#include "irccoms.h"
#include <QtDebug>
#include <IrcCommand>
#include <IrcMessage>
#include <QSslSocket>
#include <QFile>
#include <QNetworkProxy>
#include <QTimer>
#include <QScriptEngine>

#define foreach Q_FOREACH

IrcCommunication::IrcCommunication(std::map<std::string, std::string> config, QObject* parent)
	:QObject(parent)
{

	session = new IrcSession(this);
	mSsl=false;

	QObject::connect(session, &IrcSession::connected, [this](){
		connectedChanged();
		connected();
	});

	QObject::connect(session, &IrcSession::disconnected, [this](){
		connectedChanged();
		disconnected();
		reconnect();
	});

	QObject::connect(session,SIGNAL(socketError(QAbstractSocket::SocketError)),this,SLOT(socketError(QAbstractSocket::SocketError)));
	QObject::connect(session,SIGNAL(connecting()),this,SIGNAL(connecting()));
	QObject::connect(session,SIGNAL(messageReceived(IrcMessage*)),this,SLOT(messageReceived(IrcMessage*)));

	QScriptEngine *engine = new QScriptEngine(this);

	QScriptValue eventEngineValue = engine->newQObject(this);
	engine->globalObject().setProperty("irc", eventEngineValue);

	QString str = config["ircSettings"].c_str();

	QFile file(str);
	if(!file.open(QIODevice::ReadOnly))
	{
		qDebug()<<"failed to open irc config file: "<<str;
		return;
	}

	QString script = file.readAll();

	file.close();

	engine->evaluate(script);

}

bool IrcCommunication::isConnected()
{
	return session->isConnected();
}

void IrcCommunication::announce(QString s)
{
	qDebug()<<"channels: "<<mChannels;
	foreach(QString channel, mChannels)
	{
		qDebug()<<"sending "<<s<<" to channel: "<<channel;
		IrcCommand command;
		command.setType(IrcCommand::Message);
		command.setParameters(QStringList()<<channel<<s);
		session->sendCommand(&command);
	}
}

void IrcCommunication::respond(QString target, QString s)
{
	IrcCommand *command = IrcCommand::createMessage(target,s);
	session->sendCommand(command);

	delete command;

}

void IrcCommunication::messageReceived(IrcMessage *msg)
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
	session->setHost(host);
	session->setPort(port);
	session->setUserName(user);
	session->setNickName(nick);
	session->setRealName(nick);

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
	session->open();
}

void IrcCommunication::setSsl(bool use)
{
	if(use)
		session->setSocket(new QSslSocket(this));
}

void IrcCommunication::join(QString channel)
{
	if(!mChannels.contains(channel))
		mChannels.append(channel);

	IrcCommand command;
	command.setType(IrcCommand::Join);
	command.setParameters(QStringList()<<channel);
	session->sendCommand(&command);
}

void IrcCommunication::reconnect()
{
	if(session->socket()->state() == QAbstractSocket::ConnectingState)
		QTimer::singleShot(5000,this,SLOT(reconnect()));
	else
		QTimer::singleShot(5000,session,SLOT(open()));
}

/*void IrcCommunication::sslError(QList<QSslError>)
{
	qDebug()<<"some ssl errors!! trying to ignore them";
	QSslSocket* socket = qobject_cast<QSslSocket*>(session->socket());
	if(socket)
	{
		socket->ignoreSslErrors();
	}
}*/

void IrcCommunication::socketError(QAbstractSocket::SocketError error)
{
	qDebug()<<"Socket error.  attempting to reconnect..."<<error;
	reconnect();
}

void IrcCommunication::setIgnoreInvalidCert(bool ignore)
{
	if(ignore)
		QObject::connect(session->socket(),SIGNAL(),this,SLOT(sslError(QList<QSslError>)));
}
