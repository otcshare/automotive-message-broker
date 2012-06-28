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

#include "runningstatusinterface.h"
#include <iostream>

#include "debugout.h"
#include "abstractproperty.h"

using namespace std;

RunningStatusInterface* RunningStatusInterface::mIface = nullptr;

RunningStatusInterface* RunningStatusInterface::iface()
{
	if(mIface == nullptr)
		mIface = new RunningStatusInterface();
	
	return mIface;
}


RunningStatusInterface::RunningStatusInterface()
	: AbstractDBusInterface(DBusServiceName".RunningStatus", "/runningStatus")
{
	
}


GVariant* RunningStatusInterface::getProperty(std::string propertyName)
{
	if(properties.count(propertyName))
		return properties[propertyName]->toGVariant();
	else
		throw -1;
}

void RunningStatusInterface::setProperty(std::string propertyName, GVariant* value)
{
	if(properties.count(propertyName))
	{
		properties[propertyName]->fromGVariant(value);
	}
	else
	{
		throw -1;
	}
}


