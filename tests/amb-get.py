#!/usr/bin/python

import dbus
import sys
import argparse
import json

parser = argparse.ArgumentParser()
parser.add_argument("get", help="get {objectName}")
args = parser.parse_args()

objectName = args.get

bus = dbus.SystemBus()
managerObject = bus.get_object("org.automotive.message.broker", "/");
managerInterface = dbus.Interface(managerObject, "org.automotive.Manager")

objects = managerInterface.FindObject(objectName);

for o in objects:
	propertiesInterface = dbus.Interface(bus.get_object("org.automotive.message.broker", o),"org.freedesktop.DBus.Properties")
	print json.dumps(propertiesInterface.GetAll("org.automotive."+objectName), indent=2)
