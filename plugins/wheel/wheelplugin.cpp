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
#define LG27 "G27 Racing Wheel"

enum TurnSignal {
	TS_RIGHT=1,
	TS_LEFT=2
};

double gearRatio[8] = {
	0.0,	//Neutral
	1.0/4.12,	//First
	1.0/2.84,	//Second
	1.0/2.28,	//Third
	1.0/1.45,	//Fourth
	1.0/1.0,	//Fifth
	1.0/0.69,	//Sixth
	1.0/3.21	//Reverse
};
			

class WheelPrivate
{

public:
	WheelPrivate(WheelSourcePlugin *parent, AbstractRoutingEngine *route);
	~WheelPrivate();

	AbstractPropertyType getProperty(VehicleProperty::Property propType);

	friend void readCallback(GObject *srcObj, GAsyncResult *res, gpointer userData);


private:
	void gotData(GAsyncResult *res);
	void newButtonValue(char number, bool val);
	void newAxisValue(char number, int val);

	void changeMachineGuns(bool val);
	void changeTurnSignal(TurnSignal dir, bool val);
	void changeGear(int gear);
	void changeOilPressure(bool increase);
	void changeCoolantTemp(bool increase);

	void changeSteeringAngle(int val);
	void changeClutch(int val);
	void changeThrottle(int val);
	void changeBrake(int val);

	uint16_t calcCarSpeed();
	uint16_t calcRPM();

	AbstractRoutingEngine *re;
	GInputStream *gis;
	struct js_event jsEvent;
	VehicleProperty::Property btnMap[16];
	int *axis;
	char *button;

	uint16_t machineGuns;
	uint16_t turnSignal;
	uint16_t currentGear;
	uint16_t oilPSI;
	uint16_t coolantTemp;
	uint16_t steeringAngle;
	uint16_t throttle;
	bool clutch;
	bool oldClutch;
	bool brake;
	bool oldBrake;

};


WheelSourcePlugin::WheelSourcePlugin(AbstractRoutingEngine* re)
:AbstractSource(re)
{
	re->setSupported(supported(), this);
	this->mWheel = new WheelPrivate(this, re);
}

WheelSourcePlugin::~WheelSourcePlugin()
{
	delete this->mWheel;
}



extern "C" AbstractSource * create(AbstractRoutingEngine* routingengine)
{
	return new WheelSourcePlugin(routingengine);
}

string WheelSourcePlugin::uuid()
{
	return "c0ffee8a-c605-4a06-9034-59c1deadbeef";
}

AbstractPropertyType WheelSourcePlugin::getProperty(VehicleProperty::Property property)
{
	return this->mWheel->getProperty(property);
}

void WheelSourcePlugin::getPropertyAsync(AsyncPropertyReply *reply)
{
	reply->value = this->mWheel->getProperty(reply->property);
	reply->completed(reply);
}

void WheelSourcePlugin::setProperty(VehicleProperty::Property , AbstractPropertyType )
{

}

