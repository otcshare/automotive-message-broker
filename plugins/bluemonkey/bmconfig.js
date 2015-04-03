#!/usr/bin/env bluemonkey

console.log("hello world!");

bluemonkey.loadModule("@PLUGIN_INSTALL_PATH@/bluemonkeyDbModule.so");
bluemonkey.loadModule("@PLUGIN_INSTALL_PATH@/bluemonkeyDBusModule.so");
bluemonkey.loadModule("@PLUGIN_INSTALL_PATH@/bluemonkeyBleModule.so");
bluemonkey.loadModule("@PLUGIN_INSTALL_PATH@/bluemonkeyWsModule.so");

exportObj = {};
exportObj.foo = function() { return "bar"; }
exportObj.bar = true;

app = new Application();

app.main = function()
{
	console.log("running main()");

	if(dbus !== undefined)
	{
		try
		{
			var dbusIface = dbus.createInterface("org.freedesktop.DBus", "/", "org.freedesktop.DBus", dbus.Session);

			var reply = dbusIface.GetId();

			console.log("org.freedesktop.DBus.GetId() response: " + reply);

			var registered = dbus.registerService("org.bluemonkey", dbus.Session)

			bluemonkey.assertIsTrue(registered, "could not register service: " + dbus.errorMessage(dbus.Session));
		}
		catch(error)
		{
			console.log("nasty dbus errors");
		}
	}

	if(ble !== undefined)
	{
		serviceUuid = "5faaf494-d4c6-483e-b592-d1a6ffd436c9";
		ble.addService(serviceUuid, "5faaf495-d4c6-483e-b592-d1a6ffd436c9", "5faaf496-d4c6-483e-b592-d1a6ffd436c9");
		ble.scanningChanged.connect(function ()
		{
			if(!ble.scan)
			{
				console.log("scan finished");
			}
		});

		ble.scan = true;
		ble.devicesChanged.connect(function ()
		{
			console.log("devices that match the service uuid: " + ble.devices(serviceUuid))
		});
	}

	if(websockets !== undefined)
	{
		console.log("can has websockets!");

		server = new WebSocketServer();
		server.onconnection = function(s)
		{
			console.log("server can has new connection!");
			s.onmessage = function(msg)
			{
				console.log("server received msg: " + msg);
				s.send("pong");
			};
		};

		server.listen(8070);

		socket = new WebSocket("ws://localhost:8070");

		socket.onmessage = function(msg)
		{
			console.log("msg: "+ msg);
		};

		socket.onopen = function()
		{
			console.log("client connection opened!");
			socket.send("ping");
		};

		socket.onclose = function()
		{
			console.log("closed");
		};

		socket.onerror = function(err)
		{
			console.log("error: " + err);
		}

		socket.open();
	}
	else
	{
		console.log("no websocket support :(");
	}
}

app.run();
