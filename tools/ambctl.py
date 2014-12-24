#!/usr/bin/python

import argparse
import dbus
import sys
import json
import gobject
import fileinput
from dbus.mainloop.glib import DBusGMainLoop

def changed(interface, properties, invalidated):
	print json.dumps(properties, indent=2)

def processCommand(command, commandArgs):
	dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
	bus = dbus.SystemBus()
	managerObject = bus.get_object("org.automotive.message.broker", "/");
	managerInterface = dbus.Interface(managerObject, "org.automotive.Manager")

	if command == "list" :
		supportedList = managerInterface.List();
		for objectName in supportedList:
			print objectName
		return 1
	elif command == "get":
		if commandArgs[0] == "help":
			print "ObjectName [ObjectName...]"
			return 1

		for objectName in commandArgs:
			objects = managerInterface.FindObject(objectName);
			print objectName
			for o in objects:
				propertiesInterface = dbus.Interface(bus.get_object("org.automotive.message.broker", o),"org.freedesktop.DBus.Properties")
				print json.dumps(propertiesInterface.GetAll("org.automotive."+objectName), indent=2)
		return 1
	elif command == "listen":
		if commandArgs[0] == "help":
			print "ObjectName [ObjectName...]"
			return 1
		for objectName in commandArgs:
			objects = managerInterface.FindObject(objectName);
			for o in objects:
				bus.add_signal_receiver(changed,
					dbus_interface="org.freedesktop.DBus.Properties",
					signal_name="PropertiesChanged", path=o)
		loop = gobject.MainLoop()
		loop.run()

	elif command == "set":
		if commandArgs[0] == "help":
			print "ObjectName PropertyName VALUE [ZONE]"
			return 1
		if len(commandArgs) < 3:
			print "set requires more arguments (see set help)"
			return 1
		objectName = commandArgs[0]
		propertyName = commandArgs[1]
		value = commandArgs[2]
		zone = 0
		if len(commandArgs) == 4:
			zone = int(commandArgs[3])
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
		return 1
	elif command == "getHistory":
		if commandArgs[0] == "help":
			print "ObjectName [STARTTIME] [ENDTIME] [ZONE]"
			return 1
		if len(commandArgs) < 1:
			print "getHistory requires more arguments (see getHistory help)"
			return 1
		objectName = commandArgs[0]
		start = 1
		if len(commandArgs) >= 2:
			start = float(commandArgs[1])
		end = 9999999999
		if len(commandArgs) >= 3:
			end = float(commandArgs[2])
		zone = 0
		if len(commandArgs) == 4:
			zone = int(commandArgs[3])
		object = managerInterface.FindObjectForZone(objectName, zone);
		propertiesInterface = dbus.Interface(bus.get_object("org.automotive.message.broker", object),"org.automotive."+objectName)
		print json.dumps(propertiesInterface.GetHistory(start, end), indent=2)
	else:
		print "unknown command"
	return 1



parser = argparse.ArgumentParser(description='Process DBus mappings.')
parser.add_argument('command', metavar='COMMAND [help]', nargs='?', default='stdin', help='amb dbus command')

parser.add_argument('commandArgs', metavar='ARG', nargs='*',
			help='amb dbus command arguments')

args = parser.parse_args()

if args.command == "stdin":
	while True:
		line = raw_input("ambctl>> ")
		if line == 'quit':
			sys.exit()
		words = line.split(' ')
		print words[1:]
		processCommand(words[0], words[1:])
else:
	processCommand(args.command, args.commandArgs)

