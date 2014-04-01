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
#include "timestamp.h"

#define JSNAMELEN 128
#define LG27 "G27 Racing Wheel"
#define MAX_GEARS 6

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

	AbstractPropertyType *getProperty(VehicleProperty::Property propType);

	friend void readCallback(GObject *srcObj, GAsyncResult *res, gpointer userData);


private:
	void gotData(GAsyncResult *res);
	void newButtonValue(char number, bool val);
	void newAxisValue(char number, int val);

	void changeMachineGuns(bool val);
	void changeTurnSignal(TurnSignals::TurnSignalType dir, bool val);
	void changeGear(int gear);
	void changeOilPressure(bool increase);
	void changeCoolantTemp(bool increase);

	void changeSteeringAngle(int val);
	void changeClutch(int val);
	void changeThrottle(int val);
	void changeBrake(int val);

	uint16_t calcCarSpeed();
	uint16_t calcRPM();
	void checkButtonEvents();

	AbstractRoutingEngine *re;
	GInputStream *gis;
	struct js_event jsEvent;
	VehicleProperty::Property btnMap[16];
	int *axis;
	char *button;

	uint16_t machineGuns;
	TurnSignals::TurnSignalType turnSignal;
	Transmission::TransmissionPositions currentGear;
	uint16_t oilPSI;
	uint16_t coolantTemp;
	uint16_t steeringAngle;
	uint16_t throttle;
	bool clutch;
	bool oldClutch;
	bool brake;
	bool oldBrake;
	AbstractSource* mParent;

	VehicleProperty::ButtonEventType tempButton;
};


WheelSourcePlugin::WheelSourcePlugin(AbstractRoutingEngine* re, map<string, string> config)
:AbstractSource(re, config)
{
	this->mWheel = new WheelPrivate(this, re);
}

WheelSourcePlugin::~WheelSourcePlugin()
{
	delete this->mWheel;
}



extern "C" AbstractSource * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new WheelSourcePlugin(routingengine, config);
}

const string WheelSourcePlugin::uuid()
{
	return "c0ffee8a-c605-4a06-9034-59c1deadbeef";
}

void WheelSourcePlugin::getPropertyAsync(AsyncPropertyReply *reply)
{


	reply->value = this->mWheel->getProperty(reply->property);

	if(reply->value)
		reply->success = true;

	reply->completed(reply);

	delete reply->value;
}

AsyncPropertyReply *WheelSourcePlugin::setProperty(AsyncSetPropertyRequest request )
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
	props.push_back(VehicleProperty::TransmissionGearPosition);
	props.push_back(VehicleProperty::TransmissionMode);
	props.push_back(VehicleProperty::ThrottlePosition);
	props.push_back(VehicleProperty::WheelBrake);
	props.push_back(VehicleProperty::SteeringWheelAngle);
	props.push_back(VehicleProperty::TurnSignal);
	props.push_back(VehicleProperty::ClutchStatus);
	props.push_back(VehicleProperty::EngineOilPressure);
	props.push_back(VehicleProperty::EngineCoolantTemperature);
	props.push_back(VehicleProperty::MachineGunTurretStatus);
	props.push_back(VehicleProperty::ButtonEvent);

	return props;
}

