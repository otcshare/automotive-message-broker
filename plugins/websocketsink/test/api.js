/*
 * Copyright (c) 2012, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

/* ------------------------ vehicle interface ----------------------------- */

function Vehicle(socketUrl, sCB, eCB, calltimeout)
{
    var self = this;
    this.iSuccessCB = sCB;
    this.iErrorCB = eCB;
    this.retries = 5;
    this.connected = false;
    this.transactionid = 0;
    this.methodCalls = [];
    this.methodIdx = 0;
    this.timeouttime = (calltimeout == undefined)?5000:calltimeout;

    for(var i = 0; i < 100; i++)
    {
        this.methodCalls[i] = null;
    }

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
            self.socket = new WebSocket(socketUrl);
            self.socket.onopen = function()
            {
                self.connected = true;
                this.send("client");
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

Vehicle.prototype.send = function(obj, successCB, errorCB)
{
    obj.transactionid = this.transactionid++;
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
        "transactionid" : 0,
        "data" : type
    };
    this.send(obj, successCB, errorCB);
}

Vehicle.prototype.get = function(type, successCB, errorCB)
{
    var obj = {
        "type" : "method",
        "name": "get",
        "transactionid" : 0,
        "data" : type
    };
    this.send(obj, successCB, errorCB);
}

Vehicle.prototype.set = function(type, value, successCB, errorCB)
{
    var obj = {
        "type" : "method",
        "name": "set",
        "transactionid" : 0,
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
