#include "testProtocolCommon.h"

#include <debugout.h>
#include <jsonprotocol.h>

#include <QLocalSocket>
#include <QLocalServer>
#include <QCoreApplication>
#include <QObject>

#include <memory>

class Server : public amb::AmbRemoteServer
{
public:
	Server(QLocalSocket* socketConnection): amb::AmbRemoteServer(new DomainSocket(socketConnection), nullptr) {}


	// AmbRemoteServer interface
protected:
	void list(amb::ListMethodCall &call)
	{
		DebugOut(0) << "list called" << endl;

		amb::Object interface1("interface1");
		amb::Object interface2("interface2");

		interface1.emplace("vehicleSpeed", std::shared_ptr<AbstractPropertyType>(new VehicleProperty::VehicleSpeedType(100)));

		call.objectNames.push_back(interface1);
		call.objectNames.push_back(interface2);
		call.success = true;

		send(call);
	}
	void get(amb::GetMethodCall &get)
	{
		DebugOut(0) << "get called" << endl;
	}
};

int main(int argc, char** argv)
{
	DebugOut::setDebugThreshhold(7);
	DebugOut::setThrowErr(true);
	DebugOut::setThrowWarn(true);

	DebugOut(0) << "Testing AMB json server/client" << endl;

	QCoreApplication app(argc, argv);

	QLocalServer server;

	QLocalServer::removeServer("/tmp/amb");

	if(!server.listen("/tmp/amb"))
	{
		DebugOut(DebugOut::Error) << server.errorString().toStdString() << endl;
	}


	DebugOut(0) << "parent waiting for new connection..." << endl;
	server.waitForNewConnection(-1);
	QLocalSocket *clientSocket = server.nextPendingConnection();

	g_assert(clientSocket);

	Server *s = new Server(clientSocket);

	app.exec();
}
