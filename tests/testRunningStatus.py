#!/usr/bin/python

import dbus
import sys

bus = dbus.SessionBus()
runningStatusObject = bus.get_object("org.automotive.message.broker", "/runningStatus");
runningStatusInterface = dbus.Interface(runningStatusObject, "org.freedesktop.DBus.Properties")


speed = runningStatusInterface.Get("org.automotive.message.broker.RunningStatus", "VehicleSpeed");
enginespeed = runningStatusInterface.Get("org.automotive.message.broker.RunningStatus", "EngineSpeed");
vehiclePowerMode = runningStatusInterface.Get("org.automotive.message.broker.RunningStatus", "VehiclePowerMode");
fuelEconomy = runningStatusInterface.Get("org.automotive.message.broker.RunningStatus", "FuelEconomy");
tripMeter = runningStatusInterface.Get("org.automotive.message.broker.RunningStatus", "TripMeter");
transmissionGearStatus = runningStatusInterface.Get("org.automotive.message.broker.RunningStatus", "TransmissionGearStatus");
cruiseControlStatus = runningStatusInterface.Get("org.automotive.message.broker.RunningStatus", "CruiseControlStatus");
wheelBrake = runningStatusInterface.Get("org.automotive.message.broker.RunningStatus", "WheelBrake");
lightStatus = runningStatusInterface.Get("org.automotive.message.broker.RunningStatus", "LightStatus");
interiorLightStatus = runningStatusInterface.Get("org.automotive.message.broker.RunningStatus", "InteriorLightStatus");

print "VehicleSpeed: ", speed
print "EngineSpeed: ", enginespeed
print "VehiclePowerMode: %i" % (vehiclePowerMode)
print "FuelEconomy: ", fuelEconomy
print "TripmMeter: ", tripMeter
print "TransmissionGearStatus: %i" % (transmissionGearStatus)
print "CruiseControlStatus: ", cruiseControlStatus
print "WheelBrake: ", wheelBrake
print "LightStatus: ", lightStatus
print "InteriorLightStatus", interiorLightStatus

assert type(speed) == dbus.UInt16
assert speed == 5

assert type(enginespeed) == dbus.UInt16
assert enginespeed == 2000

assert type(vehiclePowerMode) == dbus.Byte
assert vehiclePowerMode == 3

assert type(fuelEconomy) == dbus.UInt16
assert fuelEconomy == 42

assert type(tripMeter) == dbus.Array
assert len(tripMeter) == 2
assert tripMeter[0] == 1500
assert tripMeter[1] == 1000

assert type(transmissionGearStatus) == dbus.Byte
assert transmissionGearStatus == 1

assert type(cruiseControlStatus) == dbus.Struct
assert cruiseControlStatus[0] == True
assert cruiseControlStatus[1] == 5

assert type(wheelBrake) == dbus.Boolean
assert wheelBrake == False

assert type(lightStatus) == dbus.Dictionary
assert lightStatus[0] == True
assert lightStatus[1] == False
assert lightStatus[2] == False
assert lightStatus[3] == False
assert lightStatus[4] == True
assert lightStatus[5] == False
assert lightStatus[6] == False
assert lightStatus[7] == False

assert type(interiorLightStatus) == dbus.Dictionary
assert interiorLightStatus[0] == False
assert interiorLightStatus[1] == True
assert interiorLightStatus[2] == False

print "Passed"