void WheelSourcePlugin::subscribeToPropertyChanges(VehicleProperty::Property property)
{
	mRequests.insert(property);
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
	mRequests.erase(property);
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

WheelPrivate::WheelPrivate(WheelSourcePlugin *parent, AbstractRoutingEngine *route)
:re(route), gis(nullptr), axis(nullptr), button(nullptr),
oilPSI(10), coolantTemp(100), turnSignal(0), throttle(0),
machineGuns(false), currentGear(0), steeringAngle(0),
clutch(false), oldClutch(false), brake(false), oldBrake(false)
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

	cout << "Driver version: " << (version >> 16) << "." << ((version >> 8) & 0xFF) << "." << (version & 0xFF) << endl;
	cout << "JS Name: " << name << endl;
	cout << "JS Axes/Buttons: " << (int)numAxes << "/" << (int)numButtons << endl;
	cout << "Converting FD to GIO Input Stream..." << endl;
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


boost::any WheelPrivate::getProperty(VehicleProperty::Property propType)
{
	if (propType == VehicleProperty::VehicleSpeed)
		return BasicPropertyType<uint16_t>(this->calcCarSpeed());
	else if (propType == VehicleProperty::EngineSpeed)
		return BasicPropertyType<uint16_t>(this->calcRPM());
	else if (propType == VehicleProperty::TransmissionShiftPosition)
		return this->currentGear;
	else if (propType == VehicleProperty::ThrottlePosition)
		return this->throttle;
	else if (propType == VehicleProperty::WheelBrake)
		return this->brake;
	else if (propType == VehicleProperty::SteeringWheelAngle)
		return this->steeringAngle;
	else if (propType == VehicleProperty::TurnSignal)
		return this->turnSignal;
	else if (propType == VehicleProperty::ClutchStatus)
		return this->clutch;
	else if (propType == VehicleProperty::EngineOilPressure)
		return this->oilPSI;
	else if (propType == VehicleProperty::EngineCoolantTemperature)
		return this->coolantTemp;
	else if (propType == VehicleProperty::MachineGunTurretStatus)
		return this->machineGuns;
	else
		cout << "Unhandled getProperty type: " << propType << endl;
}

void WheelPrivate::newButtonValue(char number, bool val)
{
	switch (number) {
		case 0:	//Gear attach diamond down
			break;
		case 1:	//Gear attach diamond left
			break;
		case 2:	//Gear attach diamond right
			break;
		case 3:	//Gear attach diamond up
			break;
		case 11://Gear attach red button row, left button
			break;
		case 8:	//Gear attach red button row, 2nd btn from left
			break;
		case 9:	//Gear attach red button row, 3rd btn from left
			break;
		case 10://Gear attach red button row, 4th btn from left (right button)
			break;
		case 4:	//Right paddle shifter
			this->changeMachineGuns(val);
			break;
		case 5:	//Left paddle shifter
			this->changeMachineGuns(val);
			break;
		case 6:	//Right upper wheel button
			this->changeTurnSignal(TS_RIGHT, val);
			break;
		case 18://Right middle wheel button
			//Oil pressure up
			if (val)
				this->changeOilPressure(true);
			break;
		case 19://Right lower wheel button
			//Coolant temperature up
			if (val)
				this->changeCoolantTemp(true);
			break;
		case 7:	//Left upper wheel button
			this->changeTurnSignal(TS_LEFT, val);
			break;
		case 20://Left middle wheel button
			//Oil pressure down
			if (val)
				this->changeOilPressure(false);
			break;
		case 21://Left lower wheel button
			//Coolant temperature down
			if (val)
				this->changeCoolantTemp(true);
			break;
		case 12://1st gear
			this->changeGear((val ? 1 : 0));
			break;
		case 13://2nd gear
			this->changeGear((val ? 2 : 0));
			break;
		case 14://3rd gear
			this->changeGear((val ? 3 : 0));
			break;
		case 15://4th gear
			this->changeGear((val ? 4 : 0));
			break;
		case 16://5th gear
			this->changeGear((val ? 5 : 0));
			break;
		case 17://6th gear
			this->changeGear((val ? 6 : 0));
			break;
		case 22://Reverse gear
			this->changeGear((val ? 128 : 0));
			break;
		default:
			cout << "Got unknown button number: " << (int)number << endl;
			break;
	}
}

void WheelPrivate::newAxisValue(char number, int val)
{
	switch (number) {
		case 0:	//Wheel angle, -32767 - 32767
			//VehicleProperty::SteeringWheelAngle
			this->changeSteeringAngle(val);
			break;
		case 1:	//Clutch, -32767 (depressed) - 32767 (undepressed)
			this->changeClutch(val);
			break;
		case 2:	//Throttle, -32767 (depressed) - 32767 (undepressed)
			//VehicleProperty::VehicleSpeed
			//VehicleProperty::EngineSpeed
			this->changeThrottle(val);
			break;
		case 3:	//Brake, -32767 (depressed) - 32767 (undepressed)
			this->changeBrake(val);
			break;
		case 4:	//D-Pad L/R, -32767 (L), 0 (Released), 32767 (R)
			break;
		case 5:	//D-Pad U/D, -32767 (U), 0 (Released), 32767 (D)
			break;
		default:
			cout << "Got unknown axis number: " << (int)number << endl;
			break;
	}
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
//				cout << "Got button event, btn# " << (int)this->jsEvent.number << ", val " << this->jsEvent.value << endl;
				this->newButtonValue(this->jsEvent.number, this->jsEvent.value);
				break;
			case JS_EVENT_AXIS:
				this->axis[this->jsEvent.number] = this->jsEvent.value;
//				cout << "Got axis event, axis# " << (int)this->jsEvent.number << ", val " << this->jsEvent.value << endl;
				this->newAxisValue(this->jsEvent.number, this->jsEvent.value);
				break;
			default:
				cout << "Got JS event that wasn't button or axis!" << endl;
				break;
		}
	}

	g_input_stream_read_async(this->gis, &this->jsEvent, sizeof(struct js_event), G_PRIORITY_DEFAULT, nullptr, &readCallback, this);
}


