/*
    Copyright (C) 2012  Intel Corporation

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


#include "vehiclepowermodeproperty.h"
#include "runningstatusinterface.h"

#include <glib.h>

void VehiclePowerModeProperty::setValue(VehiclePowerModeProperty::VehiclePowerMode mode)
{
	AbstractProperty::setValue<VehiclePowerModeProperty::VehiclePowerMode>(mode);
}

VehiclePowerModeProperty::VehiclePowerMode VehiclePowerModeProperty::value()
{
	return AbstractProperty::value<VehiclePowerModeProperty::VehiclePowerMode>();
}


void VehiclePowerModeProperty::fromGVariant(GVariant* value)
{
	setValue((VehiclePowerMode)g_variant_get_byte(value));
}

GVariant* VehiclePowerModeProperty::toGVariant()
{
	return g_variant_new_byte( (gchar) value() );
}

VehiclePowerModeProperty::VehiclePowerModeProperty()
: AbstractProperty("VehiclePowerMode", "y", AbstractProperty::Read, RunningStatusInterface::iface())
{
	setValue(VehiclePowerModeProperty::Off);
}