int WheelSourcePlugin::supportedOperations()
{
	return Get | Set;
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
oilPSI(10), coolantTemp(100), turnSignal(TurnSignals::Off), throttle(0),
machineGuns(false), currentGear(Transmission::Neutral), steeringAngle(0),
  clutch(false), oldClutch(false), brake(false), oldBrake(false), mParent(parent)
{

	unsigned char numAxes = 0;
	unsigned char numButtons = 0;
	int version = 0;
	int fd;
	char name[JSNAMELEN] = "Unknown";
	struct js_corr cal[6];
	int i, j;
	//FIXME: Ugly as all get-out, but gets the job done quick...
	unsigned int calData[36] = {
		1, 0, 8191, 8192, 65542, 65534,
		1, 0, 127, 128, 4227201, 4194176,
		1, 0, 127, 128, 4227201, 4194176,
		1, 0, 127, 128, 4227201, 4194176,
		1, 0, 0, 0, 536854528, 536854528,
		1, 0, 0, 0, 536854528, 536854528
	};
	

	//FIXME: Support config file with joystick device mapping, button/axis mappings, etc.
	std::string jsdev = parent->getConfiguration()["device"];

	DebugOut(0)<<"JSDev: "<<jsdev<<endl;

	if(jsdev == "")
		jsdev = "/dev/input/js0";

	if ((fd = open(jsdev.c_str(), O_RDONLY)) < 0) {
		throw std::runtime_error("Could not find a joystick class device!");	//FIXME: Later, don't throw, watch input devices, and jump on to any JS devices that appear
		return;
	}

	ioctl(fd, JSIOCGVERSION, &version);
	ioctl(fd, JSIOCGAXES, &numAxes);
	ioctl(fd, JSIOCGBUTTONS, &numButtons);
	ioctl(fd, JSIOCGNAME(JSNAMELEN), name);

	for (i = 0; i < 6; i++) {
		int k = 0;

                cal[i].type = calData[(i*6)+k];
		k++;
                cal[i].prec = calData[(i*6)+k];
		k++;

                for(j = 0; j < 4; j++) {
			cal[i].coef[j] = calData[(i*6)+k];
			k++;
                }
        }
	if (ioctl(fd, JSIOCSCORR, &cal) < 0) {
		throw std::runtime_error("Could not set calibration data!");
		return;
	}

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


AbstractPropertyType *WheelPrivate::getProperty(VehicleProperty::Property propType)
{
	if (propType == VehicleProperty::VehicleSpeed)
		return new VehicleProperty::VehicleSpeedType(this->calcCarSpeed());
	else if (propType == VehicleProperty::EngineSpeed)
		return new VehicleProperty::EngineSpeedType(this->calcRPM());
	else if (propType == VehicleProperty::TransmissionShiftPosition)
		return new VehicleProperty::TransmissionShiftPositionType(this->currentGear);
	else if (propType == VehicleProperty::TransmissionGearPosition)
		return new VehicleProperty::TransmissionGearPositionType(this->currentGear);
	else if (propType == VehicleProperty::TransmissionMode)
		return new VehicleProperty::TransmissionModeType(Transmission::Sports);
	else if (propType == VehicleProperty::ThrottlePosition)
		return new VehicleProperty::ThrottlePositionType(this->throttle);
	else if (propType == VehicleProperty::WheelBrake)
		return new VehicleProperty::WheelBrakeType(this->brake);
	else if (propType == VehicleProperty::SteeringWheelAngle)
		return new VehicleProperty::SteeringWheelAngleType(this->steeringAngle);
	else if (propType == VehicleProperty::TurnSignal)
		return new VehicleProperty::TurnSignalType(this->turnSignal);
	else if (propType == VehicleProperty::ClutchStatus)
		return new VehicleProperty::ClutchStatusType(this->clutch);
	else if (propType == VehicleProperty::EngineOilPressure)
		return new VehicleProperty::EngineOilPressureType(this->oilPSI);
	else if (propType == VehicleProperty::EngineCoolantTemperature)
		return new VehicleProperty::EngineCoolantTemperatureType(this->coolantTemp);
	else if (propType == VehicleProperty::MachineGunTurretStatus)
		return new VehicleProperty::MachineGunTurretStatusType(this->machineGuns);
	else if (propType == VehicleProperty::ButtonEvent)
		return new VehicleProperty::ButtonEventType(tempButton);
	else
		cout << "Unhandled getProperty type: " << propType << endl;

	return nullptr;
}

void WheelPrivate::newButtonValue(char number, bool val)
{
	switch (number) {
		case 0:	//Gear attach diamond down
			checkButtonEvents();
			break;
		case 1:	//Gear attach diamond left
			checkButtonEvents();
			break;
		case 2:	//Gear attach diamond right
			checkButtonEvents();
			break;
		case 3:	//Gear attach diamond up
			checkButtonEvents();
			break;
		case 11://Gear attach red button row, left button
			checkButtonEvents();
			break;
		case 8:	//Gear attach red button row, 2nd btn from left
			checkButtonEvents();
			break;
		case 9:	//Gear attach red button row, 3rd btn from left
			checkButtonEvents();
			break;
		case 10://Gear attach red button row, 4th btn from left (right button)
			checkButtonEvents();
			break;
		case 4:	//Right paddle shifter
			if(val && this->currentGear < MAX_GEARS)
			{
				this->changeGear(Transmission::TransmissionPositions(this->currentGear+1));
				changeMachineGuns(val);
			}
			break;
		case 5:	//Left paddle shifter
			if(val && this->currentGear > 0)
				this->changeGear(Transmission::TransmissionPositions(this->currentGear-1));
			break;
		case 6:	//Right upper wheel button
			this->changeTurnSignal(TurnSignals::Right, val);
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
			this->changeTurnSignal(TurnSignals::Left, val);
			break;
		case 20://Left middle wheel button
			//Oil pressure down
			if (val)
				this->changeOilPressure(false);
			break;
		case 21://Left lower wheel button
			//Coolant temperature down
			if (val)
                this->changeCoolantTemp(false);
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
			DebugOut() << "Got unknown button number: " << (int)number << endl;
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
	VehicleProperty::MachineGunTurretStatusType temp(this->machineGuns);
	temp.timestamp = amb::currentTime();
	this->re->updateProperty(&temp, mParent->uuid());
}

void WheelPrivate::changeTurnSignal(TurnSignals::TurnSignalType dir, bool val)
{
	TurnSignals::TurnSignalType tsVal= TurnSignals::Off;
	if (val) {
		if (dir == TurnSignals::Left)
			tsVal = TurnSignals::Left;
		else
			tsVal = TurnSignals::Right;
	}
	this->turnSignal = tsVal;
	VehicleProperty::TurnSignalType temp(this->turnSignal);
	temp.timestamp = amb::currentTime();
	this->re->updateProperty(&temp, mParent->uuid());
}

void WheelPrivate::changeGear(int gear)
{
	this->currentGear = (Transmission::TransmissionPositions)gear;
	VehicleProperty::TransmissionShiftPositionType tempTrans(this->currentGear);
	VehicleProperty::TransmissionGearPositionType tempTransGear(this->currentGear);
	VehicleProperty::VehicleSpeedType tempSpeed(this->calcCarSpeed());

	this->re->updateProperty(&tempTrans, mParent->uuid());
	this->re->updateProperty(&tempTransGear, mParent->uuid());
	this->re->updateProperty(&tempSpeed, mParent->uuid());
}

void WheelPrivate::changeOilPressure(bool increase)
{
	VehicleProperty::EngineOilPressureType temp(increase ? ++this->oilPSI : --this->oilPSI);
	temp.timestamp = amb::currentTime();
	this->re->updateProperty(&temp, mParent->uuid());
}

void WheelPrivate::changeCoolantTemp(bool increase)
{
	VehicleProperty::EngineCoolantTemperatureType temp(increase ? ++this->coolantTemp : --this->coolantTemp);
	temp.timestamp = amb::currentTime();
	this->re->updateProperty(&temp, mParent->uuid());
}


void WheelPrivate::changeSteeringAngle(int val)
{
	this->steeringAngle = (((double)val/(double)32767.0) + (double)1.0) * (double)180.0;
	VehicleProperty::SteeringWheelAngleType temp(this->steeringAngle);
	temp.timestamp = amb::currentTime();
	this->re->updateProperty(&temp, mParent->uuid());
}

void WheelPrivate::changeClutch(int val)
{
	this->oldClutch = this->clutch;
	this->clutch = (val < 20000);
	if (this->oldClutch != this->clutch)
	{
		VehicleProperty::ClutchStatusType temp(this->clutch);
		temp.timestamp = amb::currentTime();
		this->re->updateProperty(&temp, mParent->uuid());
	}
}

void WheelPrivate::changeThrottle(int val)
{
	this->throttle = ((double)(val - 32767)/(double)-65534.0)*(double)100.0;

	VehicleProperty::ThrottlePositionType tempThrottle(this->throttle);
	VehicleProperty::EngineSpeedType tempRpm(this->calcRPM());
	VehicleProperty::VehicleSpeedType tempSpeed(this->calcCarSpeed());

	this->re->updateProperty(&tempThrottle, mParent->uuid());
	this->re->updateProperty(&tempRpm, mParent->uuid());
	this->re->updateProperty(&tempSpeed, mParent->uuid());
}

void WheelPrivate::changeBrake(int val)
{
	this->oldBrake = this->brake;
	this->brake = (val < 20000);
	if (this->oldBrake != this->brake)
	{
		VehicleProperty::WheelBrakeType temp(this->brake);
		this->re->updateProperty(&temp, mParent->uuid());
	}
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

void WheelPrivate::checkButtonEvents()
{
	if (this->button[0]) {
		//	cout << "Inside button 11!" << endl;
		tempButton = ButtonEvents::StopButton;

	}
	if (this->button[1]) {
		//	cout << "Inside button 11!" << endl;
		tempButton = (ButtonEvents::PrevButton);
	}
	if (this->button[2]) {
		tempButton = (ButtonEvents::SkipButton);

	}
	if (this->button[3]) {
		tempButton = ButtonEvents::PlayButton;

	}
	if (this->button[11]) {
		tempButton = (ButtonEvents::Preset1Button);
	}
	if (this->button[8]) {
		tempButton = (ButtonEvents::Preset2Button);
	}
	if (this->button[9]) {
		tempButton = (ButtonEvents::Preset3Button);

	}
	if (this->button[10]) {
		tempButton = (ButtonEvents::Preset4Button);
	}

	this->re->updateProperty(&tempButton, mParent->uuid());
}
