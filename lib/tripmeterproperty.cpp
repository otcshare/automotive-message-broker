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


#include "tripmeterproperty.h"
#include "runningstatusinterface.h"


void TripMeterProperty::setValue(TripMeters trips)
{
	AbstractProperty::setValue<TripMeters>(trips);
}

TripMeters TripMeterProperty::value()
{
	return AbstractProperty::value<TripMeters>();
}


void TripMeterProperty::fromGVariant(GVariant* value)
{
	
}

GVariant* TripMeterProperty::toGVariant()
{
	
	TripMeters list = AbstractProperty::value<TripMeters>();
	
	GVariant* variant;
	
	GVariantBuilder params;
	g_variant_builder_init(&params, G_VARIANT_TYPE(signature().c_str()));
	for(TripMeters::iterator itr = list.begin(); itr != list.end(); itr++)
	{
		g_variant_builder_add_value(&params, g_variant_new_uint32(*itr));
	}
	
	variant = g_variant_new("au",&params);
	g_variant_builder_unref(&params);
	
	return variant;
}

TripMeterProperty::TripMeterProperty()
: AbstractProperty("TripMeter", "au", AbstractProperty::ReadWrite, RunningStatusInterface::iface())
{

}

