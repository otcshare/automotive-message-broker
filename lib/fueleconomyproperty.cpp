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


#include "fueleconomyproperty.h"
#include "runningstatusinterface.h"

void FuelEconomyProperty::fromGVariant(GVariant* value)
{
	mValue = (uint16_t)g_variant_get_uint16(value);
}

GVariant* FuelEconomyProperty::toGVariant()
{
	return g_variant_new_uint16(AbstractProperty::value<uint16_t>());
}

FuelEconomyProperty::FuelEconomyProperty()
: AbstractProperty("FuelEconomy","q",AbstractProperty::Read, RunningStatusInterface::iface())
{
	
}

void FuelEconomyProperty::setValue(uint16_t val)
{
	AbstractProperty::setValue<uint16_t>(val);
}
