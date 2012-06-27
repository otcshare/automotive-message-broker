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


#include "lightstatusproperty.h"
#include "runningstatusinterface.h"
#include "debugout.h"

void LightStatusProperty::setValue(LightStatusProperty::LightStatus val)
{
	AbstractProperty::setValue<LightStatusProperty::LightStatus>(val);
}

LightStatusProperty::LightStatus LightStatusProperty::value()
{
	return AbstractProperty::value<LightStatusProperty::LightStatus>();
}


void LightStatusProperty::fromGVariant(GVariant* value)
{

}

GVariant* LightStatusProperty::toGVariant()
{
	GVariantBuilder builder;
	
	g_variant_builder_init(&builder, G_VARIANT_TYPE(signature().c_str()));
	
	LightStatusProperty::LightStatus lights = value();
		
	for(LightStatus::const_iterator itr = lights.begin(); itr != lights.end(); itr++)
	{
		g_variant_builder_add(&builder, "{yb}", (char)(*itr).first, (*itr).second);
	}
	
	GVariant *variant = g_variant_new(signature().c_str(), &builder);
	g_variant_builder_unref(&builder);
	
	return variant;
}

LightStatusProperty::LightStatusProperty()
: AbstractProperty("LightStatus","a{yb}", AbstractProperty::Read, RunningStatusInterface::iface())
{

}

