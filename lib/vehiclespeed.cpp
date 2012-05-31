/*
    Copyright 2012 Kevron Rees <email>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/


#include "vehiclespeed.h"
#include "runningstatusinterface.h"
#include "debugout.h"

#include <glib.h>

VehicleSpeedProperty::VehicleSpeedProperty(): AbstractProperty("VehicleSpeed", "q", AbstractProperty::Read)
{
	if(RunningStatusInterface::iface == nullptr)
	{
		new RunningStatusInterface();
	}

	RunningStatusInterface::iface->addProperty(this);
}

void VehicleSpeedProperty::setValue(boost::any val)
{
	AbstractProperty::setValue(val);
	
	RunningStatusInterface::iface->updateValue(this);
}

GVariant* VehicleSpeedProperty::toGVariant()
{
	uint16_t v = 0;
	try
	{
		v = boost::any_cast<uint16_t>(mValue);
	}
	catch (...)
	{
		cerr<<"Could not cast VehicleSpeedProperty.  Maybe value is not uint16?";
		throw -1;
	}

	debugOut("TRACE");
	
	return g_variant_new_uint16(v);
}

void VehicleSpeedProperty::fromGVariant(GVariant *value)
{
	mValue = g_variant_get_uint16(value);
}
