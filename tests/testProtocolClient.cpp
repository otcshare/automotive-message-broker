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

	DebugOut(0) << "calling client->get()" << endl;
	c->get("interface1", [&c](amb::Object::Ptr obj)
	{
		DebugOut(0) << "get call reply" << endl;
		g_assert(obj->size() == 2);

		obj->emplace("vehicleSpeed", amb::make_shared(new VehicleProperty::VehicleSpeedType(69)));

		c->set("interface1", obj, [](bool s)
		{
			DebugOut(0) << "set call reply status: " << (s ? "success!" : "fail") << endl;
			g_assert(s);
		});
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

	socket.getSocket()->waitForConnected();

	DebugOut(0) << "We are connected!" << endl;

	amb::AmbRemoteClient client(&socket);

	runTest(&client);

	app.exec();
}