//Data handling functions


void WheelPrivate::changeMachineGuns(bool val)
{
	this->machineGuns = val;
	this->re->updateProperty(VehicleProperty::MachineGunTurretStatus, this->machineGuns);
}

void WheelPrivate::changeTurnSignal(TurnSignal dir, bool val)
{
	int tsVal=0;
	if (val) {
		if (dir == TS_LEFT)
			tsVal = 2;
		else
			tsVal = 1;
	}
	this->turnSignal = tsVal;
	this->re->updateProperty(VehicleProperty::TurnSignal, this->turnSignal);
}

void WheelPrivate::changeGear(int gear)
{
	this->currentGear = gear;
	this->re->updateProperty(VehicleProperty::TransmissionShiftPosition, this->currentGear);
	this->re->updateProperty(VehicleProperty::VehicleSpeed, this->calcCarSpeed());
}

void WheelPrivate::changeOilPressure(bool increase)
{
	this->re->updateProperty(VehicleProperty::EngineOilPressure, (increase ? ++this->oilPSI : --this->oilPSI));
}

void WheelPrivate::changeCoolantTemp(bool increase)
{
	this->re->updateProperty(VehicleProperty::EngineCoolantTemperature, (increase ? ++this->coolantTemp : --this->coolantTemp));
}


void WheelPrivate::changeSteeringAngle(int val)
{
	this->steeringAngle = (((double)val/(double)32767.0) + (double)1.0) * (double)180.0;
	this->re->updateProperty(VehicleProperty::SteeringWheelAngle, this->steeringAngle);
}

void WheelPrivate::changeClutch(int val)
{
	this->oldClutch = this->clutch;
	this->clutch = (val < 20000);
	if (this->oldClutch != this->clutch)
		this->re->updateProperty(VehicleProperty::ClutchStatus, this->clutch);
}

void WheelPrivate::changeThrottle(int val)
{
	this->throttle = ((double)(val - 32767)/(double)-65534.0)*(double)100.0;

	this->re->updateProperty(VehicleProperty::ThrottlePosition, this->throttle);
	this->re->updateProperty(VehicleProperty::EngineSpeed, this->calcRPM());
	this->re->updateProperty(VehicleProperty::VehicleSpeed, this->calcCarSpeed());
}

void WheelPrivate::changeBrake(int val)
{
	this->oldBrake = this->brake;
	this->brake = (val < 20000);
	if (this->oldBrake != this->brake)
		this->re->updateProperty(VehicleProperty::WheelBrake, this->brake);
}


uint16_t WheelPrivate::calcCarSpeed()
{
//	cout << "Calc Car Speed, rpm: " << this->calcRPM() << ", gearRatio: " << gearRatio[this->currentGear == 128 ? 7 : this->currentGear] << " current gear: " << this->currentGear << endl;
	return (this->calcRPM() * gearRatio[this->currentGear == 128 ? 7 : this->currentGear])/100;

}

uint16_t WheelPrivate::calcRPM()
{
//	cout << "Calc rpm, throttle: " << this->throttle << endl;
	return this->throttle * 100;
}

