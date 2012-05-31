/*
    Copyright 2012 Kevron Rees <email>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
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


