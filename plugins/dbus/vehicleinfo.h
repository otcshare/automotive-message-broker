#ifndef _VEHICLEINFO_H_
#define _VEHICLEINFO_H_


#include "dbusplugin.h"
#include "abstractdbusinterface.h"
#include "abstractroutingengine.h"

/** @interface VehicleId **/
class VehicleId: public DBusSink
{
public:
	VehicleId(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.VehicleId","/org/automotive/vehicleinfo/VehicleId", re, connection, map<string, string>())
	{
		/** @attributeName WMI
		 *  @type DOMString
		 *  @access readonly
		 **/
		wantPropertyString(VehicleProperty::WMI, "WMI", "s", AbstractProperty::Read);

		/** @attributeName VIN
		 *  @type DOMString
		 *  @access readonly
		 **/
		wantPropertyString(VehicleProperty::VIN, "VIN", "s", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

/** @interface Size **/
class SizeProperty: public DBusSink
{
public:
	SizeProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.Size","/org/automotive/vehicleinfo/Size", re, connection, map<string, string>())
	{
		/** @attributeName Width
		 *  @type unsigned long
		 *  @access readonly
		 **/
		wantProperty<uint>(VehicleProperty::VehicleWidth, "Width", "i", AbstractProperty::Read);

		/** @attributeName Height
		 *  @type unsigned long
		 *  @access readonly
		 **/
		wantProperty<uint>(VehicleProperty::VehicleHeight, "Height", "i", AbstractProperty::Read);

		/** @attributeName Length
		 *  @type unsigned long
		 *  @access readonly
		 **/
		wantProperty<uint>(VehicleProperty::VehicleLength, "Length", "i", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

/** @interface FuelInfo **/
class FuelInfoProperty: public DBusSink
{
public:
	FuelInfoProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.FuelInfo","/org/automotive/vehicleinfo/FuelInfo", re, connection, map<string, string>())
	{
		/** @attributeName Type
		 *  @type unsigned short
		 *  @access readonly
		 **/
		wantProperty<Fuel::Type>(VehicleProperty::FuelType, "Type", "i", AbstractProperty::Read);

		/** @attributeName RefuelPosition
		 *  @type unsigned short
		 *  @access readonly
		 **/
		wantProperty<Fuel::RefuelPosition>(VehicleProperty::FuelPositionSide, "RefuelPosition", "i", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

/** @interface VehicleType **/
class VehicleTypeProperty: public DBusSink
{
public:
	VehicleTypeProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.VehicleType","/org/automotive/vehicleinfo/VehicleType", re, connection, map<string, string>())
	{
		/** @attributeName Type
		 *  @type unsigned short
		 *  @access readonly
		 **/
		wantProperty<Vehicle::Type>(VehicleProperty::VehicleType, "Type", "i", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

/** @interface Doors **/
class DoorsProperty: public DBusSink
{
public:
	DoorsProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.Doors","/org/automotive/vehicleinfo/Doors", re, connection, map<string, string>())
	{
		/** @attributeName DoorsPerRow
		 *  @type sequence<unsigned short>
		 *  @access readonly
		 **/
		wantPropertyVariant(VehicleProperty::DoorsPerRow, "DoorsPerRow", "ai", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

/** @interface TransmissionGearType **/
class TransmissionInfoProperty: public DBusSink
{
public:
	TransmissionInfoProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.TransmissionGearType","/org/automotive/vehicleinfo/TransmissionGearType", re, connection, map<string, string>())
	{
		/** @attributeName TransmissionGearType
		 *  @type unsigned short
		 *  @access readonly
		 **/
		wantProperty<Transmission::Type>(VehicleProperty::TransmissionGearType, "TransmissionGearType", "i", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

/** @interface WheelInformation **/
class WheelInformationProperty: public DBusSink
{
public:
	WheelInformationProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.WheelInformation","/org/automotive/vehicleinfo/WheelInformation", re, connection, map<string, string>())
	{

		/** @attributeName FrontWheelRadius
		 *  @type unsigned short
		 *  @access readonly
		 **/
		wantProperty<uint16_t>(VehicleProperty::FrontWheelRadius, "FrontWheelRadius", "i", AbstractProperty::Read);

		/** @attributeName RearWheelRadius
		 *  @type unsigned short
		 *  @access readonly
		 **/
		wantProperty<uint16_t>(VehicleProperty::RearWheelRadius, "RearWheelRadius", "i", AbstractProperty::Read);

		/** @attributeName WheelTrack
		 *  @type unsigned long
		 *  @access readonly
		 **/
		wantProperty<uint>(VehicleProperty::WheelTrack, "WheelTrack", "i", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

#endif
