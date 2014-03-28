#!/usr/bin/python

import dbus
import sys
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("property", help="{propertyToSet}")
parser.add_argument("value", help="{value}")
parser.add_argument("zone", help="{zone}")

args = parser.parse_args()

# "{'type' : 'method', 'name' : 'set', 'transactionid' : 'meh', 
# 'data' : [{'interface' : 'vcan0', 'property' : 'VehicleSpeed', 'value' : '33', 'zone' : '0' }]}"

realValue = "{'type' : 'method', 'name' : 'set', 'transactionid' : 'meh', 'data' : [{'interface' : 'vcan0', 'property' : '" + args.property + "', 'value' : '" + args.value + "', 'zone' : " + args.zone + " }]}"

bus = dbus.SystemBus()
managerObject = bus.get_object("org.automotive.message.broker", "/");
managerInterface = dbus.Interface(managerObject, "org.automotive.Manager")

object = managerInterface.FindObjectForZone("SimCommand", 0)

propertiesInterface = dbus.Interface(bus.get_object("org.automotive.message.broker", object),"org.freedesktop.DBus.Properties")
propertiesInterface.Set("org.automotive.SimCommand", "SimCommand", realValue)
print "Success!"
