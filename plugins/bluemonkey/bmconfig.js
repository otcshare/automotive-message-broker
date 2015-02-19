console.log("hello world!");

bluemonkey.loadModule("@PLUGIN_INSTALL_PATH@/bluemonkeyDbModule.so");
bluemonkey.loadModule("@PLUGIN_INSTALL_PATH@/bluemonkeyDBusModule.so");
bluemonkey.loadModule("@PLUGIN_INSTALL_PATH@/bluemonkeyBleModule.so");
bluemonkey.loadModule("@PLUGIN_INSTALL_PATH@/bluemonkeyWsModule.so");

exportObj = {};
exportObj.foo = function() { return "bar"; }
exportObj.bar = true;

if(dbus !== undefined)
{
    try
    {
        var dbusIface = dbus.createInterface("org.freedesktop.DBus", "/", "org.freedesktop.DBus", dbus.Session);

        var reply = dbusIface.GetId();

        console.log("org.freedesktop.DBus.GetId() response: " + reply);

        var registered = dbus.registerService("org.bluemonkey", dbus.Session)

        bluemonkey.assertIsTrue(registered, "could not register service: " + dbus.errorMessage(dbus.Session));

        /* TODO: Uncomment this when dbus export works
        var exported = dbus.exportObject("/one", "org.awesome.interface", dbus.Session, exportObj);
        console.log("exported: " + exported)

        bluemonkey.assertIsTrue(exported, "Failed to export custom dbus object: " + dbus.errorMessage(dbus.Session));

        var exported2 = dbus.exportObject("/two", "org.awesome.interface2", dbus.Session, testExport)
        console.log("exported2: " + exported2)

        bluemonkey.assertIsTrue(exported2, "failed to export testExport: " + dbus.errorMessage());*/
    }
    catch(error)
    {
        console.log("nasty dbus errors");
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
            console.log("scan finished");
        }
    });

    ble.scan = true;
    ble.devicesChanged.connect(function ()
    {
        console.log("devices that match the service uuid: " + ble.devices(serviceUuid))
    });
}

var shndl

if(websockets !== undefined)
{
    console.log("can has websockets!");

    socket = new WebSocket("ws://echo.websocket.org");

    socket.onmessage = function(msg)
    {
        console.log("msg: "+ msg);
    };

    socket.onopen = function()
    {
        console.log("opened!");
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

    shndl = socket;
}
else
{
    console.log("no websocket support :(");
}
