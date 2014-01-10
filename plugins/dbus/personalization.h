#ifndef PERSONALIZATION_H_
#define PERSONALIZATION_H_

#include "dbusplugin.h"
#include "abstractdbusinterface.h"
#include "abstractroutingengine.h"

class SeatPostionProperty: public DBusSink
{
public:
	SeatPostionProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("SeatPosition", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::SeatPositionBackCushion,"BackCushion", "i", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionRecline,"Recline", "i", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionSlide,"Slide", "i", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionCushionHeight,"CushionHeight", "i", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionHeadrest,"Headrest", "i", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SeatPositionSideCushion,"SideCushion", "i", AbstractProperty::ReadWrite);
	}


};

class SteeringWheelPositionProperty: public DBusSink
{
public:
	SteeringWheelPositionProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("SteeringWheelPosition", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::SteeringWheelPositionSlide,"Slide", "i", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::SteeringWheelPositionTilt,"Tilt", "i", AbstractProperty::ReadWrite);
	}
};

class MirrorSettingProperty: public DBusSink
{
public:
	MirrorSettingProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("MirrorSetting", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::MirrorSettingPan,"Pan", "i", AbstractProperty::ReadWrite);
		wantPropertyVariant(VehicleProperty::MirrorSettingTilt,"Tilt", "i", AbstractProperty::ReadWrite);
	}
};

#endif
