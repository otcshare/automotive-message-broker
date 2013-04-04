#ifndef _VEHICLEINFO_H_
#define _VEHICLEINFO_H_


#include "dbusplugin.h"
#include "abstractdbusinterface.h"
#include "abstractroutingengine.h"

/** @interface VehicleId : VehiclePropertyType **/
class VehicleId: public DBusSink
{
public:
	VehicleId(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.VehicleId","/org/automotive/vehicleinfo/VehicleId", re, connection, map<string, string>())
	{
		/** @attributeName WMI
		 *  @type DOMString
		 *  @access readonly
		 *  @attributeComment \brief MUST return World Manufacturer Identifier (WMI)
		 *  @attributeComment WMI is defined by SAE ISO 3780:2009.  3 characters.
		 **/
		wantPropertyString(VehicleProperty::WMI, "WMI", "s", AbstractProperty::Read);

		/** @attributeName VIN
		 *  @type DOMString
		 *  @access readonly
		 *  @attributeComment \brief MUST return Vehicle Identification Number (VIN) as defined by ISO 3779. 17 characters.
		 **/
		wantPropertyString(VehicleProperty::VIN, "VIN", "s", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

/** @interface Size : VehiclePropertyType **/
class SizeProperty: public DBusSink
{
public:
	SizeProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.Size","/org/automotive/vehicleinfo/Size", re, connection, map<string, string>())
	{
		/** @attributeName Width
		 *  @type unsigned long
		 *  @access readonly
		 *  @attributeComment \brief MUST return width of vehicle in mm
		 **/
		wantProperty<uint>(VehicleProperty::VehicleWidth, "Width", "u", AbstractProperty::Read);

		/** @attributeName Height
		 *  @type unsigned long
		 *  @access readonly
		 *  @attributeComment \brief MUST return height of vehicle in mm
		 **/
		wantProperty<uint>(VehicleProperty::VehicleHeight, "Height", "u", AbstractProperty::Read);

		/** @attributeName Length
		 *  @type unsigned long
		 *  @access readonly
		 *  @attributeComment \brief MUST return length of vehicle in mm
		 **/
		wantProperty<uint>(VehicleProperty::VehicleLength, "Length", "u", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

/** @interface FuelInfo : VehiclePropertyType **/
class FuelInfoProperty: public DBusSink
{
public:
	FuelInfoProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.FuelInfo","/org/automotive/vehicleinfo/FuelInfo", re, connection, map<string, string>())
	{
		/**
		 * @enum const unsigned short FUELTYPE_GASOLINE = 0;
		 * @enum const unsigned short FUELTYPE_HIGH_OCTANE= 1;
		 * @enum const unsigned short FUELTYPE_DIESEL = 2;
		 * @enum const unsigned short FUELTYPE_ELECTRIC = 3;
		 * @enum const unsigned short FUELTYPE_HYDROGEN = 4;
		 **/

		/** @attributeName Type
		 *  @type unsigned short
		 *  @access readonly
		 *  @attributeComment \brief MUST return type of fuel.  integer 0-4 (see FUELTYPE_*)
		 **/
		wantProperty<Fuel::Type>(VehicleProperty::FuelType, "Type", "y", AbstractProperty::Read);

		/**
		 * @enum const unsigned short REFUELPOSITION_LEFT = 0;
		 * @enum const unsigned short REFUELPOSITION_RIGHT= 1;
		 * @enum const unsigned short REFUELPOSITION_FRONT = 2;
		 * @enum const unsigned short REFUELPOSITION_REAR = 3;
		 **/

		/** @attributeName RefuelPosition
		 *  @type unsigned short
		 *  @access readonly
		 *  @attributeComment \brief MUST return position of refuling (see REFUELPOSITION_*)
		 **/
		wantProperty<Fuel::RefuelPosition>(VehicleProperty::FuelPositionSide, "RefuelPosition", "y", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

/** @interface VehicleType : VehiclePropertyType **/
class VehicleTypeProperty: public DBusSink
{
public:
	VehicleTypeProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.VehicleType","/org/automotive/vehicleinfo/VehicleType", re, connection, map<string, string>())
	{
		/**
		 * @enum const unsigned short VEHICLETYPE_SEDAN = 0;
		 * @enum const unsigned short VEHICLETYPE_COUPE= 1;
		 * @enum const unsigned short VEHICLETYPE_CABRIOLE = 2;
		 * @enum const unsigned short VEHICLETYPE_ROADSTER = 3;
		 * @enum const unsigned short VEHICLETYPE_SUV = 4;
		 * @enum const unsigned short VEHICLETYPE_TRUCK = 5;
		 **/

		/** @attributeName Type
		 *  @type unsigned short
		 *  @access readonly
		 *  @attributeComment \brief MUST return type of Vehicle.  Integer 0-5 (see VEHICLETYPE_*)
		 **/
		wantProperty<Vehicle::Type>(VehicleProperty::VehicleType, "Type", "y", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

/** @interface Doors : VehiclePropertyType **/
class DoorsProperty: public DBusSink
{
public:
	DoorsProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.Doors","/org/automotive/vehicleinfo/Doors", re, connection, map<string, string>())
	{
		/** @attributeName DoorsPerRow
		 *  @type sequence<unsigned short>
		 *  @access readonly
		 *  @attributeComment \brief MUST return Number of doors in each row.  The index represents the row.  Position '0'
		 *  @attributeComment represents the first row, '1' the second row etc.
		 *  @attributeComment Example a common mini-van may have Doors[0] = 2 doors,
		 *  @attributeComment Doors[1] = 1 (side door), Doors[2] = 1 (trunk).
		 **/
		wantPropertyVariant(VehicleProperty::DoorsPerRow, "DoorsPerRow", "ay", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

/** @interface TransmissionGearType : VehiclePropertyType **/
class TransmissionInfoProperty: public DBusSink
{
public:
	TransmissionInfoProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.TransmissionGearType","/org/automotive/vehicleinfo/TransmissionGearType", re, connection, map<string, string>())
	{
		/**
		 * @enum const unsigned short TRANSMISSIONGEARTYPE_AUTO=0;
		 * @enum const unsigned short TRANSMISSIONGEARTYPE_MANUAL=1;
		 * @enum const unsigned short TRANSMISSIONGEARTYPE_CV=2;
		 **/

		/** @attributeName TransmissionGearType
		 *  @type unsigned short
		 *  @access readonly
		 *  @attributeComment \brief MUST return transmission gear type of either Automatic, Manual or Constant Variable (CV).  See TRANSMISSIONGEARTYPE_*.
		 **/
		wantProperty<Transmission::Type>(VehicleProperty::TransmissionGearType, "TransmissionGearType", "y", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

/** @interface WheelInformation : VehiclePropertyType **/
class WheelInformationProperty: public DBusSink
{
public:
	WheelInformationProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.WheelInformation","/org/automotive/vehicleinfo/WheelInformation", re, connection, map<string, string>())
	{

		/** @attributeName FrontWheelRadius
		 *  @type unsigned short
		 *  @access readonly
		 *  @attributeComment \brief MUST return Radius of Front Wheel(s) in mm.
		 **/
		wantProperty<uint16_t>(VehicleProperty::FrontWheelRadius, "FrontWheelRadius", "q", AbstractProperty::Read);

		/** @attributeName RearWheelRadius
		 *  @type unsigned short
		 *  @access readonly
		 *  @attributeComment \brief MUST return Radius of Rear Wheel(s) in mm.
		 **/
		wantProperty<uint16_t>(VehicleProperty::RearWheelRadius, "RearWheelRadius", "q", AbstractProperty::Read);

		/** @attributeName WheelTrack
		 *  @type unsigned long
		 *  @access readonly
		 *  @attributeComment \brief MUST return Wheel Track in mm.
		 **/
		wantProperty<uint>(VehicleProperty::WheelTrack, "WheelTrack", "u", AbstractProperty::Read);

		/** @attributeName ABS
		 *  @type boolean
		 *  @access readonly
		 *  @attributeComment \brief MUST return Antilock Brake System status: on = true, off = false.
		 **/
		wantPropertyVariant(VehicleProperty::WheelTrack, "ABS", "b", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

#endif
