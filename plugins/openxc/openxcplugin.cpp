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

#include "bluetooth.hpp"

using namespace std;

#include "debugout.h"

bool gioPollingFunc(GIOChannel *source,GIOCondition condition,gpointer data)
{
	if (condition == G_IO_HUP)
	{
		//Hang up. Returning false closes out the GIOChannel.
		//printf("Callback on G_IO_HUP\n");
		return false;
	}

	OpenXCPlugin* plugin = static_cast<OpenXCPlugin*>(data);

	if(!plugin)
	{
		throw std::runtime_error("Bad cast to OpenXCPlugin*");
	}

	plugin->processData();

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

	bool test = false;
	if(config.find("test") != config.end())
	{
		test = config["test"] == "true";
	}

	if(test)
	{
		testParseEngine();
	}

	std::string bluetoothAddy = config["device"];
	std::string bluetoothAdapter = config["bluetoothAdapter"];
	std::string serialDevice;

	if(bluetoothAddy != "")
	{
		BluetoothDevice btDevice;

		serialDevice = btDevice.getDeviceForAddress(bluetoothAddy, bluetoothAdapter);
	}

	device = new SerialPort(serialDevice);
	if(!device->open())
	{
		throw std::runtime_error("unable to open serial device " + serialDevice);
	}

	GIOChannel *chan = g_io_channel_unix_new(device->fileDescriptor());
	g_io_add_watch(chan, G_IO_IN,(GIOFunc)gioPollingFunc, this);
	g_io_add_watch(chan,G_IO_HUP,(GIOFunc)gioPollingFunc,this);
	g_io_add_watch(chan,G_IO_ERR,(GIOFunc)gioPollingFunc,this);
	g_io_channel_unref(chan); //Pass ownership of the GIOChannel to the watch.


}



extern "C" AbstractSource * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new OpenXCPlugin(routingengine, config);
	
}

const string OpenXCPlugin::uuid()
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



bool OpenXCPlugin::translateOpenXCEvent(string json)
{
	/// signal:
	/// {"name": "steering_wheel_angle", "value": 45}

	/// event:
	/// {"name": "button_event", "value": "up", "event": "pressed"}

	json_object *rootobject;
	json_tokener *tokener = json_tokener_new();


	///TODO: we have several json leaks here

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

			return true;
		}

		else if (property == VehicleProperty::EngineSpeed)
		{
			int val = json_object_get_int(value);
			VehicleProperty::EngineSpeedType ambVal(val);

			routingEngine->updateProperty(property, &ambVal,uuid());
			return true;
		}

		else if (property == VehicleProperty::VehicleSpeed)
		{
			int val = json_object_get_int(value);
			VehicleProperty::VehicleSpeedType ambVal(val);

			routingEngine->updateProperty(property, &ambVal,uuid());
			return true;
		}

		else if (property == VehicleProperty::ThrottlePosition)
		{
			int val = json_object_get_int(value);
			VehicleProperty::ThrottlePositionType ambVal(val);

			routingEngine->updateProperty(property, &ambVal,uuid());

			return true;
		}

		else if (property == VehicleProperty::LightBrake)
		{
			bool val = json_object_get_boolean(value);
			VehicleProperty::LightBrakeType ambVal(val);

			routingEngine->updateProperty(property, &ambVal,uuid());

			return true;
		}

		else if (property == VehicleProperty::ParkingBrakeStatus)
		{
			bool val = json_object_get_boolean(value);
			VehicleProperty::ParkingBrakeStatusType ambVal(val);

			routingEngine->updateProperty(property, &ambVal,uuid());

			return true;
		}

		else if (property == VehicleProperty::TransmissionGearPosition)
		{
			/// TODO: implement
			return false;
		}

		else if (property == VehicleProperty::Odometer)
		{
			int val = json_object_get_int(value);
			VehicleProperty::OdometerType ambVal(val);

			routingEngine->updateProperty(property, &ambVal,uuid());

			return true;
		}

		else if (property == VehicleProperty::VehiclePowerMode)
		{
			/// TODO: implement
			return false;
		}

		else if (property == VehicleProperty::FuelLevel)
		{
			int val = json_object_get_int(value);
			VehicleProperty::FuelLevelType ambVal(val);

			routingEngine->updateProperty(property, &ambVal,uuid());

			return true;
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

			return true;
		}

		else if (property == VehicleProperty::LightHighBeam)
		{
			bool val = json_object_get_boolean(value);
			VehicleProperty::LightHighBeamType ambVal(val);

			routingEngine->updateProperty(property, &ambVal, uuid());

			return true;
		}

		else if (property == VehicleProperty::WindshieldWiper)
		{
			bool val = json_object_get_boolean(value);
			if(val)
			{
				Window::WiperSpeed speed = Window::Auto;

				VehicleProperty::WindshieldWiperType ambVal(speed);
				routingEngine->updateProperty(property, &ambVal, uuid());

				return true;
			}
			else
			{
				Window::WiperSpeed speed = Window::Off;

				VehicleProperty::WindshieldWiperType ambVal(speed);
				routingEngine->updateProperty(property, &ambVal, uuid());

				return true;
			}

		}

		else if (property == VehicleProperty::Latitude)
		{
			double val = json_object_get_double(value);
			VehicleProperty::LatitudeType ambVal(val);

			routingEngine->updateProperty(property, &ambVal, uuid());

			return true;
		}

		else if (property == VehicleProperty::Longitude)
		{
			double val = json_object_get_double(value);
			VehicleProperty::LongitudeType ambVal(val);

			routingEngine->updateProperty(property, &ambVal, uuid());

			return true;
		}

		else if (property == VehicleProperty::ButtonEvent)
		{
			/// TODO: implement
			DebugOut(DebugOut::Error)<<"OpenXC button event not implemented"<<endl;
			return false;
		}
	}

	return false;

}

