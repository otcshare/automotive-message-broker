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
    logElement: null,
    init: function(log_id) {
        this.logElement = document.getElementById(log_id);
    },

    scrollToBottom: function() {
        this.logElement.scrollTop = this.logElement.scrollHeight;
    },

    clear: function() {
        this.logElement.innerHTML = "";
    },

    pass: function(msg) {
        this.logElement.innerHTML += "<div class='PassClass'>PASS: " + msg + "</div>";
        this.scrollToBottom();
    },

    fail: function(msg) {
        this.logElement.innerHTML += "<div class='FailClass'>FAIL: " + msg + "</div>";
        this.scrollToBottom();
    },

    log: function(msg) {
        this.logElement.innerHTML += "<div class='LogClass'> " + msg + "</div>";
        this.scrollToBottom();
    },
}

/* ----------------------------- test code --------------------------------- */

var vehicleEventType = new VehicleEventType();
var selected = [];

function getTypes(event) {
    var types = window.vehicle.getSupportedEventTypes(event, false,
        function(data) {
            if (data && data.length > 1) {
                PRINT.pass(event + " is a set of " + data.length + " events:");
                for (i in data) {
                    PRINT.log(data[i]);
                }
            } else if (data && data.length > 0) {
                PRINT.pass(event + " is a single event:");
                for (i in data) {
                    PRINT.log(data[i]);
                }
            } else {
                PRINT.fail(event + " unexcepted empty data field");
            }
        },
        function(msg) {
            PRINT.fail(((event === "") ? "all events" : event) + ":<br>" + msg);
        }
    );
}

function updateInput(input, value) {
    input.value = value;
    input.className = "change";
    input.addEventListener('webkitTransitionEnd', function callback(e) {
        e.target.removeEventListener('webkitTransitionEnd', callback, false);
        e.target.className = "";
    }, false);
}

function getValue(event) {
    var zone =  getZone(event);
    var types = window.vehicle.get(event, zone,
        function(data) {
            if (data) {
                PRINT.pass("values received:");
                PRINT.log(data.property + ": " + data.value + ", zone: " + data.zone);

                    var elements = document.getElementsByClassName('proptest');
                    for (var i = 0; i < elements.length; i++) {
                        var propinfo = elements[i].getElementsByClassName('propinfo')[0];
                        var name = propinfo.innerHTML;
                        var zone = elements[i].getElementsByTagName('input')[1];
                        updateInput(zone, zone.value);
                        var input = elements[i].getElementsByTagName('input')[0];
                        updateInput(input, data.value);
                    }
            } else {
                PRINT.fail("no values retrieved for " + eventlist);
            }
        },
        function(msg) {
            PRINT.fail(msg);
        }
    );
}

function setValue(eventlist) {
    var zoneList =  getZone(eventlist);
    var elements = document.getElementsByClassName('proptest');
    var i, valuelist = [] ;

    /* initialize the value list */
    for (i = 0; i < eventlist.length; i++) {
        valuelist[i] = 0;
    }

    for (var i = 0; i < elements.length; i++) {
        var propinfo = elements[i].getElementsByClassName('propinfo')[0];
        var name = propinfo.innerHTML;
        var idx = eventlist.indexOf(name);
        if (idx >= 0) {

            var input = elements[i].getElementsByTagName('input')[0];
            valuelist[idx] = input.value;
        }
    }

    var types = window.vehicle.set(eventlist, valuelist, zoneList, 
        function(msg) {
            PRINT.pass("Set success for: " + JSON.stringify(msg));
        },
        function(msg) {
            PRINT.fail("Set error: " + JSON.stringify(msg));
        }
    );
}

function eventListener(e) {
	PRINT.log(e.name + " update: " + JSON.stringify(e.value));
    var elements = document.getElementsByClassName('proptest');
    for (var i = 0; i < elements.length; i++) {
        var propinfo = elements[i].getElementsByClassName('propinfo')[0];
        var name = propinfo.innerHTML;
        if (name === e.name) {
            var input = elements[i].getElementsByTagName('input')[0];
            updateInput(input, e.value.value);
            var zone = elements[i].getElementsByTagName('input')[1];
            updateInput(zone, e.value.zone);
        }
    }
}

