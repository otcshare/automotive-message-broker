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
	Server(QLocalSocket* socketConnection)
		: amb::AmbRemoteServer(new DomainSocket(socketConnection), nullptr),
		  speed(100), engineSpeed(1999)
	{}


	// AmbRemoteServer interface
protected:
	void list(amb::ListMethodCallPtr call)
	{
		DebugOut(0) << "list called" << endl;

		amb::Object::ObjectPtr interface1(new amb::Object("interface1"));
		amb::Object::ObjectPtr interface2( new amb::Object("interface2"));

		interface1->emplace("vehicleSpeed", std::shared_ptr<AbstractPropertyType>(new VehicleProperty::VehicleSpeedType(speed)));
		interface1->emplace("engineSpeed", std::shared_ptr<AbstractPropertyType>(new VehicleProperty::EngineSpeedType(engineSpeed)));

		interface2->emplace("engineSpeed", std::shared_ptr<AbstractPropertyType>(new VehicleProperty::EngineSpeedType(3099)));

		call->objectNames.push_back(interface1);
		call->objectNames.push_back(interface2);
		amb::MethodReply<amb::ListMethodCall> reply(call, true);

		send(reply);
	}
	void get(amb::GetMethodCallPtr get)
	{
		DebugOut(0) << "get called" << endl;

		if(get->value->interfaceName == "interface1")
		{
			amb::Object::ObjectPtr interface1(new amb::Object("interface1"));

			interface1->emplace("vehicleSpeed", std::shared_ptr<AbstractPropertyType>(new VehicleProperty::VehicleSpeedType(100)));
			interface1->emplace("engineSpeed", std::shared_ptr<AbstractPropertyType>(new VehicleProperty::EngineSpeedType(1999)));
			get->value = interface1;
			amb::MethodReply<amb::GetMethodCall> reply(get, true);
			send(reply);
		}
		else if(get->value->interfaceName == "interface2")
		{
			amb::Object::ObjectPtr interface2(new amb::Object("interface2"));
			interface2->emplace("engineSpeed", std::shared_ptr<AbstractPropertyType>(new VehicleProperty::EngineSpeedType(3099)));
			get->value = interface2;
			amb::MethodReply<amb::GetMethodCall> reply(get, true);
			send(reply);
		}
	}
	void set(amb::SetMethodCallPtr set)
	{
		if(set->value->interfaceName == "interface1")
		{
			speed = set->value->at("vehicleSpeed")->value<uint16_t>();

			DebugOut(0) << "Speed set to " << speed << endl;
			amb::MethodReply<amb::SetMethodCall> reply (set, true);
			send(reply);
		}
		else
		{
			amb::MethodReply<amb::SetMethodCall> reply (set, false);
			send(reply);
		}
	}

	uint16_t speed;
	uint16_t engineSpeed;
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
