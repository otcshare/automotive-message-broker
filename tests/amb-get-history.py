#!/usr/bin/python

import dbus
import sys
import argparse
import json

print "This application is deprecated.  It will disappear in the future.  Use ambctl"

parser = argparse.ArgumentParser()
parser.add_argument("get", help="{objectName}")
parser.add_argument("start", help="{startTime}")
parser.add_argument("end", help="{endTime}")
parser.add_argument("zone", help="{zone}")

args = parser.parse_args()

objectName = args.get
start = float(args.start)
end = float(args.end)
zone = int(args.zone)

bus = dbus.SystemBus()
managerObject = bus.get_object("org.automotive.message.broker", "/");
managerInterface = dbus.Interface(managerObject, "org.automotive.Manager")

object = managerInterface.FindObjectForZone(objectName, zone);

propertiesInterface = dbus.Interface(bus.get_object("org.automotive.message.broker", object),"org.automotive."+objectName)
print json.dumps(propertiesInterface.GetHistory(start,end),indent=2)
