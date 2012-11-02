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

#include "tpmsplugin.h"

#include <iostream>
#include <boost/assert.hpp>
#include <glib.h>
#include <libusb.h>

using namespace std;

#include "debugout.h"

#define ENDPOINT_IN             0x81
#define ENDPOINT_OUT            0x01

#define DEVICE_VID 0x0000
#define DEVICE_PID 0x0001

#define MAX_SENSORS 4

//timeout for performing interrupt r/w operations in milliseconds
#define INTR_TIMEOUT 1000

#define PSI_MULTIPLIER 14.5038
#define KPA_MULTIPLIER 100
#define PRESSURE_SCALE 0.025
 
#define FARENHEIT_MULTIPLIER 1.8


static gboolean timeoutCallback(gpointer data)
{
	TpmsPlugin* src = (TpmsPlugin*)data;
	
	int r = src->readValues();

	return true;
}

TpmsPlugin::TpmsPlugin(AbstractRoutingEngine* re, map<string, string> config)
:AbstractSource(re, config)
{
    lfPressure = rfPressure = lrPressure = rrPressure = 0;
    lfTemperature = rfTemperature = lrTemperature = rrTemperature = 0;

    int r = 1;
    
    r = libusb_init(NULL);
    if (r < 0) {
      DebugOut() << "TPMS: Plugin load failure. Failed to initialize libusb" << endl;
    }
    else {
      r = findDevice();
      if (r < 0) {
        DebugOut() << "TPMS: Plugin load failure. Could not find/open device - run as root?" << endl;
      }
      else {
        // need to detach device from kernel driver before claiming the interface
        r = detachDevice();
        if (r < 0) {
          DebugOut() << "TPMS: Plugin load failure. USB device detach failed with code " << r << endl;
        }
        else {
          r = libusb_claim_interface(mDeviceHandle, 0);
          if (r < 0) {
            DebugOut() << "TPMS: Plugin load failure. usb_claim_interface error " << r << endl;
          }
          else {
            DebugOut() << "TPMS: USB interface initialized" << endl;	

            re->setSupported(supported(), this);
            g_timeout_add(5000, timeoutCallback, this );
            DebugOut() << "TPMS: set to read sensor every 5 seconds" << endl;
          }
        }
      }
    }
}


extern "C" AbstractSource * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new TpmsPlugin(routingengine, config);
	
}

string TpmsPlugin::uuid()
{
	return "CHANGE THIS 6dd4268a-c605-4a06-9034-59c1e8344c8e";
}


void TpmsPlugin::getPropertyAsync(AsyncPropertyReply *reply)
{
	DebugOut() << "TPMS: getPropertyAsync called for property: " << reply->property << endl;

    if(reply->property == VehicleProperty::TirePressureLeftFront) {
      VehicleProperty::TirePressureType temp(lfPressure);
      reply->value = &temp;
      reply->completed(reply);
    }
    else if(reply->property == VehicleProperty::TirePressureRightFront) {
      VehicleProperty::TirePressureType temp(rfPressure);
      reply->value = &temp;
      reply->completed(reply);
    }
    else if(reply->property == VehicleProperty::TirePressureLeftRear) {
      VehicleProperty::TirePressureType temp(lrPressure);
      reply->value = &temp;
      reply->completed(reply);
    }
    else if(reply->property == VehicleProperty::TirePressureRightRear) {
      VehicleProperty::TirePressureType temp(rrPressure);
      reply->value = &temp;
      reply->completed(reply);
    }
    else if(reply->property == VehicleProperty::TireTemperatureLeftFront) {
      VehicleProperty::EngineSpeedType temp(lfTemperature);
      reply->value = &temp;
      reply->completed(reply);
    }
    else if(reply->property == VehicleProperty::TireTemperatureRightFront) {
      VehicleProperty::EngineSpeedType temp(rfTemperature);
      reply->value = &temp;
      reply->completed(reply);
    }
    else if(reply->property == VehicleProperty::TireTemperatureLeftRear) {
      VehicleProperty::EngineSpeedType temp(lrTemperature);
      reply->value = &temp;
      reply->completed(reply);
    }
    else if(reply->property == VehicleProperty::TireTemperatureRightRear) {
      VehicleProperty::EngineSpeedType temp(rrTemperature);
      reply->value = &temp;
      reply->completed(reply);
    }

    else {
      DebugOut() << "TPMS: no such getProperty type: " << reply->property << endl;
      reply->value = nullptr;
      reply->completed(reply);
	}
}

