/*
 * Copyright (c) 2012, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

/*****************************************************************************
* Class name: Vehicle
* Description:
*    A javascript implementation of the IVI vehicle API that communicates
*    to the automotive message broker through a websocket
* Optional constructor arguments:
*    sCB: success callback, called when socket is connected, argument is 
*         success message string
*    eCB: error callback, called on socket close or error, argument is error
*         message string
*    url: the URL to use for the websocket, in the form "ws://host:port/script"
*    protocol: the protocol to use for the websocket, default is "http-only"
*
* [Public Member functions]
*  Function name: getSupportedEventTypes(type, writeable, successCB, errorCB)
*    Description:
*        Retrieves a list of vehicle events for the requested type
*    Required arguments:
*        type: target event or group to query (use empty string for all events)
*        writeable: if true, return only writeable events, otherwise get all
*        successCB: success callback, gets called with a string list of names
*              for all the events and event groups that are children of the 
*              target. e.g. "vehicle_info" returns all events/groups with the 
*              vehicle_info prefix. If the target is an event group, it's
*              omitted from the returned list
*        errorCB: error callback, called with error message string
*
*  Function name: get(eventlist, successCB, errorCB)
*    Description:
*        Retrieves a list of event/value pairs for a target list of event names
*    Required arguments:
*        eventlist[]: list of events to read (use empty string for all events)
*        successCB: success callback, gets called with the event/value pair list
*                   for all requested events. The list is the in the 
*                   form of data[n].name/data[n].value
*        errorCB: error callback, called with error message string
*
*  Function name: getHistory(event, startTime, endTime, successCB, errorCB)
*    Description:
*        Retrieves a list of event/value pairs for a target list of event names
*    Required arguments:
*        event: event to read
*        startTime: start date/time
*        endTime: end date/time
*        successCB: success callback, gets called with the event/value pair list
*                   for all requested events. The list is the in the
*                   form of data[n].name/data[n].value
*        errorCB: error callback, called with error message string
*
*
*  Function name: set(eventlist, valuelist, successCB, errorCB)
*    Description:
*        Sets a gourp of event's values (triggers error on read-only events)
*    Required arguments:
*        eventlist: target events to set
*        valuelist: target event values
*        successCB: success callback, gets called with the eventlist
*                   that was successfully set
*        errorCB: error callback, called with error message string
*
*  Function name: subscribe(eventlist, successCB, errorCB)
*    Description:
*        Subscribe to a list of events so you can listen to value changes, they
*        can be monitored with document.addEventListener(eventname, callback, false);
*        The Event object passed to the callback has two parameters, e.name and 
*        e.value. Events are sent to the handler individually.
*    Required arguments:
*        eventlist: target events to listen to
*        successCB: success callback, gets called with the eventlist
*                   that was successfully subscribed
*        errorCB: error callback, called with the eventlist that failed to subscribe
*
*  Function name: unsubscribe(eventlist, successCB, errorCB)
*    Description:
*        Unsubscribe to a list of events to let the server know you're not listening, 
*        they should stop being sent from the server if no other clients are using them,
*        but will at least stop being triggered in your app.
*    Required arguments:
*        eventlist: target events to stop listening to
*        successCB: success callback, gets called with the eventlist
*                   that was successfully unsubscribed
*        errorCB: error callback, called with the eventlist that failed to unsubscribe
*
******************************************************************************/

function Vehicle(sCB, eCB, url, protocol)
{
    /* store a copy of Vehicle this for reference in callbacks */
    var self = this;

    this.iSuccessCB = sCB;
    this.iErrorCB = eCB;

    /* variables for call management, supports up to 100 simultaneously */
    this.methodIdx = 0;
    this.methodCalls = [];
    for(var i = 0; i < 100; i++)
    {
        this.methodCalls[i] = null;
    }

    /* number of connection retries to attempt if the socket closes */
    this.retries = 5;
    this.connected = false;

    /* timeout for method calls in milliseconds */
    this.timeouttime = 5000;

    /* default values for WebSocket */
    this.socketUrl = "ws://localhost:23000/vehicle";
    this.socketProtocol = "http-only";

    /* override the websocket address if parameters are given */
    if(url != undefined) this.socketUrl = url;
    if(protocol != undefined) this.socketProtocol = protocol;

    this.VehicleMethodCall = function(id, name, successCB, errorCB)
    {
        var me = this;
        this.successCB = successCB;
        this.errorCB = errorCB;
        this.transactionid = id;
        this.name = name;
        this.done = false;
        this.start = function()
        {
            me.timeout = setTimeout(function(){
                if(me.errorCB != undefined)
                {
                    me.errorCB("\""+me.name+"\" method timed out after "+self.timeouttime+"ms");
                }
                me.finish();
            }, self.timeouttime);
        }
        this.finish = function()
        {
            if(me.timeout != undefined)
            {
                clearTimeout(me.timeout);
            }
            me.done = true;
        }
    }

    function init() {
        if ("WebSocket" in window)
        {
            if(self.socketProtocol.length > 0)
            {
                self.socket = new WebSocket(self.socketUrl, self.socketProtocol);
            }
            else
            {
                self.socket = new WebSocket(self.socketUrl);
            }
            self.socket.onopen = function()
            {
                self.connected = true;
                self.iSuccessCB((self.retries < 5)?"(RECONNECTED)":"");
                self.retries = 5;
            };
            self.socket.onclose = function()
            {
                self.connected = false;
                self.iErrorCB("socket closed "+((self.retries > 0)?"retrying in 5 seconds ...":""));
                if(self.retries > 0)
                {
                    setTimeout(function(){
                        self.retries--;
                        init();
                    }, 5000);
                }
            };
            self.socket.onerror = function(e)
            {
                self.iErrorCB(e.data);
            };
            self.socket.onmessage = function (e) 
            {
                self.receive(e.data);
            };
        }
        else
        {
            console.log("This browser doesn't appear to support websockets!");
        }
    }
    init();
}