function subscribe(event) {
   var zoneList =  getZone(event);
    window.vehicle.subscribe(event, zoneList,
        function(data) {
            PRINT.pass("Subscribe success for: " + data);
            document.addEventListener(data, eventListener, false);
        },
        function(msg) {
            PRINT.fail("Subscribe failed for: " + msg);
        }
    );
}

function unsubscribe(event, zone) {
    zone = getZone(event);
    /* kill the handers first, so even if the service fails to acknowledge */
    /* we've stopped listening */
    document.removeEventListener(event, eventListener, false);

    window.vehicle.unsubscribe(event, zone,
        function(data) {
            PRINT.pass("Unsubscribe success for: " + data);
        },
        function(msg) {
            PRINT.fail("Unsubscribe failed for: " + msg);
        }
    );
}

function getZone(eventlist) {

        var elements = document.getElementsByClassName('proptest');
        for (var i = 0; i < elements.length; i++) {
            var propinfo = elements[i].getElementsByClassName('propinfo')[0];
            if (event == propinfo.innerHTML) {
                var zone = elements[i].getElementsByTagName('input')[1];
                return zone.value;
            }
        }
}

function select(elem) {
    var name = elem.innerHTML;
    if (!vehicleEventType.isValid(name))
        return;

    var idx = selected.indexOf(name);
    if (elem.className === "propinfo") {
        if (idx < 0) {
            selected[selected.length] = name;
        }
        elem.className = "propinfo select";
    } else if (elem.className === "propinfo select") {
        if (idx >= 0) {
            selected.splice(idx, 1);
        }
        elem.className = "propinfo";
    }
}

function start(msg) {
    if (window.vehicle && window.vehicle.getSupportedEventTypes) {
        PRINT.pass("vehicle interface online " + msg);
    } else {
        PRINT.fail("vehicle interface not found");
        return;
    }

    var tester = document.getElementById("tester");
    var part = ['<div class="proptest"><div class="propinfo" onclick=select(this)>',
        '</div><div class="buttons"><div class="testbutton types" onclick=getTypes("',
        '")></div><div id="',
        '_subscribe" class="testbutton subscribe" onclick=subscribe("',
        '")></div><div id="',
        '_unsubscribe" class="testbutton unsubscribe" onclick=unsubscribe("',
        '")></div><div class="testbutton get" onclick=getValue("',
        '")></div><div class="testbutton set" onclick=setValue("',
        '")></div><input class = "Textvalue" type="text" value="0" placeholder="Value" /><div class = "smallText"> Zone: </div><input class = "zone" type="text" value="0" placeholder="Zone"/></div></div>'
    ];
    var events = vehicleEventType.event;

    /* apply on all selected events */
    var html = '<div class="proptest"><div class="propinfo unselectable">apply on all selected events' +
        '</div><div class="buttons">' +
        '<div class="testbutton subscribe" onclick=subscribe(selected)></div>' +
        '<div class="testbutton unsubscribe" onclick=unsubscribe(selected)></div>' +
        '<div class="testbutton get" onclick=getValue(selected)></div>' +
        '<div class="testbutton set" onclick=setValue(selected)></div></div></div>';

    /* all events */
    html += '<div class="proptest"><div class="propinfo unselectable">all events';
    html +=     '</div><div class="buttons"><div class="testbutton types" onclick=getTypes("' ;
    html +=     '")></div><div class="testbutton subscribe" onclick=\"subscribe([\'' ;
    html +=     events.join("','") ;
    html +=     '\'])\"></div><div class="testbutton unsubscribe" onclick= \"unsubscribe([\'' ;
    html +=     events.join("','");
    html +=     '\'])\"></div><div class="testbutton get" onclick=\"getValue([\'' ;
    html +=     events.join("','");
    html +=     '\'])\"></div></div></div>';

    /* events */
    for (var i = 0; i < events.length; i++) {
        var piece = "";
        for (var j = 0; j < part.length - 1; j++) {
            piece += part[j] + events[i];
        }
        html += piece + part[j];
    }
    tester.innerHTML = html;
}

function error(msg) {
    PRINT.fail(msg);
}

function init(url, protocol) {
    PRINT.init("result");
    window.vehicle = new Vehicle(start, error, url, protocol);
}
