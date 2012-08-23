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
*  Function name: get(type, successCB, errorCB)
*    Description:
*        Retrieves a list of event/value pairs for a target event or event group
*    Required arguments:
*        type: target event group to query (use empty string for all events)
*        successCB: success callback, gets called with the event/value pair list
*                   for all event children of the target. The list is the in the 
*                   form of data[n].name/data[n].value
*        errorCB: error callback, called with error message string
*
*  Function name: set(type, value, successCB, errorCB)
*    Description:
*        Sets a single event's value (triggers error if it's read-only)
*    Required arguments:
*        type: target event to set (an event group will trigger an error)
*        successCB: success callback, gets called with the event/value pair
*                   that was successfully set in the form data.name/data.value
*        errorCB: error callback, called with error message string
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

Vehicle.prototype.get = function(type, successCB, errorCB)
{
    var obj = {
        "type" : "method",
        "name": "get",
        "transactionid" : this.generateTransactionId(),
        "data" : type
    };
    this.send(obj, successCB, errorCB);
}

Vehicle.prototype.set = function(type, value, successCB, errorCB)
{
    var obj = {
        "type" : "method",
        "name": "set",
        "transactionid" : this.generateTransactionId(),
        "data" : {"property" : type, "value" : value}
    };
    this.send(obj, successCB, errorCB);
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

    if((event == undefined)||(event.type == undefined)||
       (event.name == undefined)||(event.transactionid == undefined))
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
                    else
                    {
                        call.successCB(event.data);
                    }
                    return;
                }
            }
        }
    }
}
