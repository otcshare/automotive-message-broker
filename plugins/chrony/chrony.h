/*
    Copyright (C) 2012  Intel Corporation
    Copyright (C) 2015  AWTC Europe

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


#ifndef CHRONYSINK_H
#define CHRONYSINK_H

#include "ambpluginimpl.h"

#define CHRONYD_SOCKET "/tmp/chrony.gps.sock"

struct chrony_sock_sample {
  struct timeval tv;
  double offset;
  int pulse;
  int leap;
  int _pad;
  int magic;
};

class ChronySink : public AmbPluginImpl
{

public:
	ChronySink(AbstractRoutingEngine* re, const std::map<std::string, std::string>& config, AbstractSource& parent);
	~ChronySink() { };
	const std::string uuid() const { return "35324592-db72-11e4-b432-0022684a4a24"; }
	virtual void init();
	virtual PropertyList subscriptions();
	virtual void propertyChanged( AbstractPropertyType* value);

};

#endif // CHRONYSINK_H
