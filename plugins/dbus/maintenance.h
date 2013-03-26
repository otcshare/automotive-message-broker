#ifndef _MAINTENANCE_H_
#define _MAINTENANCE_H_


#include "dbusplugin.h"
#include "abstractdbusinterface.h"
#include "abstractroutingengine.h"

/** @interface Odometer : VehiclePropertyType **/
class OdometerProperty: public DBusSink
{
public:
	OdometerProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.Odometer","/org/automotive/maintenance/Odometer", re, connection, map<string, string>())
	{
		/** @attributeName Odometer
		 *  @type unsigned long
		 *  @access readonly
		 **/
		wantProperty<uint>(VehicleProperty::Odometer, "Odometer", "i", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

/** @interface Battery : VehiclePropertyType **/
class BatteryProperty: public DBusSink
{
public:
	BatteryProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.Battery","/org/automotive/maintenance/Battery", re, connection, map<string, string>())
	{
		/** @attributeName Voltage
		 *  @type double
		 *  @access readonly
		 **/
		wantProperty<double>(VehicleProperty::BatteryVoltage, "Voltage", "d", AbstractProperty::Read);

		/** @attributeName Current
		 *  @type double
		 *  @access readonly
		 **/
		wantProperty<double>(VehicleProperty::BatteryCurrent, "Current", "d", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

/** @interface TirePressure : VehiclePropertyType **/
class TirePressureProperty: public DBusSink
{
public:
	TirePressureProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.TirePressure","/org/automotive/maintenance/TirePressure", re, connection, map<string, string>())
	{
		/** @attributeName LeftFront
		 *  @type double
		 *  @access readonly
		 **/
		wantProperty<double>(VehicleProperty::TirePressureLeftFront, "LeftFront", "d", AbstractProperty::Read);

		/** @attributeName RightFront
		 *  @type double
		 *  @access readonly
		 **/
		wantProperty<double>(VehicleProperty::TirePressureRightFront, "RightFront", "d", AbstractProperty::Read);

		/** @attributeName LeftRear
		 *  @type double
		 *  @access readonly
		 **/
		wantProperty<double>(VehicleProperty::TirePressureLeftRear, "LeftRear", "d", AbstractProperty::Read);

		/** @attributeName RightRear
		 *  @type double
		 *  @access readonly
		 **/
		wantProperty<double>(VehicleProperty::TirePressureRightRear, "RightRear", "d", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

/** @interface TireTemperature : VehiclePropertyType **/
class TireTemperatureProperty: public DBusSink
{
public:
	TireTemperatureProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.TireTemperature","/org/automotive/maintenance/TireTemperature", re, connection, map<string, string>())
	{
		/** @attributeName LeftFront
		 *  @type double
		 *  @access readonly
		 **/
		wantProperty<double>(VehicleProperty::TireTemperatureLeftFront, "LeftFront", "d", AbstractProperty::Read);

		/** @attributeName RightFront
		 *  @type double
		 *  @access readonly
		 **/
		wantProperty<double>(VehicleProperty::TireTemperatureRightFront, "RightFront", "d", AbstractProperty::Read);

		/** @attributeName LeftRear
		 *  @type double
		 *  @access readonly
		 **/
		wantProperty<double>(VehicleProperty::TireTemperatureLeftRear, "LeftRear", "d", AbstractProperty::Read);

		/** @attributeName RightRear
		 *  @type double
		 *  @access readonly
		 **/
		wantProperty<double>(VehicleProperty::TireTemperatureRightRear, "RightRear", "d", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

#endif
