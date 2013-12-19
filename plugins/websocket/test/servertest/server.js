/*
 * Copyright (c) 2012, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

/* --------------------------- utility code ------------------------------- */

var PRINT = {
    logElement : null,
    init : function(log_id) {
        this.logElement = document.getElementById(log_id);
    },

    scrollToBottom : function() {
        this.logElement.scrollTop = this.logElement.scrollHeight;
    },

    incoming : function(msg) {
        this.logElement.innerHTML += "<div style='color: blue'> REQUEST: " + msg + "</div>";
        this.scrollToBottom();
    },

    outgoing : function(msg) {
        this.logElement.innerHTML += "<div style='color: purple'> RESPONSE: " + msg + "</div>";
        this.scrollToBottom();
    },

    pass : function(msg) {
        this.logElement.innerHTML += "<div style='color: green'> SUCCESS: " + msg + "</div>";
        this.scrollToBottom();
    },

    fail : function(msg) {
        this.logElement.innerHTML += "<div style='color: red'> FAIL: " + msg + "</div>";
        this.scrollToBottom();
    },

    log : function(msg) {
        this.logElement.innerHTML += "<div class='LogClass'> " + msg + "</div>";
        this.scrollToBottom();
    },
}

/* ----------------------------- test code --------------------------------- */

function VehicleServer(socketUrl)
{
    var self = this;
    this.vehicleEventType = new VehicleEventType();
    this.subscriptions = [];

    this.Signal = function(name)
    {
        var me = this;
        this.users = 0;
        this.name = name;
        this.start = function() {
            if(me.users <= 0)
            {
                var interval = Math.floor(Math.random()*5000) + 1000;
                me.timer = setInterval(function() {
                    var value = parseInt(self.vehicleEventType.getValue(me.name)) + 1;
                    self.vehicleEventType.setValue(me.name, value);
                    var obj = {
                        "type" : "valuechanged",
                        "name": me.name,
                        "data" : value
                    };
                    self.socket.send(JSON.stringify(obj));
                }, interval);
            }
            me.users = 1;
        }
        this.stop = function() {
            me.users--;
            if((me.users <= 0)&&(me.timer != undefined))
            {
                clearInterval(me.timer);
            }
        }
    }

    function init() {
        if ("WebSocket" in window)
        {
            var list = self.vehicleEventType.getValueEventList();
            for(var i = 0; i < list.length; i++)
            {
                self.subscriptions[i] = new self.Signal(list[i]);
            }

            self.socket = new WebSocket(socketUrl);
            self.socket.onopen = function()
            {
                PRINT.pass("Server READY");
            };
            self.socket.onclose = function()
            {
                PRINT.fail("Server CLOSED");
            };
            self.socket.onerror = function(e)
            {
                PRINT.fail("Server ERROR: "+e.data);
            };
            self.socket.onmessage = function (e) 
            {
                self.receive(e.data);
            };
        }
        else
        {
            PRINT.fail("This browser doesn't appear to support websockets!");
        }
    }
    init();
}

VehicleServer.prototype.subscribe = function(list)
{
    for(var i = 0; i < this.subscriptions.length; i++)
    {
        if(list.indexOf(this.subscriptions[i].name) >= 0)
        {
            this.subscriptions[i].start();
        }
    }
}

VehicleServer.prototype.unsubscribe = function(list)
{
    for(var i = 0; i < this.subscriptions.length; i++)
    {
        if(list.indexOf(this.subscriptions[i].name) >= 0)
        {
            this.subscriptions[i].stop();
        }
    }
}

VehicleServer.prototype.receive = function(msg)
{
    var event = JSON.parse(msg);
    /* accept only methods with transaction ids */
    if((event == undefined)||(event.transactionid == undefined)||
       (event.type != "method"))
    {
        return;
    }

    var obj;
    PRINT.incoming(msg);
    if(event.name === "getSupportedEventTypes")
    {
        var data;
        if(event.writeable)
        {
            data = this.vehicleEventType.getValueEventList(event.data);
        }
        else
        {
            data = this.vehicleEventType.getSupportedEventList(event.data);
        }
        obj = {
            "type" : "methodReply",
            "name": event.name,
            "transactionid" : event.transactionid,
            "data" : data
        };
    }
    else if(event.name === "get")
    {
        var names = this.vehicleEventType.getValuesEventList(event.data);
        if(names.length > 0)
        {
            obj = {
                "type" : "methodReply",
                "name": event.name,
                "transactionid" : event.transactionid,
                "data" : []
            };
            for(i in names)
            {
                var value = this.vehicleEventType.getValue(names[i]);
                obj.data.push({"name" : names[i], "value" : value});
            }
        }
        else
        {
            obj = {
                "type" : "methodReply",
                "name": event.name,
                "transactionid" : event.transactionid,
                "error" : event.data + " is not a valid event"
            };
        }
    }
    else if(event.name === "set")
    {
        var bad = [];
        var good = [];
        for(var i = 0; i < event.data.length; i++)
        {
            if((event.data[i].value != undefined) && 
               this.vehicleEventType.isValueEvent(event.data[i].property))
            {
                this.vehicleEventType.setValue(event.data[i].property, parseInt(event.data[i].value));
                good[good.length] = event.data[i].property;
            }
            else
            {
                bad[bad.length] = event.data[i].property;
            }
        }

        obj = {
            "type" : "methodReply",
            "name": event.name,
            "transactionid" : event.transactionid
        };

        if(bad.length > 0)
        {
            obj.error = "Failed to set:";
            for(var i = 0; i < bad.length; i++)
            {
                obj.error += " "+bad[i];
            }
        }

        if(good.length > 0)
        {
            obj.data = "Successfully set:";
            for(var i = 0; i < good.length; i++)
            {
                obj.data += " "+good[i];
            }
        }
    }
    else if(event.name === "subscribe")
    {
        var names = this.vehicleEventType.getValuesEventList(event.data);
        if(names.length > 0)
        {
            obj = {
                "type" : "methodReply",
                "name": event.name,
                "transactionid" : event.transactionid,
                "data" : names
            };
            for(i in names)
            {
                this.subscribe(names[i]);
            }
        }
        else
        {
            obj = {
                "type" : "methodReply",
                "name": event.name,
                "transactionid" : event.transactionid,
                "error" : "no valid events provided"
            };
        }
    }
    else if(event.name === "unsubscribe")
    {
        var names = this.vehicleEventType.getValuesEventList(event.data);
        if(names.length > 0)
        {
            obj = {
                "type" : "methodReply",
                "name": event.name,
                "transactionid" : event.transactionid,
                "data" : names
            };
            for(i in names)
            {
                this.unsubscribe(names[i]);
            }
        }
        else
        {
            obj = {
                "type" : "methodReply",
                "name": event.name,
                "transactionid" : event.transactionid,
                "error" : "no valid events provided"
            };
        }
    }
    else
    {
        obj = {
            "type" : "methodReply",
            "name": event.name,
            "transactionid" : event.transactionid,
            "error" : event.name + " is not a valid method"
        };
    }
    PRINT.outgoing(JSON.stringify(obj));
    this.socket.send(JSON.stringify(obj));
}

window.addEventListener('load', function () {
    "use strict";
    PRINT.init("result");
    var server = new VehicleServer("ws://localhost:23023/vehicle?server");
});
