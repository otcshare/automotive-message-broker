#!/usr/bin/python

import dbus
import sys
import argparse
import gobject
import json
from dbus.mainloop.glib import DBusGMainLoop

print "This application is deprecated.  It will disappear in the future.  Use ambctl"

def print_data(interface, properties, invalidated):
	print json.dumps(properties, indent=2)

parser = argparse.ArgumentParser()
parser.add_argument("get", help="get {objectName}")
args = parser.parse_args()

objectName = args.get

dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
bus = dbus.SystemBus()
managerObject = bus.get_object("org.automotive.message.broker", "/");
managerInterface = dbus.Interface(managerObject, "org.automotive.Manager")

objects = managerInterface.FindObject(objectName);

for o in objects:
	bus.add_signal_receiver(print_data,
				dbus_interface="org.freedesktop.DBus.Properties",
				signal_name="PropertiesChanged",
																path=o)

loop = gobject.MainLoop()
loop.run()
