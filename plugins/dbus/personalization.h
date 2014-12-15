#ifndef PERSONALIZATION_H_
#define PERSONALIZATION_H_

#include "dbusplugin.h"
#include "abstractdbusinterface.h"
#include "abstractroutingengine.h"

///TODO Deprecated.  Remove in 0.14
class SeatPostionProperty: public DBusSink
{
public:
	SeatPostionProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("SeatPosition", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::SeatPositionBackCushion, "BackCushion", "i", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionRecline, "Recline", "i", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionSlide, "Slide", "i", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionCushionHeight, "CushionHeight", "i", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionHeadrest, "Headrest", "i", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionSideCushion, "SideCushion", "i", AbstractProperty::ReadWrite);
	}
};

class SteeringWheelPositionProperty: public DBusSink
{
public:
	SteeringWheelPositionProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("SteeringWheelPosition", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::SteeringWheelPositionSlide, "Slide", "i", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SteeringWheelPositionTilt, "Tilt", "i", AbstractProperty::ReadWrite);
	}
};

///TODO Deprecated. Remove in 0.14
class MirrorSettingProperty: public DBusSink
{
public:
	MirrorSettingProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("MirrorSetting", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::MirrorSettingPan, "Pan", "i", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::MirrorSettingTilt, "Tilt", "i", AbstractProperty::ReadWrite);
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
		wantPropertyVariant(VehicleProperty::SeatPositionBackCushion,"SeatBackCushion", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionRecline,"SeatReclineBack", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionSlide,"SeatSlide", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionCushionHeight,"SeatCushionHeight", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionHeadrest,"SeatHeadrest", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionSideCushion,"SeatSideCushion", AbstractProperty::ReadWrite);
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
