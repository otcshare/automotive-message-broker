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
