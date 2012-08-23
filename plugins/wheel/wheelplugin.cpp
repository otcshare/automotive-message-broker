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

#include <gio/gunixinputstream.h>
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

	void gotData(GAsyncResult *res);

private:
	struct js_event jsEvent;
	GInputStream *gis;
	int *axis;
	char *button;

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


void readCallback(GObject *srcObj, GAsyncResult *res, gpointer userData)
{
	if (!userData) {
		throw std::runtime_error("Got a null WheelPrivate in the Read Callback!");
	}

	WheelPrivate *wp = (WheelPrivate *)userData;
	wp->gotData(res);

}

WheelPrivate::WheelPrivate()
:gis(nullptr), axis(nullptr), button(nullptr)
{

	unsigned char numAxes = 0;
	unsigned char numButtons = 0;
	int version = 0;
	int fd;
	char name[JSNAMELEN] = "Unknown";


	//FIXME: Support config file with joystick device mapping, button/axis mappings, etc.
	if ((fd = open("/dev/input/js0", O_RDONLY)) < 0) {
		throw std::runtime_error("Could not find a joystick class device!");	//FIXME: Later, don't throw, watch input devices, and jump on to any JS devices that appear
		return;
	}

	ioctl(fd, JSIOCGVERSION, &version);
	ioctl(fd, JSIOCGAXES, &numAxes);
	ioctl(fd, JSIOCGBUTTONS, &numButtons);
	ioctl(fd, JSIOCGNAME(JSNAMELEN), name);

	cout << "Driver version: " << (version >> 16) << "." << ((version >> 8) & 0xFF) << "." << (version & 0xFF) << "\n";
	cout << "JS Name: " << name << "\n";
	cout << "JS Axes/Buttons: " << (int)numAxes << "/" << (int)numButtons << "\n";
	cout << "Converting FD to GIO Input Stream...\n";
	this->axis = (int *)calloc(numAxes, sizeof(int));
	this->button = (char *)calloc(numButtons, sizeof(char));
	this->gis = g_unix_input_stream_new(fd, TRUE);
	g_input_stream_read_async(this->gis, &this->jsEvent, sizeof(struct js_event), G_PRIORITY_DEFAULT, nullptr, &readCallback, this);
	
}

WheelPrivate::~WheelPrivate()
{
	
	if (this->gis)
		g_input_stream_close_async(this->gis, G_PRIORITY_DEFAULT, NULL, NULL, NULL);
}

void WheelPrivate::gotData(GAsyncResult *res)
{
	GError *gerror = NULL;
	int size = g_input_stream_read_finish(this->gis, res, &gerror);

	if (res < 0) {
		throw std::runtime_error(gerror->message);
		g_error_free(gerror);
	}
	if (size != sizeof(struct js_event)) {
		cout << "Only read " << size << " bytes from js device - should have been " << sizeof(struct js_event) << " bytes!";
		throw std::runtime_error("Bad read from JS device!");
		return;

	} else {
		switch (this->jsEvent.type & ~JS_EVENT_INIT) {
			case JS_EVENT_BUTTON:
				this->button[this->jsEvent.number] = this->jsEvent.value;
				cout << "Got button event, btn# " << (int)this->jsEvent.number << ", val " << this->jsEvent.value << "\n";
				break;
			case JS_EVENT_AXIS:
				this->axis[this->jsEvent.number] = this->jsEvent.value;
				cout << "Got axis event, axis# " << (int)this->jsEvent.number << ", val " << this->jsEvent.value << "\n";
				break;
			default:
				cout << "Got JS event that wasn't button or axis!\n";
				break;
		}
	}

	g_input_stream_read_async(this->gis, &this->jsEvent, sizeof(struct js_event), G_PRIORITY_DEFAULT, nullptr, &readCallback, this);
}

