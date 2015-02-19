bluemonkey.log("hello world!");

bluemonkey.loadModule("@PLUGIN_INSTALL_PATH@/bluemonkeyDbModule.so");
bluemonkey.loadModule("@PLUGIN_INSTALL_PATH@/bluemonkeyDBusModule.so");
bluemonkey.loadModule("@PLUGIN_INSTALL_PATH@/bluemonkeyBleModule.so");

exportObj = {};
exportObj.foo = function() { return "bar"; }
exportObj.bar = true;

if(dbus !== undefined)
{
	try
	{
		var dbusIface = dbus.createInterface("org.freedesktop.DBus", "/", "org.freedesktop.DBus", dbus.Session);

		var reply = dbusIface.GetId();

		bluemonkey.log("org.freedesktop.DBus.GetId() response: " + reply);

		var registered = dbus.registerService("org.bluemonkey", dbus.Session)

		bluemonkey.assertIsTrue(registered, "could not register service: " + dbus.errorMessage(dbus.Session));

		/* TODO: Uncomment this when dbus export works
		var exported = dbus.exportObject("/one", "org.awesome.interface", dbus.Session, exportObj);
		bluemonkey.log("exported: " + exported)

		bluemonkey.assertIsTrue(exported, "Failed to export custom dbus object: " + dbus.errorMessage(dbus.Session));

		var exported2 = dbus.exportObject("/two", "org.awesome.interface2", dbus.Session, testExport)
		bluemonkey.log("exported2: " + exported2)

		bluemonkey.assertIsTrue(exported2, "failed to export testExport: " + dbus.errorMessage());*/
	}
	catch(error)
	{
		bluemonkey.log("nasty dbus errors");
	}
}

if(ble !== undefined)
{
	serviceUuid = "5faaf494-d4c6-483e-b592-d1a6ffd436c9";
	device = ble.addService(serviceUuid, "5faaf495-d4c6-483e-b592-d1a6ffd436c9", "5faaf496-d4c6-483e-b592-d1a6ffd436c9");
	ble.scanningChanged.connect(function ()
	{
		if(!ble.scan)
		{
			bluemonkey.log("scan finished");
		}
	});

	ble.scan = true;
	ble.devicesChanged.connect(function ()
	{
		bluemonkey.log("devices that match the service uuid: " + ble.devices(serviceUuid))
	});
}
