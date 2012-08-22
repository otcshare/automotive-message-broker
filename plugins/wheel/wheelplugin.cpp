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

#include "wheelplugin.h"

#include <iostream>
#include <stdexcept>
#include <boost/assert.hpp>
#include <glib.h>


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/joystick.h>

using namespace std;

#include "debugout.h"


#define JSNAMELEN 128


class WheelPrivate
{

public:
	WheelPrivate();
	~WheelPrivate();

	int fd;
	char doShutdown;
	char isShutdown;

private:
//	boost::thread pollThread;

};


static gboolean timeoutCallback(gpointer data)
{
	WheelSourcePlugin* src = (WheelSourcePlugin*)data;
	
	//src->randomizeProperties();
	
	return true;
}

WheelSourcePlugin::WheelSourcePlugin(AbstractRoutingEngine* re)
:AbstractSource(re), velocity(0), engineSpeed(0), wheel(new WheelPrivate())
{
	re->setSupported(supported(), this);
}

WheelSourcePlugin::~WheelSourcePlugin()
{
	delete this->wheel;
}



extern "C" AbstractSource * create(AbstractRoutingEngine* routingengine)
{
	return new WheelSourcePlugin(routingengine);
	
}

string WheelSourcePlugin::uuid()
{
	return "c0ffee8a-c605-4a06-9034-59c1deadbeef";
}

boost::any WheelSourcePlugin::getProperty(VehicleProperty::Property property)
{
	if(property == VehicleProperty::VehicleSpeed)
	{
		return velocity;
	}
	else if(property == VehicleProperty::EngineSpeed)
	{
		return engineSpeed;
	}
}

void WheelSourcePlugin::getPropertyAsync(AsyncPropertyReply *reply)
{
	if(reply->property == VehicleProperty::VehicleSpeed)
	{
		reply->value = velocity;
		reply->completed(reply);
	}
	else if(reply->property == VehicleProperty::EngineSpeed)
	{
		reply->value = engineSpeed;
		reply->completed(reply);
	}
}

void WheelSourcePlugin::setProperty(VehicleProperty::Property , boost::any )
{

}

void WheelSourcePlugin::subscribeToPropertyChanges(VehicleProperty::Property property)
{
	mRequests.push_back(property);
}

PropertyList WheelSourcePlugin::supported()
{
	PropertyList props;
	props.push_back(VehicleProperty::EngineSpeed);
	props.push_back(VehicleProperty::VehicleSpeed);
	props.push_back(VehicleProperty::TransmissionShiftPosition);
	props.push_back(VehicleProperty::ThrottlePosition);
	props.push_back(VehicleProperty::WheelBrake);
	props.push_back(VehicleProperty::SteeringWheelAngle);
	props.push_back(VehicleProperty::TurnSignal);
	props.push_back(VehicleProperty::ClutchStatus);
	props.push_back(VehicleProperty::EngineOilPressure);
	props.push_back(VehicleProperty::EngineCoolantTemperature);
	props.push_back(VehicleProperty::MachineGunTurretStatus);

	return props;
}

void WheelSourcePlugin::unsubscribeToPropertyChanges(VehicleProperty::Property property)
{
	mRequests.remove(property);
}

//PIMPL:


void pollJS(WheelPrivate *wp)
{
	if (!wp) {
		throw std::runtime_error("NULL WheelPrivate in pollJS!");
		return;
	}
	while (!wp->doShutdown) {
		
	}
	wp->isShutdown = 1;
}

WheelPrivate::WheelPrivate()
:fd(-1), isShutdown(0), doShutdown(0)
{

	unsigned char axes = 0;
	unsigned char buttons = 0;
	int version = 0;
	char name[JSNAMELEN] = "Unknown";


	//FIXME: Support config file with joystick device mapping, button/axis mappings, etc.
	if ((this->fd = open("/dev/input/js0", O_RDONLY)) < 0) {
		throw std::runtime_error("Could not find a joystick class device!");	//FIXME: Later, don't throw, watch input devices, and jump on to any JS devices that appear
		return;
	}

	ioctl(fd, JSIOCGVERSION, &version);
	ioctl(fd, JSIOCGAXES, &axes);
	ioctl(fd, JSIOCGBUTTONS, &buttons);
	ioctl(fd, JSIOCGNAME(JSNAMELEN), name);

	cout << "Driver version: " << (version >> 16) << "." << ((version >> 8) & 0xFF) << "." << (version & 0xFF) << "\n";
	cout << "JS Name: " << name << "\n";
	cout << "JS Axes/Buttons: " << (int)axes << "/" << (int)buttons << "\n";
	cout << "Starting polling thread...\n";
	
}

WheelPrivate::~WheelPrivate()
{
	if (this->fd != -1)
		close(fd);
}
