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

#include "hornproperty.h"
#include "runningstatusinterface.h"

void HornProperty::setValue(bool val)
{
    AbstractProperty::setValue<bool>(val);
}

bool HornProperty::value()
{
    return AbstractProperty::value<bool>();
}


void HornProperty::fromGVariant(GVariant* value)
{

}

GVariant* HornProperty::toGVariant()
{
    return g_variant_new_boolean(value());
}

HornProperty::HornProperty()
    :AbstractProperty("Horn","b",AbstractProperty::Read,RunningStatusInterface::iface())
{
	setValue(false);
}

