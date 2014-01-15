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
		:DBusSink("VehicleId", re, connection, map<string, string>())
	{
		/** @attributeName WMI
		 *  @type DOMString
		 *  @access readonly
		 *  @attributeComment \brief MUST return World Manufacturer Identifier (WMI)
		 *  @attributeComment WMI is defined by SAE ISO 3780:2009.  3 characters.
		 **/
		wantPropertyVariant(VehicleProperty::WMI, "WMI", "s", AbstractProperty::Read);

		/** @attributeName VIN
		 *  @type DOMString
		 *  @access readonly
		 *  @attributeComment \brief MUST return Vehicle Identification Number (VIN) as defined by ISO 3779. 17 characters.
		 **/
		wantPropertyVariant(VehicleProperty::VIN, "VIN", "s", AbstractProperty::Read);

		
	}
};

/** @interface Size : VehiclePropertyType **/
class SizeProperty: public DBusSink
{
public:
	SizeProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Size", re, connection, map<string, string>())
	{
		/** @attributeName Width
		 *  @type unsigned long
		 *  @access readonly
		 *  @attributeComment \brief MUST return width of vehicle in mm
		 **/
		wantPropertyVariant(VehicleProperty::VehicleWidth, "Width", "u", AbstractProperty::Read);

		/** @attributeName Height
		 *  @type unsigned long
		 *  @access readonly
		 *  @attributeComment \brief MUST return height of vehicle in mm
		 **/
		wantPropertyVariant(VehicleProperty::VehicleHeight, "Height", "u", AbstractProperty::Read);

		/** @attributeName Length
		 *  @type unsigned long
		 *  @access readonly
		 *  @attributeComment \brief MUST return length of vehicle in mm
		 **/
		wantPropertyVariant(VehicleProperty::VehicleLength, "Length", "u", AbstractProperty::Read);

		
	}
};

/** @interface FuelInfo : VehiclePropertyType **/
class FuelInfoProperty: public DBusSink
{
public:
	FuelInfoProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("FuelInfo", re, connection, map<string, string>())
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
		wantPropertyVariant(VehicleProperty::FuelType, "Type", "y", AbstractProperty::Read);

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
		wantPropertyVariant(VehicleProperty::FuelPositionSide, "RefuelPosition", "y", AbstractProperty::Read);

		
	}
};

/** @interface VehicleType : VehiclePropertyType **/
class VehicleTypeProperty: public DBusSink
{
public:
	VehicleTypeProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("VehicleType", re, connection, map<string, string>())
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
		wantPropertyVariant(VehicleProperty::VehicleType, "Type", "y", AbstractProperty::Read);

		
	}
};

/** @interface Doors : VehiclePropertyType **/
class DoorsProperty: public DBusSink
{
public:
	DoorsProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Doors", re, connection, map<string, string>())
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

		
	}
};

/** @interface WheelInformation : VehiclePropertyType **/
class WheelInformationProperty: public DBusSink
{
public:
	WheelInformationProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("WheelInformation", re, connection, map<string, string>())
	{

		/** @attributeName FrontWheelRadius
		 *  @type unsigned short
		 *  @access readonly
		 *  @attributeComment \brief MUST return Radius of Front Wheel(s) in mm.
		 **/
		wantPropertyVariant(VehicleProperty::FrontWheelRadius, "FrontWheelRadius", "q", AbstractProperty::Read);

		/** @attributeName RearWheelRadius
		 *  @type unsigned short
		 *  @access readonly
		 *  @attributeComment \brief MUST return Radius of Rear Wheel(s) in mm.
		 **/
		wantPropertyVariant(VehicleProperty::RearWheelRadius, "RearWheelRadius", "q", AbstractProperty::Read);

		/** @attributeName WheelTrack
		 *  @type unsigned long
		 *  @access readonly
		 *  @attributeComment \brief MUST return Wheel Track in mm.
		 **/
		wantPropertyVariant(VehicleProperty::WheelTrack, "WheelTrack", "u", AbstractProperty::Read);

		/** @attributeName ABS
		 *  @type boolean
		 *  @access readonly
		 *  @attributeComment \brief MUST return Antilock Brake System status: on = true, off = false.
		 **/
		wantPropertyVariant(VehicleProperty::AntilockBrakingSystem, "AntilockBrakingSystem", "b", AbstractProperty::Read);

		
	}
};

#endif
