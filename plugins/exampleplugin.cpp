/*
Copyright (C) 2012 Intel Corporation

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "exampleplugin.h"

#include <iostream>
#include <boost/assert.hpp>

using namespace std;

#include "enginespeedproperty.h"
#include "vehiclepowermodeproperty.h"
#include "fueleconomyproperty.h"
#include "tripmeterproperty.h"
#include "transmissiongearstatus.h"
#include "cruisecontrolstatus.h"
#include "wheelbrakeproperty.h"
#include "lightstatusproperty.h"
#include "interiorlightstatusproperty.h"
#include "hornproperty.h"
#include "chime.h"
#include "fuellevelproperty.h"
#include "fuelrangeproperty.h"
#include "engineoilproperty.h"
#include "enginecoolantproperty.h"
#include "accelerationproperty.h"
#include "steeringwheelangleproperty.h"

#include "debugout.h"

ExamplePlugin::ExamplePlugin()
{
	int velocity = 5;
	
	VehicleSpeedProperty* speed = new VehicleSpeedProperty();
	*speed = velocity;
	
	EngineSpeedProperty* engineSpeed = new EngineSpeedProperty();
	engineSpeed->setValue(2000);
	
	VehiclePowerModeProperty* vehiclePowerMode = new VehiclePowerModeProperty();
	*vehiclePowerMode = VehiclePowerModeProperty::Run;
	
	FuelEconomyProperty* fueleconomyproperty = new FuelEconomyProperty();
	*fueleconomyproperty = 42;
	
	TripMeters trips;
	trips.push_back(1500);
	trips.push_back(1000);
	
	TripMeterProperty* tripmeterproperty = new TripMeterProperty();
	*tripmeterproperty = trips;
	
	TransmissionGearStatusProperty* transmissiongearstatus = new TransmissionGearStatusProperty();
	transmissiongearstatus->setValue(TransmissionGearStatusProperty::First);
	
	CruiseControlStatus cruisecontrolstatus = CruiseControlStatus(true, velocity);

	CruiseControlStatusProperty * cruisecontrolstatusproperty = new CruiseControlStatusProperty();
	*cruisecontrolstatusproperty = cruisecontrolstatus;
	
	WheelBrakeProperty *wheelbrakeproperty = new WheelBrakeProperty();
	*wheelbrakeproperty = false;
	
	LightStatusProperty::LightStatus lights;
	lights[LightStatusProperty::Brake] = wheelbrakeproperty->value();
	lights[LightStatusProperty::Fog] = true;
	lights[LightStatusProperty::Head] = true;
	lights[LightStatusProperty::HighBeam] = false;
	lights[LightStatusProperty::Hazard] = false;
	lights[LightStatusProperty::LeftTurn] = false;
	lights[LightStatusProperty::RightTurn] = false;
	lights[LightStatusProperty::Parking] = false;
	
	LightStatusProperty *lightstatusproperty = new LightStatusProperty();
	*lightstatusproperty = lights;
	
	InteriorLightStatusProperty::InteriorLightStatus interiorLights;
	interiorLights[InteriorLightStatusProperty::Driver] = false;
	interiorLights[InteriorLightStatusProperty::Passenger] = true;
	interiorLights[InteriorLightStatusProperty::Center] = false;
	
	InteriorLightStatusProperty* interiorlightstatusproperty = new InteriorLightStatusProperty();
	*interiorlightstatusproperty = interiorLights;

	HornProperty *hornProperty = new HornProperty();
	*hornProperty = false;

	Chime * chime = new Chime();
	*chime = false;

	bool val = *chime;

	BOOST_ASSERT(val == false);

	FuelLevelProperty *fuelLevel = new FuelLevelProperty();
	*fuelLevel = (uint8_t)95;

	uint8_t fl = *fuelLevel;

	BOOST_ASSERT(fl == 95);

	FuelRangeProperty* fuelRange = new FuelRangeProperty();

	*fuelRange = 321;

	uint16_t fr = *fuelRange;

	BOOST_ASSERT(fr == 321);

	EngineOilProperty *oilproperty = new EngineOilProperty();

	EngineOil oil;
	oil.temperature = 32;
	oil.pressure = 400;
	oil.remaining = 88;

	*oilproperty = oil;

	EngineOil ol = *oilproperty;

	BOOST_ASSERT(ol == oil);

	EngineCoolant coolant;
	coolant.level= 99;
	coolant.temperature = 44;

	EngineCoolantProperty *coolantProp = new EngineCoolantProperty();
	*coolantProp = coolant;

	EngineCoolant ec = *coolantProp;

	BOOST_ASSERT(ec == coolant);

	AccelerationProperty* accelerationProp = new AccelerationProperty();
	Acceleration accel;
	accel.x = 1;
	accel.y = 0.25;
	accel.z = 0;

	*accelerationProp = accel;

	SteeringWheelAngleProperty * steeringWheelAngle = new SteeringWheelAngleProperty();

	*steeringWheelAngle = 100;
}

extern "C" void create()
{
	new ExamplePlugin;
}
