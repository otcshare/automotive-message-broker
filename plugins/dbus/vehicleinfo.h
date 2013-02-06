#ifndef _VEHICLEINFO_H_
#define _VEHICLEINFO_H_


#include "dbusplugin.h"
#include "abstractdbusinterface.h"
#include "abstractroutingengine.h"


class VehicleId: public DBusSink
{
public:
	VehicleId(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.vehicleId","/org/automotive/vehicleinfo/vehicleId", re, connection, map<string, string>())
	{
		wantPropertyString(VehicleProperty::WMI, "WMI", "s", AbstractProperty::Read);
		wantPropertyString(VehicleProperty::VIN, "VIN", "s", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

class SizeProperty: public DBusSink
{
public:
	SizeProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.size","/org/automotive/vehicleinfo/size", re, connection, map<string, string>())
	{
		wantProperty<uint>(VehicleProperty::VehicleWidth, "Width", "i", AbstractProperty::Read);
		wantProperty<uint>(VehicleProperty::VehicleHeight, "Height", "i", AbstractProperty::Read);
		wantProperty<uint>(VehicleProperty::VehicleLength, "Length", "i", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

class FuelInfoProperty: public DBusSink
{
public:
	FuelInfoProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
			:DBusSink("org.automotive.fuelInfo","/org/automotive/vehicleinfo/fuelInfo", re, connection, map<string, string>())
		{
			wantProperty<Fuel::Type>(VehicleProperty::FuelType, "Type", "i", AbstractProperty::Read);
			wantProperty<Fuel::RefuelPosition>(VehicleProperty::FuelPositionSide, "RefuelPosition", "i", AbstractProperty::Read);

			supportedChanged(re->supported());
		}
};


class VehicleTypeProperty: public DBusSink
{
public:
	VehicleTypeProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.vehicleType","/org/automotive/vehicleinfo/vehicleType", re, connection, map<string, string>())
	{
		wantProperty<Vehicle::Type>(VehicleProperty::VehicleType, "Type", "i", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

class DoorsProperty: public DBusSink
{
public:
	DoorsProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.doors","/org/automotive/vehicleinfo/doors", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::DoorsPerRow, "DoorsPerRow", "a(i)", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

class TransmissionInfoProperty: public DBusSink
{
public:
	TransmissionInfoProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.transmissionGearType","/org/automotive/vehicleinfo/transmissionGearType", re, connection, map<string, string>())
	{
		wantProperty<Transmission::Type>(VehicleProperty::TransmissionGearType, "TransmissionGearType", "i", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

class WheelInformationProperty: public DBusSink
{
public:
	WheelInformationProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.WheelInformationProperty","/org/automotive/vehicleinfo/WheelInformationProperty", re, connection, map<string, string>())
	{
		wantProperty<uint16_t>(VehicleProperty::FrontWheelRadius, "FrontWheelRadius", "i", AbstractProperty::Read);
		wantProperty<uint16_t>(VehicleProperty::RearWheelRadius, "RearWheelRadius", "i", AbstractProperty::Read);
		wantProperty<uint>(VehicleProperty::WheelTrack, "WheelTrack", "i", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

#endif
