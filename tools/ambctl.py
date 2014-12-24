#!/usr/bin/python

import argparse
import dbus
import sys
import json
import gobject
from dbus.mainloop.glib import DBusGMainLoop

def changed(interface, properties, invalidated):
	print json.dumps(properties, indent=2)

parser = argparse.ArgumentParser(description='Process DBus mappings.')
parser.add_argument('command', metavar='COMMAND [help]', help='amb dbus command')

parser.add_argument('commandArgs', metavar='ARG', nargs='*',
			help='amb dbus command arguments')

args = parser.parse_args()

dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
bus = dbus.SystemBus()
managerObject = bus.get_object("org.automotive.message.broker", "/");
managerInterface = dbus.Interface(managerObject, "org.automotive.Manager")

if args.command == "list" :
	supportedList = managerInterface.List();
	for objectName in supportedList:
		print objectName
	sys.exit()

elif args.command == "get":
	if args.commandArgs[0] == "help":
		print "ObjectName [ObjectName...]"
		sys.exit()

	for objectName in args.commandArgs:
		objects = managerInterface.FindObject(objectName);
		print objectName
		for o in objects:
			propertiesInterface = dbus.Interface(bus.get_object("org.automotive.message.broker", o),"org.freedesktop.DBus.Properties")
			print json.dumps(propertiesInterface.GetAll("org.automotive."+objectName), indent=2)
	sys.exit()

elif args.command == "listen":
	if args.commandArgs[0] == "help":
		print "ObjectName [ObjectName...]"
		sys.exit()
	for objectName in args.commandArgs:
		objects = managerInterface.FindObject(objectName);
		for o in objects:
			bus.add_signal_receiver(changed,
					dbus_interface="org.freedesktop.DBus.Properties",
					signal_name="PropertiesChanged", path=o)
elif args.command == "set":
	if args.commandArgs[0] == "help":
		print "ObjectName PropertyName VALUE [ZONE]"
		sys.exit()
	if len(args.commandArgs) < 3:
		print "set requires more arguments (see set help)"
		sys.exit()
	objectName = args.commandArgs[0]
	propertyName = args.commandArgs[1]
	value = args.commandArgs[2]
	zone = 0
	if len(args.commandArgs) == 4:
		zone = int(args.commandArgs[3])
	object = managerInterface.FindObjectForZone(objectName, zone)
	propertiesInterface = dbus.Interface(bus.get_object("org.automotive.message.broker", object),"org.freedesktop.DBus.Properties")
	property = propertiesInterface.Get("org.automotive."+objectName, propertyName)
	realValue = property.__class__(value)
	propertiesInterface.Set("org.automotive."+objectName, propertyName, realValue)
	property = propertiesInterface.Get("org.automotive."+objectName, propertyName)
	if property == realValue:
		print propertyName + " = ", property
	else:
		print "Error setting property"
	sys.exit()
elif args.command == "getHistory":
	if args.commandArgs[0] == "help":
		print "ObjectName [STARTTIME] [ENDTIME] [ZONE]"
		sys.exit()
	if len(args.commandArgs) < 1:
		print "getHistory requires more arguments (see getHistory help)"
		sys.exit()
	objectName = args.commandArgs[0]
	start = 1
	if len(args.commandArgs) >= 2:
		start = float(args.commandArgs[1])
	end = 9999999999
	if len(args.commandArgs) >= 3:
		end = float(args.commandArgs[2])
	zone = 0
	if len(args.commandArgs) == 4:
		zone = int(args.commandArgs[3])

	object = managerInterface.FindObjectForZone(objectName, zone);
	propertiesInterface = dbus.Interface(bus.get_object("org.automotive.message.broker", object),"org.automotive."+objectName)
	print json.dumps(propertiesInterface.GetHistory(start, end), indent=2)
	sys.exit()
else:
	print "unknown command"
	sys.exit()
loop = gobject.MainLoop()
loop.run()
