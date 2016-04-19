#!/usr/bin/python

import dbus
import sys
import argparse

print "This application is deprecated.  It will disappear in the future.  Use ambctl"

parser = argparse.ArgumentParser()
parser.add_argument("objectName", help="{objectName}")
parser.add_argument("property", help="{propertyToSet}")
parser.add_argument("value", help="{value}")
parser.add_argument("valueType", help="{valueType}")
parser.add_argument("zone", help="{zone}")

args = parser.parse_args()

objectName = args.objectName
propertyToSet = args.property
value = args.value
valueType = args.valueType.lower()
zone = int(args.zone)

realValue = 0

if valueType == "boolean":
	realValue = value == "true"
elif valueType == "integer":
	realValue = dbus.Int32(value)
elif valueType == "string":
	realValue = value
elif valueType == "double":
	realValue = double(value)
elif valueType == "UInt16":
	realValue = dbus.UInt16(value)
else:
	raise Exception("Unknown type: " + valueType)

bus = dbus.SystemBus()
managerObject = bus.get_object("org.automotive.message.broker", "/");
managerInterface = dbus.Interface(managerObject, "org.automotive.Manager")

object = managerInterface.FindObjectForZone(objectName, zone)

propertiesInterface = dbus.Interface(bus.get_object("org.automotive.message.broker", object),"org.freedesktop.DBus.Properties")
propertiesInterface.Set("org.automotive."+objectName, propertyToSet, realValue)
print "Success!"
