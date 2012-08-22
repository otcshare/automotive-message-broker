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
 
    function init() {
        if ("WebSocket" in window)
        {
            self.socket = new WebSocket(socketUrl);
            self.socket.onopen = function()
            {
                PRINT.pass("Server READY");
                this.send("server");
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
        obj = {
            "type" : "methodReply",
            "name": event.name,
            "transactionid" : event.transactionid,
            "data" : this.vehicleEventType.getSupportedEventList(event.data)
        };
    }
    else if(event.name === "get")
    {
        var names = this.vehicleEventType.getValueEventList(event.data);
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
    var server = new VehicleServer("ws://localhost:23000/vehicle");
});
