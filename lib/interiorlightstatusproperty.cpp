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


#include "interiorlightstatusproperty.h"
#include "runningstatusinterface.h"

void InteriorLightStatusProperty::setValue(InteriorLightStatusProperty::InteriorLightStatus val)
{
	AbstractProperty::setValue<InteriorLightStatusProperty::InteriorLightStatus>(val);
}

InteriorLightStatusProperty::InteriorLightStatus InteriorLightStatusProperty::value()
{
	return AbstractProperty::value<InteriorLightStatusProperty::InteriorLightStatus>();
}


void InteriorLightStatusProperty::fromGVariant(GVariant* value)
{

}

GVariant* InteriorLightStatusProperty::toGVariant()
{
	GVariantBuilder builder;
	
	g_variant_builder_init(&builder, G_VARIANT_TYPE(signature().c_str()));
	
	InteriorLightStatusProperty::InteriorLightStatus lights = value();
	
	for(InteriorLightStatus::const_iterator itr = lights.begin(); itr != lights.end(); itr++)
	{
		g_variant_builder_add(&builder, "{yb}", (char)(*itr).first, (*itr).second);
	}
	
	GVariant *variant = g_variant_new(signature().c_str(), &builder);
	g_variant_builder_unref(&builder);
	
	return variant;
}

InteriorLightStatusProperty::InteriorLightStatusProperty()
: AbstractProperty("InteriorLightStatus","a{yb}", AbstractProperty::Read, RunningStatusInterface::iface())
{

}

