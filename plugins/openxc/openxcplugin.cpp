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

#include "openxcplugin.h"
#include "timestamp.h"

#include <iostream>
#include <boost/assert.hpp>
#include <glib.h>

using namespace std;

#include "debugout.h"

static gboolean timeoutCallback(gpointer data)
{
	OpenXCPlugin* src = (OpenXCPlugin*)data;
	
	src->randomizeProperties();
	
	return true;
}

OpenXCPlugin::OpenXCPlugin(AbstractRoutingEngine* re, map<string, string> config)
:AbstractSource(re, config)
{
	re->setSupported(supported(), this);	

	/// populate the openxc to amb map:

	openXC2AmbMap["steering_wheel_angle"] = VehicleProperty::SteeringWheelAngle;
	//openXC2AmbMap["torque_at_transmission"] = VehicleProperty::Engine;
	openXC2AmbMap["engine_speed"] = VehicleProperty::EngineSpeed;
	openXC2AmbMap["vehicle_speed"] = VehicleProperty::VehicleSpeed;
	openXC2AmbMap["accelerator_pedal_position"] = VehicleProperty::ThrottlePosition;
	openXC2AmbMap["parking_brake_status"] = VehicleProperty::ParkingBrakeStatus;
	openXC2AmbMap["brake_pedal_status"] = VehicleProperty::LightBrake;
	openXC2AmbMap["transmission_gear_position"] = VehicleProperty::TransmissionGearPosition;
	openXC2AmbMap["odometer"] = VehicleProperty::Odometer;
	openXC2AmbMap["ignition_status"] = VehicleProperty::VehiclePowerMode;
	openXC2AmbMap["fuel_level"] = VehicleProperty::FuelLevel;
	openXC2AmbMap["fuel_consumed_since_restart"] = VehicleProperty::FuelConsumption;
	openXC2AmbMap["headlamp_status"] = VehicleProperty::LightHead;
	openXC2AmbMap["high_beam_status"] = VehicleProperty::LightHighBeam;
	openXC2AmbMap["windshield_wiper_status"] = VehicleProperty::WindshieldWiper;
	openXC2AmbMap["latitude"] = VehicleProperty::Latitude;
	openXC2AmbMap["longitude"] = VehicleProperty::Longitude;
	openXC2AmbMap["button_event"] = VehicleProperty::ButtonEvent;


}



extern "C" AbstractSource * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new OpenXCPlugin(routingengine, config);
	
}

string OpenXCPlugin::uuid()
{
	return "openxc";
}


void OpenXCPlugin::getPropertyAsync(AsyncPropertyReply *reply)
{

}

void OpenXCPlugin::getRangePropertyAsync(AsyncRangePropertyReply *reply)
{

}

AsyncPropertyReply *OpenXCPlugin::setProperty(AsyncSetPropertyRequest request )
{

}

void OpenXCPlugin::subscribeToPropertyChanges(VehicleProperty::Property property)
{
	mRequests.push_back(property);
}

PropertyList OpenXCPlugin::supported()
{
	PropertyList props;
	
	return props;
}

int OpenXCPlugin::supportedOperations()
{
	return Get;
}

void OpenXCPlugin::unsubscribeToPropertyChanges(VehicleProperty::Property property)
{
	mRequests.remove(property);
}



