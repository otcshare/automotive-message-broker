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

    clear : function() {
        this.logElement.innerHTML = "";
    },

    pass : function(msg) {
        this.logElement.innerHTML += "<div class='PassClass'>PASS: " + msg + "</div>";
    },

    fail : function(msg) {
        this.logElement.innerHTML += "<div class='FailClass'>FAIL: " + msg + "</div>";
        this.scrollToBottom();
    },

    log : function(msg) {
        this.logElement.innerHTML += "<div class='LogClass'> " + msg + "</div>";
    },
}

/* ----------------------------- test code --------------------------------- */

function getTypes(event)
{
    var types = window.vehicle.getSupportedEventTypes(event, false,
        function(data) {
            PRINT.clear();
            if(data && data.length > 1)
            {
                PRINT.pass(event+" is a set of "+data.length+" events:");
                for(i in data)
                {
                    PRINT.log(data[i]);
                }
            }
            else if(data && data.length > 0)
            {
                PRINT.pass(event+" is a single event:");
                for(i in data)
                {
                    PRINT.log(data[i]);
                }
            }
            else
            {
                PRINT.fail(event+" unexcepted empty data field");
            }
        },
        function(msg) {
            PRINT.fail(((event === "")?"all events":event)+":<br>"+msg);
        }
    );
}

function getValue(event)
{
    var types = window.vehicle.get(event,
        function(data) {
            PRINT.clear();
            if(data && data.length > 0)
            {
                PRINT.pass(event+" values received");
                for(i in data)
                {
                    PRINT.log(data[i].name+": "+data[i].value);
                }
            }
            else
            {
                PRINT.fail("no values retrieved for "+event);
            }
        },
        function(msg) {
            PRINT.fail(((event === "")?"all events":event)+":<br>"+msg);
        }
    );
}

function start(msg)
{
    if(window.vehicle && window.vehicle.getSupportedEventTypes)
    {
        PRINT.pass("vehicle interface online "+msg);
    }
    else
    {
        PRINT.fail("vehicle interface not found");
        return;
    }

    var vehicleEventType = new VehicleEventType();
    var tester = document.getElementById("tester");
    var part1 = '<div class="proptest">';
    var part2 = '<div class="buttons"><div class="testbutton types" onclick=getTypes("';
    var part3 = '")></div><div class="testbutton get" onclick=getValue("'
    var part4 = '")></div></div></div>';
    var events = vehicleEventType.event;
    var html = part1 + "all events" + part2 + part3 + part4;
    for(i in events)
    {
        html += part1 + events[i] + part2 + events[i] + 
                part3 + events[i] + part4;
    }
    tester.innerHTML = html;
}

function error(msg)
{
    PRINT.fail(msg);
}

function init() {
    PRINT.init("result");
    window.vehicle = new Vehicle("ws://localhost:23000/vehicle",
        start, error);
}
