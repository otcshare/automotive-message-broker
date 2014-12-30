#ifndef PERSONALIZATION_H_
#define PERSONALIZATION_H_

#include "dbusplugin.h"
#include "abstractdbusinterface.h"
#include "abstractroutingengine.h"

class SteeringWheelPositionProperty: public DBusSink
{
public:
	SteeringWheelPositionProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("SteeringWheelPosition", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::SteeringWheelPositionSlide, "Slide", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SteeringWheelPositionTilt, "Tilt", AbstractProperty::ReadWrite);
	}
};

class MirrorProperty: public DBusSink
{
public:
	MirrorProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Mirror", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::MirrorSettingPan, "MirrorPan", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::MirrorSettingTilt, "MirrorTilt", AbstractProperty::ReadWrite);
	}
};

class SeatAdjustment: public DBusSink
{
public:
	SeatAdjustment(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("SeatAdjustment", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::SeatPositionBackCushion, "SeatBackCushion", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionRecline, "SeatReclineBack", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionSlide, "SeatSlide", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionCushionHeight, "SeatCushionHeight", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionHeadrest, "SeatHeadrest", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionSideCushion, "SeatSideCushion", AbstractProperty::ReadWrite);
	}
};

class DriveMode: public DBusSink
{
public:
	DriveMode(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("DriveMode", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::VehicleDriveMode, "DriveMode", AbstractProperty::ReadWrite);
	}
};

class VehicleSound: public DBusSink
{
public:
	VehicleSound(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("VehicleSound", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::ActiveNoiseControlMode, "ActiveNoiseControlMode", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::EngineSoundEnhancementMode, "EngineSoundEnhancementMode", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::AvailableSounds, "AvailableSounds", AbstractProperty::ReadWrite);
	}
};



#endif
