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

using namespace std;

RunningStatusInterface* RunningStatusInterface::iface = nullptr;

RunningStatusInterface::RunningStatusInterface()
	: AbstractDBusInterface(DBusServiceName".RunningStatus", "/runningStatus")
{
	if(iface != nullptr)
	{
		cerr<<"only one instance of this class may exist "<<__FUNCTION__<<" "<<__FILE__<<":"<<__LINE__<<endl;
		throw -1;
	}

	iface = this;
}


GVariant* RunningStatusInterface::getProperty(std::string propertyName)
{
	debugOut("TRACE");
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


