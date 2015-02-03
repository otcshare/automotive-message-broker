#ifndef TESTS_PROTOCOL_COMMON_H_
#define TESTS_PROTOCOL_COMMON_H_

#include <jsonprotocol.h>
#include <abstractio.hpp>
#include <QLocalSocket>

class DomainSocket : public AbstractIo
{
public:

	DomainSocket() { }
	DomainSocket(QLocalSocket* sock): socket(sock) { }
	// AbstractIo interface
public:
	bool open()
	{
		if(!socket)
			socket = std::shared_ptr<QLocalSocket>(new QLocalSocket());

		socket->connectToServer("/tmp/amb");

		return socket->errorString().isEmpty();
	}
	bool close()
	{
		if(socket)
			socket->close();

		return true;
	}
	bool isOpen()
	{
		return socket && socket->state() == QLocalSocket::ConnectedState;
	}

	string read()
	{
		return socket->readAll().data();
	}

	void write(const string & data)
	{
		socket->write(data.c_str(), data.length());
	}

	int fileDescriptor()
	{
		return socket->socketDescriptor();
	}

	std::string errorString()
	{
		return socket->errorString().toStdString();
	}

	QLocalSocket *getSocket() { return socket.get(); }

private:
	std::shared_ptr<QLocalSocket> socket;
};

#endif
