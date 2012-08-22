/*
 * Copyright (c) 2012, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

var vehicle;
var PRINT = {
    moduleName : "IVI API",
    logElement : null,
    init : function(log_id) {
        this.logElement = document.getElementById(log_id);
    },

    setModule : function(moduleName) {
        this.moduleName = moduleName;
    },

    pass : function(msg) {
        this.logElement.innerHTML += "<div class='PassClass'> " + this.moduleName + " : Pass : " + msg + "</div>";
    },

    fail : function(msg) {
        this.logElement.innerHTML += "<div class='FailClass'> " + this.moduleName + " : Fail : " + msg + "</div>";
    },

    log : function(msg) {
        this.logElement.innerHTML += "<div class='LogClass'> " + msg + "</div>";
    },
}

var myJSONObject = {
    "type" : "method",
    "name": "GetProperty",
    "Arguments": [
         "Velocity"
    ],
    "transactionid": "0f234002-95b8-48ac-aa06-cb49e372cc1c"
};

function Vehicle()
{
    var self = this;
 
    function init() {
       if ("WebSocket" in window)
        {
            PRINT.pass("The browser is websocket capable");
	    
            this.socket = new WebSocket("ws://localhost:7681","http-only");
            this.socket.onopen = function()
            {
                PRINT.pass("Connection OPEN");
                this.send(JSON.stringify(myJSONObject));
            };
            this.socket.onmessage = function (e) 
            {
                self.receive(e.data);
            };
            this.socket.onclose = function(e)
            {
                PRINT.fail("Connection CLOSED: " + e.reason + " code: " + e.code);
            };
            this.socket.onerror = function(evt) {
		alert(evt);
            }
        }
        else
        {
            PRINT.fail("This browser doesn't ppear to support websockets!");
        }
    }
    init();
}

Vehicle.prototype.send = function(msg)
{

}

Vehicle.prototype.receive = function(msg)
{
    PRINT.log("Message Received: "+msg);
    var data = JSON.parse(msg);
    console.log(data);
}

function init() {
    PRINT.init("result");
    vehicle = new Vehicle();
}
