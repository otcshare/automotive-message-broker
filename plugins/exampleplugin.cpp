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

#include "debugout.h"

ExamplePlugin::ExamplePlugin()
{
	uint16_t velocity = 5;
	
	VehicleSpeedProperty* speed = new VehicleSpeedProperty();
	speed->setValue(velocity);
	
	EngineSpeedProperty* engineSpeed = new EngineSpeedProperty();
	engineSpeed->setValue(2000);
	
	VehiclePowerModeProperty* vehiclePowerMode = new VehiclePowerModeProperty();
	vehiclePowerMode->setValue(VehiclePowerModeProperty::Run);
	
	FuelEconomyProperty* fueleconomyproperty = new FuelEconomyProperty();
	fueleconomyproperty->setValue(42);
	
	TripMeters trips;
	trips.push_back(1500);
	trips.push_back(1000);
	
	TripMeterProperty* tripmeterproperty = new TripMeterProperty();
	tripmeterproperty->setValue(trips);
	
	TransmissionGearStatusProperty* transmissiongearstatus = new TransmissionGearStatusProperty();
	transmissiongearstatus->setValue(TransmissionGearStatusProperty::First);
	
	CruiseControlStatus cruisecontrolstatus = CruiseControlStatus(true, velocity);
	DebugOut()<<"Speed: "<<cruisecontrolstatus.speed<<endl;
		
	CruiseControlStatusProperty * cruisecontrolstatusproperty = new CruiseControlStatusProperty();
	cruisecontrolstatusproperty->setValue(cruisecontrolstatus);
	
	WheelBrakeProperty *wheelbrakeproperty = new WheelBrakeProperty();
	wheelbrakeproperty->setValue(false);
	
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
	lightstatusproperty->setValue(lights);
	
	InteriorLightStatusProperty::InteriorLightStatus interiorLights;
	interiorLights[InteriorLightStatusProperty::Driver] = false;
	interiorLights[InteriorLightStatusProperty::Passenger] = true;
	interiorLights[InteriorLightStatusProperty::Center] = false;
	
	InteriorLightStatusProperty* interiorlightstatusproperty = new InteriorLightStatusProperty();
	interiorlightstatusproperty->setValue(interiorLights);
	
}

extern "C" void create()
{
	new ExamplePlugin;
}
