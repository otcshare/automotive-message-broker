#ifndef _VEHICLEINFO_H_
#define _VEHICLEINFO_H_


#include "dbusplugin.h"
#include "abstractdbusinterface.h"
#include "abstractroutingengine.h"

/** @interface VehicleId : VehiclePropertyType **/
class VehicleId: public DBusSink
{
public:
	VehicleId(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("VehicleId", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::WMI, "WMI", VariantType::Read);
		wantPropertyVariant(VehicleProperty::VIN, "VIN", VariantType::Read);
	}
};

/** @interface Size : VehiclePropertyType **/
class SizeProperty: public DBusSink
{
public:
	SizeProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Size", re, connection, map<string, string>())
	{
		/** @attributeName Width
		 *  @type unsigned long
		 *  @access readonly
		 *  @attributeComment \brief MUST return width of vehicle in mm
		 **/
		wantPropertyVariant(VehicleProperty::VehicleWidth, "Width", VariantType::Read);

		/** @attributeName Height
		 *  @type unsigned long
		 *  @access readonly
		 *  @attributeComment \brief MUST return height of vehicle in mm
		 **/
		wantPropertyVariant(VehicleProperty::VehicleHeight, "Height", VariantType::Read);

		/** @attributeName Length
		 *  @type unsigned long
		 *  @access readonly
		 *  @attributeComment \brief MUST return length of vehicle in mm
		 **/
		wantPropertyVariant(VehicleProperty::VehicleLength, "Length", VariantType::Read);


	}
};

/** @interface FuelInfo : VehiclePropertyType **/
class FuelInfoProperty: public DBusSink
{
public:
	FuelInfoProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("FuelInfo", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::FuelType, "Type", VariantType::Read);
		wantPropertyVariant(VehicleProperty::FuelPositionSide, "RefuelPosition", VariantType::Read);
	}
};

/** @interface VehicleType : VehiclePropertyType **/
class VehicleTypeProperty: public DBusSink
{
public:
	VehicleTypeProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("VehicleType", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::VehicleType, "Type", VariantType::Read);
	}
};

/** @interface Doors : VehiclePropertyType **/
class DoorsProperty: public DBusSink
{
public:
	DoorsProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("Doors", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::DoorsPerRow, "DoorsPerRow", VariantType::Read);
	}
};

/** @interface WheelInformation : VehiclePropertyType **/
class WheelInformationProperty: public DBusSink
{
public:
	WheelInformationProperty(VehicleProperty::Property, AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("WheelInformation", re, connection, map<string, string>())
	{
		wantPropertyVariant(VehicleProperty::FrontWheelRadius, "FrontWheelRadius", VariantType::Read);
		wantPropertyVariant(VehicleProperty::RearWheelRadius, "RearWheelRadius", VariantType::Read);
		wantPropertyVariant(VehicleProperty::WheelTrack, "WheelTrack", VariantType::Read);
		wantPropertyVariant(VehicleProperty::AntilockBrakingSystem, "AntilockBrakingSystem", VariantType::Read);
	}
};

#endif