Vehicle.prototype.generateTransactionId = function()
{
    var i, val = [];
    for(i = 0; i < 8; i++)
    {
       var num = Math.floor((Math.random()+1)*65536);
       val[i] = num.toString(16).substring(1);
    }
    var uuid = val[0]+val[1]+"-"+
               val[2]+"-"+val[3]+"-"+val[4]+"-"+
               val[5]+val[6]+val[7];
    return uuid;
}

Vehicle.prototype.send = function(obj, successCB, errorCB)
{
    if(!this.connected)
    {
        if(errorCB != undefined)
        {
            errorCB("\""+obj.name+"\" method failed because socket is closed");
        }
        return;
    }
    var i = this.methodIdx;
    this.methodIdx = (this.methodIdx + 1)%100;
    this.methodCalls[i] = new this.VehicleMethodCall(obj.transactionid, 
        obj.name, successCB, errorCB);
    this.socket.send(JSON.stringify(obj));
    this.methodCalls[i].start();
}


Vehicle.prototype.getSupportedEventTypes = function(type, writeable, successCB, errorCB)
{
    var obj = {
        "type" : "method",
        "name" : "getSupportedEventTypes",
        "writeable" : writeable,
        "transactionid" : this.generateTransactionId(),
        "data" : type
    };
    this.send(obj, successCB, errorCB);
}

Vehicle.prototype.get = function(namelist, successCB, errorCB)
{
    if(namelist.length <= 0)
    {
        return;
    }

    var obj = {
        "type" : "method",
        "name": "get",
        "transactionid" : this.generateTransactionId(),
        "data" : namelist
    };
    this.send(obj, successCB, errorCB);
}

Vehicle.prototype.getHistory = function(event, startTime, endTime, successCB, errorCB)
{
    var obj = {
        "type" : "method",
        "name": "getHistory",
        "transactionid" : this.generateTransactionId(),
        "data" : [event, (startTime.getTime()/1000).toString(), (endTime.getTime()/1000).toString()]
    };

    this.send(obj, successCB, errorCB);

}

Vehicle.prototype.set = function(namelist, valuelist, successCB, errorCB)
{
    if((namelist.length != valuelist.length)||(namelist.length <= 0))
    {
        return;
    }

    var obj = {
        "type" : "method",
        "name": "set",
        "transactionid" : this.generateTransactionId(),
        "data" : []
    };
    var list = [];
    for(var i = 0; i < namelist.length; i++)
    {
        var val = {"property" : namelist[i], "value" : valuelist[i]};
        list[list.length] = val;
    }
    obj.data = list;
    this.send(obj, successCB, errorCB);
}

Vehicle.prototype.subscribe = function(namelist, successCB, errorCB)
{
    var obj = {
        "type" : "method",
        "name": "subscribe",
        "transactionid" : this.generateTransactionId(),
        "data" : namelist
    };
    this.send(obj, successCB, errorCB);
}

Vehicle.prototype.unsubscribe = function(namelist, successCB, errorCB)
{
    var obj = {
        "type" : "method",
        "name": "unsubscribe",
        "transactionid" : this.generateTransactionId(),
        "data" : namelist
    };
    this.send(obj, successCB, errorCB);
}

Vehicle.prototype.sendEvent = function(name, value)
{
    var evt = document.createEvent("Event");
    evt.initEvent(name, true, true);
    evt.name = name;
    evt.value = value;
    document.dispatchEvent(evt);
    console.log(evt);
}

Vehicle.prototype.receive = function(msg)
{
    var self = this;
    var event;
    try {
        event = JSON.parse(msg);
    }
    catch(e) {
        self.iErrorCB("GARBAGE MESSAGE: "+msg);
        return;
    }

    if((event === undefined)||(event.type === undefined)||
       (event.name === undefined))
    {
        self.iErrorCB("BADLY FORMED MESSAGE: "+msg);
        return;
    }
    else
    {
        if(event.type === "methodReply")
        {
            var calls = this.methodCalls;
            for(var i = 0; i < calls.length; i++)
            {
                var call = calls[i];
                if(call&&(!call.done)&&(call.transactionid === event.transactionid))
                {
                    call.finish();
                    if(event.error != undefined)
                    {
                        call.errorCB(event.error);
                    }
                    if(event.data != undefined)
                    {
                        call.successCB(event.data);
                    }
                    return;
                }
            }
        }
        else if(event.type === "valuechanged")
        {
            self.sendEvent(event.name, event.data);
        }
    }
}
