/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2012  Michael Carpenter <email>

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


#ifndef TCPSINKPLUGIN_H
#define TCPSINKPLUGIN_H
#include <glib.h>
#include <abstractroutingengine.h>
#include "abstractsink.h"
class TcpSinkPlugin : public AbstractSink
{

public:
	TcpSinkPlugin(AbstractRoutingEngine* re);
	string uuid() ;
	void propertyChanged(VehicleProperty::Property property, boost::any value, string  uuid);
	void supportedChanged(PropertyList supportedProperties);
	PropertyList subscriptions();

};

#endif // TCPSINKPLUGIN_H