void OpenXCPlugin::testParseEngine()
{
	bool passed = true;

	if(!translateOpenXCEvent("{\"name\": \"steering_wheel_angle\", \"value\": 45}"))
	{
		DebugOut(0)<<"OpenXC Parse Test (steering_wheel_angle): \tFailed"<<endl;
		passed = false;
	}
	else DebugOut(0)<<"OpenXC Parse Test (steering_wheel_angle): \tPassed"<<endl;

	if(!translateOpenXCEvent("{\"name\": \"engine_speed\", \"value\": 5000}"))
	{
		DebugOut(0)<<"OpenXC Parse Test (engine_speed): \t\tFailed"<<endl;
		passed = false;
	}
	else DebugOut(0)<<"OpenXC Parse Test (engine_speed): \t\tPassed"<<endl;
	if(!translateOpenXCEvent("{\"name\": \"vehicle_speed\", \"value\": 100}"))
	{
		DebugOut(0)<<"OpenXC Parse Test (vehicle_speed): \t\tFailed"<<endl;
		passed = false;
	}
	else DebugOut(0)<<"OpenXC Parse Test (vehicle_speed): \t\tPassed"<<endl;
	if(!translateOpenXCEvent("{\"name\": \"accelerator_pedal_position\", \"value\": 90}"))
	{
		DebugOut(0)<<"OpenXC Parse Test (accelerator_pedal_position): tFailed"<<endl;
		passed = false;
	}
	else DebugOut(0)<<"OpenXC Parse Test (accelerator_pedal_position):\tPassed"<<endl;
	if(!translateOpenXCEvent("{\"name\": \"parking_brake_status\", \"value\": \"false\" }"))
	{
		DebugOut(0)<<"OpenXC Parse Test (parking_brake_status):\tFailed"<<endl;
		passed = false;
	}
	else DebugOut(0)<<"OpenXC Parse Test (parking_brake_status):\tPassed"<<endl;
	if(!translateOpenXCEvent("{\"name\": \"brake_pedal_status\", \"value\": \"false\" }"))
	{
		DebugOut(0)<<"OpenXC Parse Test (brake_pedal_status):\tFailed"<<endl;
		passed = false;
	}
	else DebugOut(0)<<"OpenXC Parse Test (brake_pedal_status):\tPassed"<<endl;
	if(!translateOpenXCEvent("{\"name\": \"transmission_gear_position\", \"value\": \"fourth\"}"))
	{
		DebugOut(0)<<"OpenXC Parse Test (transmission_gear_position):\tFailed"<<endl;
		passed = false;
	}
	else DebugOut(0)<<"OpenXC Parse Test (transmission_gear_position):\tPassed"<<endl;
	if(!translateOpenXCEvent("{\"name\": \"odometer\", \"value\": 1000}"))
	{
		DebugOut(0)<<"OpenXC Parse Test (odometer): \t\tFailed"<<endl;
		passed = false;
	}
	else DebugOut(0)<<"OpenXC Parse Test (odometer): \t\tPassed"<<endl;
	if(!translateOpenXCEvent("{\"name\": \"ignition_status\", \"value\": \"run\"}"))
	{
		DebugOut(0)<<"OpenXC Parse Test (ignition_status): \t\tFailed"<<endl;
		passed = false;
	}
	else DebugOut(0)<<"OpenXC Parse Test (ignition_status): \t\tPassed"<<endl;
	if(!translateOpenXCEvent("{\"name\": \"fuel_level\", \"value\": 30}"))
	{
		DebugOut(0)<<"OpenXC Parse Test (fuel_level): \t\tFailed"<<endl;
		passed = false;
	}
	else DebugOut(0)<<"OpenXC Parse Test (fuel_level): \t\tPassed"<<endl;
	if(!translateOpenXCEvent("{\"name\": \"fuel_consumed_since_restart\", \"value\": 45}"))
	{
		DebugOut(0)<<"OpenXC Parse Test (fuel_consumed_since_restart):\tFailed"<<endl;
		passed = false;
	}
	else DebugOut(0)<<"OpenXC Parse Test (fuel_consumed_since_restart):\tPassed"<<endl;
	if(!translateOpenXCEvent("{\"name\": \"headlamp_status\", \"value\": \"true\"}"))
	{
		DebugOut(0)<<"OpenXC Parse Test (headlamp_status): \t\tFailed"<<endl;
		passed = false;
	}
	else DebugOut(0)<<"OpenXC Parse Test (headlamp_status): \t\tPassed"<<endl;
	if(!translateOpenXCEvent("{\"name\": \"latitude\", \"value\": 88.12125}"))
	{
		DebugOut(0)<<"OpenXC Parse Test (latitude): \t\tFailed"<<endl;
		passed = false;
	}
	else DebugOut(0)<<"OpenXC Parse Test (latitude): \t\tPassed"<<endl;
	if(!translateOpenXCEvent("{\"name\": \"longitude\", \"value\": 108.12125}"))
	{
		DebugOut(0)<<"OpenXC Parse Test (longitude): \t\tFailed"<<endl;
		passed = false;
	}
	else DebugOut(0)<<"OpenXC Parse Test (longitude): \t\tPassed"<<endl;
	if(!translateOpenXCEvent("{\"name\": \"button_event\", \"value\": \"up\", \"event\": \"pressed\"}"))
	{
		DebugOut(0)<<"OpenXC Parse Test (button_event): \t\tFailed"<<endl;
		passed = false;
	}
	else DebugOut(0)<<"OpenXC Parse Test (button_event): \t\tPassed"<<endl;

	if(!passed)
	{
		DebugOut(0)<<"Some OpenXC Parse Tests failed.  Aborting";
		throw std::runtime_error("OpenXC Parse tests failed.");
	}
}


void OpenXCPlugin::processData()
{
	std::string data = device->read();

	translateOpenXCEvent(data);
}
