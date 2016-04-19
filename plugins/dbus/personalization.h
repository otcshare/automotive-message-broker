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
		wantPropertyVariant(VehicleProperty::SteeringWheelPositionSlide, "Slide", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::SteeringWheelPositionTilt, "Tilt", VariantType::ReadWrite);
	}
};

class MirrorProperty: public DBusSink
{
public:
	MirrorProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Mirror", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::MirrorSettingPan, "MirrorPan", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::MirrorSettingTilt, "MirrorTilt", VariantType::ReadWrite);
	}
};

class SeatAdjustment: public DBusSink
{
public:
	SeatAdjustment(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("SeatAdjustment", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::SeatPositionBackCushion, "SeatBackCushion", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionRecline, "SeatReclineBack", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionSlide, "SeatSlide", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionCushionHeight, "SeatCushionHeight", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionHeadrest, "SeatHeadrest", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionSideCushion, "SeatSideCushion", VariantType::ReadWrite);
	}
};

class DriveMode: public DBusSink
{
public:
	DriveMode(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("DriveMode", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::VehicleDriveMode, "DriveMode", VariantType::ReadWrite);
	}
};

class VehicleSound: public DBusSink
{
public:
	VehicleSound(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("VehicleSound", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::ActiveNoiseControlMode, "ActiveNoiseControlMode", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::EngineSoundEnhancementMode, "EngineSoundEnhancementMode", VariantType::ReadWrite);
		wantPropertyVariant(VehicleProperty::AvailableSounds, "AvailableSounds", VariantType::ReadWrite);
	}
};



#endif
