#ifndef _MAINTENANCE_H_
#define _MAINTENANCE_H_


#include "dbusplugin.h"
#include "abstractdbusinterface.h"
#include "abstractroutingengine.h"


class OdometerProperty: public DBusSink
{
public:
	OdometerProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.odometer","/org/automotive/maintenance/odometer", re, connection, map<string, string>())
	{
		wantProperty<uint>(VehicleProperty::Odometer,"Odometer", "i", AbstractProperty::Read);

		supportedChanged(re->supported());
	}
};

class BatteryProperty: public DBusSink
{
public:
	BatteryProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.battery","/org/automotive/maintenance/battery", re, connection, map<string, string>())
	{
		wantProperty<double>(VehicleProperty::BatteryVoltage,"Voltage", "d", AbstractProperty::Read);
		wantProperty<double>(VehicleProperty::BatteryCurrent,"Current", "d", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};


class TirePressureProperty: public DBusSink
{
public:
	TirePressureProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.tirePressure","/org/automotive/maintenance/tirePressure", re, connection, map<string, string>())
	{
		wantProperty<double>(VehicleProperty::TirePressureLeftFront,"LeftFront", "d", AbstractProperty::Read);
		wantProperty<double>(VehicleProperty::TirePressureRightFront,"RightFront", "d", AbstractProperty::Read);
		wantProperty<double>(VehicleProperty::TirePressureLeftRear,"LeftRear", "d", AbstractProperty::Read);
		wantProperty<double>(VehicleProperty::TirePressureRightRear,"RightRear", "d", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

class TireTemperatureProperty: public DBusSink
{
public:
	TireTemperatureProperty(AbstractRoutingEngine* re, GDBusConnection* connection)
		:DBusSink("org.automotive.tireTemperature","/org/automotive/maintenance/tireTemperature", re, connection, map<string, string>())
	{
		wantProperty<double>(VehicleProperty::TireTemperatureLeftFront,"LeftFront", "d", AbstractProperty::Read);
		wantProperty<double>(VehicleProperty::TireTemperatureRightFront,"RightFront", "d", AbstractProperty::Read);
		wantProperty<double>(VehicleProperty::TireTemperatureLeftRear,"LeftRear", "d", AbstractProperty::Read);
		wantProperty<double>(VehicleProperty::TireTemperatureRightRear,"RightRear", "d", AbstractProperty::Read);
		supportedChanged(re->supported());
	}
};

#endif
