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


#include "transmissiongearstatus.h"
#include "runningstatusinterface.h"

void TransmissionGearStatusProperty::setValue(TransmissionGearStatusProperty::TransmissionGearStatus status)
{
	AbstractProperty::setValue<TransmissionGearStatusProperty::TransmissionGearStatus>(status);
}


TransmissionGearStatusProperty::TransmissionGearStatus TransmissionGearStatusProperty::value()
{
	return AbstractProperty::value<TransmissionGearStatusProperty::TransmissionGearStatus>();
}


void TransmissionGearStatusProperty::fromGVariant(GVariant* value)
{
	setValue((TransmissionGearStatusProperty::TransmissionGearStatus)g_variant_get_byte(value));
}

GVariant* TransmissionGearStatusProperty::toGVariant()
{
	return g_variant_new_byte( (gchar) value() );
}

TransmissionGearStatusProperty::TransmissionGearStatusProperty()
: AbstractProperty("TransmissionGearStatus", "b", AbstractProperty::Read, RunningStatusInterface::iface())
{

}

