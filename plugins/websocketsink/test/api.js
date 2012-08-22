/*
 * Copyright (c) 2012, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

/* ------------------------ vehicle interface ----------------------------- */

function Vehicle(socketUrl, sCB, eCB, tout)
{
    var self = this;
    this.successCB = sCB;
    this.errorCB = eCB;
    this.transactionid = 0;
    this.methodCalls = [];
    this.timeouttime = (tout == undefined)?2000:tout;
    this.retries = 5;
    this.connected = false;

    this.VehicleMethodCall = function(n, sCB, eCB)
    {
        var me = this;
        this.name = n;
        this.successCB = sCB;
        this.errorCB = eCB;
        this.transactionid = self.transactionid++;

        this.send = function(msg)
        {
            var errCB = (me.errorCB == undefined)?self.errorCB:me.errorCB;
            if(!self.connected)
            {
                if(errCB != undefined)
                {
                    errCB("\""+me.name+"\" method failed because socket is closed");
                }
                self.methodCalls.pop(this);
            }
            me.timeout = setTimeout(function(){
                if(errCB != undefined)
                {
                    errCB("\""+me.name+"\" method timed out after "+self.timeouttime+"ms");
                }
                self.methodCalls.pop(this);
            }, self.timeouttime);
            self.socket.send(msg);
        }
        this.done = function()
        {
            if(me.timeout != undefined)
            {
                clearTimeout(me.timeout);
            }
        }
    }

    function init() {
        if ("WebSocket" in window)
        {
            self.socket = new WebSocket(socketUrl);
            self.socket.onopen = function()
            {
                self.connected = true;
                this.send("client");
                self.successCB((self.retries < 5)?"(RECONNECTED)":"");
                self.retries = 5;
            };
            self.socket.onclose = function()
            {
                self.connected = false;
                self.errorCB("socket closed "+((self.retries > 0)?"retrying in 5 seconds ...":""));
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
                self.errorCB(e.data);
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

Vehicle.prototype.getSupportedEventTypes = function(type, writeable, successCB, errorCB)
{
    var call = new this.VehicleMethodCall("getSupportedEventTypes", successCB, errorCB);
    this.methodCalls.push(call);
    var obj = {
        "type" : "method",
        "name": "getSupportedEventTypes",
        "transactionid" : call.transactionid,
        "data" : type
    };
    call.send(JSON.stringify(obj));
}

Vehicle.prototype.get = function(type, successCB, errorCB)
{
    var call = new this.VehicleMethodCall("get", successCB, errorCB);
    this.methodCalls.push(call);
    var obj = {
        "type" : "method",
        "name": "get",
        "transactionid" : call.transactionid,
        "data" : type
    };
    call.send(JSON.stringify(obj));
}

Vehicle.prototype.receive = function(msg)
{
    var self = this;
    var event;
    try {
        event = JSON.parse(msg);
    }
    catch(e) {
        self.errCB("GARBAGE MESSAGE: "+msg);
        return;
    }

    if((event == undefined)||(event.type == undefined)||
       (event.name == undefined)||(event.transactionid == undefined))
    {
        self.errCB("BADLY FORMED MESSAGE: "+msg);
        return;
    }
    else
    {
        if(event.type === "methodReply")
        {
            var calls = this.methodCalls;
            for(i in calls)
            {
                var call = calls[i];
                if((call.transactionid == event.transactionid)&&
                   (call.name === event.name))
                {
                    call.done();
                    if(event.error != undefined)
                    {
                        call.errorCB(event.error);
                    }
                    else
                    {
                        call.successCB(event.data);
                    }
                    this.methodCalls.pop(call);
                    return;
                }
            }
        }
    }
}
