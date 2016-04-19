/*
	Copyright (C) 2012  Intel Corporation

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include <QObject>
#include <QVariant>
#include <QStringList>
#include <QSslError>
#include <map>
#include <QAbstractSocket>

#include <IrcSession>
#include <IrcMessage>

class IrcCommunication: public IrcSession
{
	Q_OBJECT
	Q_PROPERTY(QStringList channels READ channels WRITE setChannels)
	Q_PROPERTY(bool ssl READ ssl WRITE setSsl)
public:
	IrcCommunication(std::map<std::string, std::string> config, QObject* parent=0);
	QStringList channels() { return mChannels; }
	void setChannels(QStringList c) { mChannels = c; }

	void announceDequeue();

	bool ssl() { return isSecure(); }

public Q_SLOTS:
	void respond(QString target, QString msg);
	void announce(QString);
	void connect(QString host,int port, QString proxy, QString user, QString nick, QString pass);
	void setSsl(bool use);
	void setIgnoreInvalidCert(bool ignore);
	void join(QString channel);

	void debugMessage(QString v) { qDebug()<<v; }

	void reconnect();

private Q_SLOTS:
	void onMessageReceived(IrcMessage*);
	void sslError(QList<QSslError>);
	void socketError(QAbstractSocket::SocketError);

Q_SIGNALS:
	void message(QString sender, QString prefix, QString codes);

private:
	QStringList announceQueue;
	QStringList mChannels;
};

#endif // COMMUNICATION_H