void TpmsPlugin::getRangePropertyAsync(AsyncRangePropertyReply *reply)
{
	///not supported
	reply->completed(reply);
}

AsyncPropertyReply *TpmsPlugin::setProperty(AsyncSetPropertyRequest request )
{
	return NULL;
}

void TpmsPlugin::subscribeToPropertyChanges(VehicleProperty::Property property)
{
	mRequests.push_back(property);
}

PropertyList TpmsPlugin::supported()
{
	PropertyList props;
    props.push_back(VehicleProperty::TirePressureLeftFront);
	props.push_back(VehicleProperty::TirePressureRightFront);
	props.push_back(VehicleProperty::TirePressureLeftRear);
	props.push_back(VehicleProperty::TirePressureRightRear);
	props.push_back(VehicleProperty::TireTemperatureLeftFront);
	props.push_back(VehicleProperty::TireTemperatureRightFront);
	props.push_back(VehicleProperty::TireTemperatureLeftRear);
	props.push_back(VehicleProperty::TireTemperatureRightRear);
	
	return props;
}

void TpmsPlugin::unsubscribeToPropertyChanges(VehicleProperty::Property property)
{
	mRequests.remove(property);
}

int TpmsPlugin::findDevice(void)
{
  int deviceVid = DEVICE_VID;
  int devicePid = DEVICE_PID;

  DebugOut() << "TPMS: Trying to open USB device with VID: " << deviceVid << " PID: " << devicePid << endl;
  mDeviceHandle = libusb_open_device_with_vid_pid(NULL, DEVICE_VID, DEVICE_PID);

  return mDeviceHandle ? 0 : -1;
}


int TpmsPlugin::detachDevice(void)
{
  int r;
  r = libusb_kernel_driver_active(mDeviceHandle, 0);
  if (r == 1) {
    DebugOut() << "TPMS: USB device seems to be kernel driven, trying to detach" << endl;
    r = libusb_detach_kernel_driver(mDeviceHandle, 0);
  }
  return r;
}


int TpmsPlugin::exitClean(int deinit)
{
  if (deinit) {
    libusb_release_interface(mDeviceHandle, 0);
    libusb_attach_kernel_driver(mDeviceHandle, 0);
    libusb_close(mDeviceHandle);
  }
  libusb_exit(NULL);
}


