#include "testProtocolCommon.h"

#include <debugout.h>
#include <jsonprotocol.h>

#include <QLocalSocket>
#include <QLocalServer>
#include <QCoreApplication>
#include <QObject>

#include <memory>

void runTest(amb::AmbRemoteClient *c)
{
	DebugOut(0) << "calling client->list()" << endl;
	c->list([](std::vector<amb::Object::Ptr> supported)
	{
		DebugOut(0) << "list call reply" << endl;
		g_assert(supported.size() == 2);
	});

	c->subscribe("interface1", [](amb::Object::Ptr obj)
	{
		DebugOut(0) << obj->interfaceName << " changed!" << endl;
	});

	DebugOut(0) << "calling client->get()" << endl;
	c->get("interface1", [](amb::Object::Ptr obj)
	{
		DebugOut(0) << "get call reply" << endl;
		g_assert(obj->size() == 2);

		obj->emplace("vehicleSpeed", amb::make_shared(new VehicleProperty::VehicleSpeedType(69)));

	});

	amb::Object::Ptr obj = amb::Object::create();

	obj->interfaceName = "interface1";
	obj->emplace("vehicleSpeed", amb::make_shared(new VehicleProperty::VehicleSpeedType(22)));

	c->set("interface1", obj, [](bool s)
	{
		DebugOut(0) << "set call reply status: " << (s ? "success!" : "fail") << endl;
		g_assert(s);
	});
}

int main(int argc, char** argv)
{
	DebugOut::setDebugThreshhold(7);
	DebugOut::setThrowErr(true);
	DebugOut::setThrowWarn(false);

	DebugOut(0) << "Testing AMB json server/client" << endl;

	QCoreApplication app(argc, argv);

	DomainSocket socket;

	socket.open();

	if(!socket.getSocket()->waitForConnected())
	{
		DebugOut("Could not connect");
		return -1;
	}

	DebugOut(0) << "We are connected!" << endl;

	amb::AmbRemoteClient client(&socket);

	runTest(&client);

	app.exec();
}
