/*
 * Copyright (c) 2012, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

/* ---------------------- vehicle event typedef --------------------------- */

function VehicleEventType()
{
    this.event = [
"Randomize",
"AirConditioning",
"AirRecirculation",
"AirflowDirection",
"AvgKW",
"BatteryStatus",
"ChildLock",
"Defrost",
"ExteriorBrightness",
"ExteriorTemperature",
"FanSpeed",
"FrontWheelRadius",
"FullBatteryRange",
"InteriorTemperature",
"LightHazard",
"LightHead",
"LightParking",
"NightMode",
"Odometer",
"SeatHeater",
"TargetTemperature",
"TransmissionShiftPosition",
"VehicleSpeed",
"Weather"
    ];
    this.value = [];

    /* set random initial values for all the props */
    for(i in this.event)
    {
        var prop = this.event[i];
        this.value[prop] = Math.floor(Math.random() * 1000000);
    }
}

VehicleEventType.prototype.getSupportedEventList = function(val)
{
    /* for undefined just assume everything */
    if((val == undefined)||(val === ""))
        return this.event;

    /* grab every event with case insensitive prefix of val */
    var value = val.toLowerCase();
    var list = [];
    for(i in this.event)
    {
        var prop = this.event[i].toLowerCase();
        if(prop.indexOf(value) === 0)
        {
            list[list.length] = prop;
        }
    }

    /* if the target val isn't alone, remove it, it's a grouping */
    var idx = list.indexOf(value);
    if((idx >= 0)&&(list.length > 1))
    {
        list.splice(idx, 1);
    }
    return list;
}

VehicleEventType.prototype.getValueEventList = function(val)
{
    var i, j, list = this.getSupportedEventList(val);
    for(i = 0; i < list.length; i++)
    {
        for(j = i + 1; j < list.length; j++)
        {
            if(list[j].indexOf(list[i]) === 0)
            {
                list.splice(i, 1);
                i--;
            }
        }
    }
    return list;
}

VehicleEventType.prototype.getValuesEventList = function(vals)
{
    var i, j, list = [];
    for(i = 0; i < vals.length; i++)
    {
        var sublist = this.getValueEventList(vals[i]);
        for(j = 0; j < sublist.length; j++)
        {
            if(list.indexOf(sublist[j]) < 0)
            {
                list[list.length] = sublist[j];
            }
        }
    }
    return list;
}

VehicleEventType.prototype.isValueEvent = function(val)
{
    var list = this.getValueEventList(val);
    return(list.length === 1);
}

VehicleEventType.prototype.getValue = function(prop)
{
    return this.value[prop];
}

VehicleEventType.prototype.isValid = function(prop)
{
    return (this.event.indexOf(prop) >= 0);
}

VehicleEventType.prototype.setValue = function(prop, newval)
{
    this.value[prop] = newval;
}
