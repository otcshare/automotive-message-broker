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

#ifndef RUNNINGSTATUSINTERFACE_H
#define RUNNINGSTATUSINTERFACE_H

#include "abstractdbusinterface.h"


class RunningStatusInterface : public AbstractDBusInterface
{
public:
	static RunningStatusInterface* iface();

protected:
	RunningStatusInterface();
	
	virtual GVariant* getProperty(std::string propertyName);
	virtual void setProperty(std::string propertyName, GVariant* value);
	
	static RunningStatusInterface* mIface;
};



#endif // RUNNINGSTATUSINTERFACE_H
