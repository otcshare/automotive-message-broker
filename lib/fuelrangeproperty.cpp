/*
    <one line to give the library's name and an idea of what it does.>
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

#include "fuelrangeproperty.h"
#include "runningstatusinterface.h"
void FuelRangeProperty::fromGVariant(GVariant* value)
{

}

GVariant* FuelRangeProperty::toGVariant()
{
	return g_variant_new_uint16(value<uint16_t>());
}

FuelRangeProperty::FuelRangeProperty()
	:AbstractProperty("FuelRange","q",AbstractProperty::Read, RunningStatusInterface::iface())
{

}

FuelRangeProperty& FuelRangeProperty::operator=(uint16_t const & other)
{
	setValue<uint16_t>(other);
	return *this;
}