void OpenXCPlugin::translateOpenXCEvent(string json)
{
	/// signal:
	/// {"name": "steering_wheel_angle", "value": 45}

	/// event:
	/// {"name": "button_event", "value": "up", "event": "pressed"}

	json_object *rootobject;
	json_tokener *tokener = json_tokener_new();


	enum json_tokener_error err;
	do
	{
		rootobject = json_tokener_parse_ex(tokener, json.c_str(),json.length());
	} while ((err = json_tokener_get_error(tokener)) == json_tokener_continue);
	if (err != json_tokener_success)
	{
		fprintf(stderr, "Error: %s\n", json_tokener_error_desc(err));
		// Handle errors, as appropriate for your application.
	}
	if (tokener->char_offset < json.length()) // XXX shouldn't access internal fields
	{
		// Handle extra characters after parsed object as desired.
		// e.g. issue an error, parse another object from that point, etc...
	}

	VehicleProperty::Property property;

	json_object *name = json_object_object_get(rootobject,"name");
	if (name)
	{
		string namestr = string(json_object_get_string(name));

		property = openXC2AmbMap[namestr];
	}

	json_object *value = json_object_object_get(rootobject, "value");
	if(value)
	{
		if (property == VehicleProperty::SteeringWheelAngle)
		{
			int val = json_object_get_int(value);
			VehicleProperty::SteeringWheelAngleType ambVal(val);

			routingEngine->updateProperty(property, &ambVal,uuid());
		}

		else if (property == VehicleProperty::EngineSpeed)
		{
			int val = json_object_get_int(value);
			VehicleProperty::EngineSpeedType ambVal(val);

			routingEngine->updateProperty(property, &ambVal,uuid());
		}

		else if (property == VehicleProperty::VehicleSpeed)
		{
			int val = json_object_get_int(value);
			VehicleProperty::VehicleSpeedType ambVal(val);

			routingEngine->updateProperty(property, &ambVal,uuid());
		}

		else if (property == VehicleProperty::ThrottlePosition)
		{
			int val = json_object_get_int(value);
			VehicleProperty::ThrottlePositionType ambVal(val);

			routingEngine->updateProperty(property, &ambVal,uuid());
		}

		else if (property == VehicleProperty::LightBrake)
		{
			bool val = json_object_get_boolean(value);
			VehicleProperty::LightBrakeType ambVal(val);

			routingEngine->updateProperty(property, &ambVal,uuid());
		}

		else if (property == VehicleProperty::TransmissionGearPosition)
		{

		}

		else if (property == VehicleProperty::Odometer)
		{
			int val = json_object_get_int(value);
			VehicleProperty::OdometerType ambVal(val);

			routingEngine->updateProperty(property, &ambVal,uuid());
		}

		else if (property == VehicleProperty::VehiclePowerMode)
		{

		}

		else if (property == VehicleProperty::FuelLevel)
		{
			int val = json_object_get_int(value);
			VehicleProperty::FuelLevelType ambVal(val);

			routingEngine->updateProperty(property, &ambVal,uuid());
		}

		else if (property == VehicleProperty::FuelConsumption)
		{
			/// This is a different concept than our 'instant' fuel consumption.
			/// TODO: we should probably do a CumulativeFuelConsuption
		}

		else if (property == VehicleProperty::DoorStatus)
		{

		}

		else if (property == VehicleProperty::LightHead)
		{
			bool val = json_object_get_boolean(value);
			VehicleProperty::LightHeadType ambVal(val);

			routingEngine->updateProperty(property, &ambVal,uuid());
		}

		else if (property == VehicleProperty::LightHighBeam)
		{
			bool val = json_object_get_boolean(value);
			VehicleProperty::LightHighBeamType ambVal(val);

			routingEngine->updateProperty(property, &ambVal, uuid());
		}

		else if (property == VehicleProperty::WindshieldWiper)
		{
			bool val = json_object_get_boolean(value);
			if(val)
			{
				Window::WiperSpeed speed = Window::Auto;

				VehicleProperty::WindshieldWiperType ambVal(speed);
				routingEngine->updateProperty(property, &ambVal, uuid());
			}
			else
			{
				Window::WiperSpeed speed = Window::Off;

				VehicleProperty::WindshieldWiperType ambVal(speed);
				routingEngine->updateProperty(property, &ambVal, uuid());
			}

		}

		else if (property == VehicleProperty::Latitude)
		{
			double val = json_object_get_double(value);
			VehicleProperty::LatitudeType ambVal(val);

			routingEngine->updateProperty(property, &ambVal, uuid());
		}

		else if (property == VehicleProperty::Longitude)
		{
			double val = json_object_get_double(value);
			VehicleProperty::LongitudeType ambVal(val);

			routingEngine->updateProperty(property, &ambVal, uuid());
		}

		else if (property == VehicleProperty::ButtonEvent)
		{

		}
	}


}
