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

#include "gpsdplugin.h"
#include "timestamp.h"

#include <iostream>
#include <boost/assert.hpp>

using namespace std;

#include "debugout.h"

static int updateGpsposition(gpointer data)
{
	GpsdPlugin::Shared* shared = static_cast<GpsdPlugin::Shared*>(data);

#if (GPSD_API_MAJOR_VERSION >= 5)

	if (!gps_waiting (&shared->gps, 0))
	{
		return 1;
	}

	if(gps_read(&shared->gps) == -1)
	{
		DebugOut(0)<<"failed to read gps data from gpsd."<<endl;
		return 1;
	}
#else

	gps_poll(&gps);

#endif

	double time = amb::currentTime();

	if(shared->gps.fix.mode > 2)
	{
		if(shared->gps.fix.latitude != shared->oldlat)
			shared->oldlat = shared->gps.fix.latitude;
		if(shared->gps.fix.longitude != shared->oldlon)
			shared->oldlon = shared->gps.fix.longitude;
		if(shared->gps.fix.altitude != shared->oldalt)
			shared->oldalt = shared->gps.fix.altitude;
		if(shared->gps.fix.track != shared->oldheading)
			shared->oldheading = shared->gps.fix.track;
		if(shared->gps.fix.speed * MPS_TO_KPH != shared->oldspeed)
		{
			shared->oldspeed = shared->gps.fix.speed * MPS_TO_KPH;
		}

		shared->parent->updateProperty();
	}

	return 1;
}

GpsdPlugin::GpsdPlugin(AbstractRoutingEngine* re, map<string, string> config)
	:AbstractSource(re, config)
{
	shared = new Shared(this);

#if(GPSD_API_MAJOR_VERSION >= 5)
	int ret = gps_open("localhost","2947", &shared->gps);

	if(ret != 0)
	{
		debugOut("failed to open gps");
		DebugOut()<<"return value: "<<ret<<endl;
		(void)fprintf(stderr,"no gpsd running or network error: %d, %s\n",
					  errno, gps_errstr(errno));
		return;
	}
#else
	shared->gps = *(gps_open("localhost","2947"));

#endif

	gps_stream(&shared->gps, WATCH_ENABLE | WATCH_JSON, NULL);

	int timeout=200;

	if(config.find("timeout") != config.end())
	{
		timeout = atoi(config["timeout"].c_str());
	}

	updateVelocity = false;

	if(config.find("updateVelocity") != config.end())
	{
		updateVelocity = config["updateVelocity"] == "true";
	}

	g_timeout_add(timeout,updateGpsposition,shared);

	re->setSupported(supported(), this);
}

GpsdPlugin::~GpsdPlugin()
{
	(void) gps_stream(&shared->gps, WATCH_DISABLE, NULL);
	gps_close(&shared->gps);
}

extern "C" AbstractSource * create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new GpsdPlugin(routingengine, config);
	
}

string GpsdPlugin::uuid()
{
	return "326011dd-65cd-4be6-a75e-3e8d46a05b5e";
}


void GpsdPlugin::getPropertyAsync(AsyncPropertyReply *reply)
{
	/*if(reply->property == VehicleProperty::ExteriorBrightness)
	{
		replyQueue.push_back(reply);
	}*/

//	else  ///We don't support what you are asking for.  Reply false
	{
		reply->value = NULL;
		reply->success = false;
		reply->completed(reply);
	}
}

void GpsdPlugin::getRangePropertyAsync(AsyncRangePropertyReply *reply)
{
	throw std::runtime_error("GpsdPlugin does not support this operation.  We should never hit this method.");
}

AsyncPropertyReply *GpsdPlugin::setProperty(AsyncSetPropertyRequest request )
{
	throw std::runtime_error("GpsdPlugin does not support this operation.  We should never hit this method.");
}

void GpsdPlugin::subscribeToPropertyChanges(VehicleProperty::Property property)
{

	//mRequests.push_back(property);
}

void GpsdPlugin::unsubscribeToPropertyChanges(VehicleProperty::Property property)
{
	//mRequests.remove(property);
}

PropertyList GpsdPlugin::supported()
{
	PropertyList props;
	props.push_back(VehicleProperty::Latitude);
	props.push_back(VehicleProperty::Longitude);
	props.push_back(VehicleProperty::Altitude);
	props.push_back(VehicleProperty::Direction);

	if(updateVelocity)
		props.push_back(VehicleProperty::VehicleSpeed);

	
	return props;
}

int GpsdPlugin::supportedOperations()
{
	return Get;
}


void GpsdPlugin::updateProperty()
{
	VehicleProperty::LatitudeType lat(shared->oldlat);
	routingEngine->updateProperty(VehicleProperty::Latitude,&lat, uuid());

	VehicleProperty::LongitudeType lon(shared->oldlon);
	routingEngine->updateProperty(VehicleProperty::Longitude, &lon, uuid());

	VehicleProperty::AltitudeType alt(shared->oldalt);
	routingEngine->updateProperty(VehicleProperty::Altitude, &alt, uuid());

	VehicleProperty::DirectionType heading(shared->oldheading);
	routingEngine->updateProperty(VehicleProperty::Direction, &heading, uuid());

	if(updateVelocity)
	{
		VehicleProperty::VehicleSpeedType speed(shared->oldspeed);
		routingEngine->updateProperty(VehicleProperty::VehicleSpeed, &speed, uuid());
	}
}