int TpmsPlugin::readValues()
{
  int snum;
  unsigned char buf[4];

  // Sensor 1 = Left Front
  // Sensor 2 = Right Front
  // Sensor 3 = Left Rear
  // Sensor 4 = Right Rear

  for (snum = 1; snum <= MAX_SENSORS; snum++) {
    readUsbSensor(snum, buf);

    // only do this if sensor is available
    if (buf[3] != 0xff) {
      string mode_string;
      char print_string[100];

      switch (snum) {
      case 1:
        lfPressure = ((float)buf[0]-40) * PRESSURE_SCALE * KPA_MULTIPLIER;
        lfTemperature = (float)buf[1]-40;
        sprintf(print_string, "TPMS: Left front pressure = %5.1f kPa, temperature = %5.1f degrees Celsius", lfPressure, lfTemperature);
        DebugOut() << print_string << endl;
        break;
      case 2:
        rfPressure = (buf[0]-40) * PRESSURE_SCALE * KPA_MULTIPLIER;
        rfTemperature = buf[1]-40;
        sprintf(print_string, "TPMS: Right front pressure = %5.1f kPa, temperature = %5.1f degrees Celsius", rfPressure, rfTemperature);
        DebugOut() << print_string << endl;
        break;
      case 3:
        lrPressure = (buf[0]-40) * PRESSURE_SCALE * KPA_MULTIPLIER;
        lrTemperature = buf[1]-40;
        sprintf(print_string, "TPMS: Left rear pressure = %5.1f kPa, temperature = %5.1f degrees Celsius", lrPressure, lrTemperature);
        DebugOut() << print_string << endl;
        break;
      case 4:
        rrPressure = (buf[0]-40) * PRESSURE_SCALE * KPA_MULTIPLIER;
        rrTemperature = buf[1]-40;
        sprintf(print_string, "TPMS: Right rear pressure = %5.1f kPa, temperature = %5.1f degrees Celsius", rrPressure, rrTemperature);
        DebugOut() << print_string << endl;
        break;
      }

      // make sensor mode human-readable
      // FIXME: for future reference, modes not being used
      switch (buf[3]) {
        case 0x01: mode_string = "normal"; break;
        case 0x02: mode_string = "pressure_alert"; break;
        // more to add here...
        default: mode_string = "unknown"; break;
      }
    }
    else {
      DebugOut() << "TPMS: Unable to read sensor " << sensorNumberToString(snum) << " (" << snum << ")" << endl;
    }
  }

  VehicleProperty::TirePressureType lfPres(lfPressure);
  VehicleProperty::TirePressureType rfPres(rfPressure);
  VehicleProperty::TirePressureType lrPres(lrPressure);
  VehicleProperty::TirePressureType rrPres(rrPressure);
  VehicleProperty::TireTemperatureType lfTemp(lfTemperature);
  VehicleProperty::TireTemperatureType rfTemp(rfTemperature);
  VehicleProperty::TireTemperatureType lrTemp(lrTemperature);
  VehicleProperty::TireTemperatureType rrTemp(rrTemperature);

  routingEngine->updateProperty(VehicleProperty::TirePressureLeftFront, &lfPres);
  routingEngine->updateProperty(VehicleProperty::TirePressureRightFront, &rfPres);
  routingEngine->updateProperty(VehicleProperty::TirePressureLeftRear, &lrPres);
  routingEngine->updateProperty(VehicleProperty::TirePressureRightRear, &rrPres);
  routingEngine->updateProperty(VehicleProperty::TireTemperatureLeftFront, &lfTemp);
  routingEngine->updateProperty(VehicleProperty::TireTemperatureRightFront, &rfTemp);
  routingEngine->updateProperty(VehicleProperty::TireTemperatureLeftRear, &lrTemp);
  routingEngine->updateProperty(VehicleProperty::TireTemperatureRightRear, &rrTemp);

  return 0;
}

int TpmsPlugin::readUsbSensor(int sid, unsigned char *buf)
{
  int r, transferred;

  buf[0] = 0x20 + sid;
  r = libusb_interrupt_transfer(mDeviceHandle, ENDPOINT_OUT, buf, 1, &transferred, INTR_TIMEOUT);
  if (r < 0) {
    DebugOut() << "TPMS: USB write interrupt failed, code " << r << endl;
  }

  r = libusb_interrupt_transfer(mDeviceHandle, ENDPOINT_IN, buf, 4, &transferred, INTR_TIMEOUT);
  if (r < 0) {
    DebugOut() << "TPMS: USB read interrupt failed, code " << r << endl;
  }

  return r;
}


string TpmsPlugin::sensorNumberToString(int snid)
{
  switch (snid) {
  case 1: return "left front"; break;
  case 2: return "right front"; break;
  case 3: return "left rear"; break;
  case 4: return "right rear"; break;
  default: return "unknown";
  }
}
