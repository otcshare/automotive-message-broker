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


#ifndef VEHICLESPEED_H
#define VEHICLESPEED_H

#include "abstractproperty.h"

class VehicleSpeedProperty : public AbstractProperty
{

public:
	VehicleSpeedProperty();
	void setValue(boost::any val);
	

	GVariant* toGVariant();
	void fromGVariant(GVariant *value);
};

#endif // VEHICLESPEED_H
