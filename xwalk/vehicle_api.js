// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

var next_async_call_id = 0;
var async_calls = {};

var subscriptions = [];

function vehicleInterfaceCommonContructor(obj, attname) {
  obj.attributeName = attname;

  var msg = {};
  msg['method'] = 'zones';
  msg['name'] = obj.attributeName;

  obj._zones = new Zone;

  var call = new AsyncCall(function(data) {
    obj._zones = data;
  });

  async_calls[next_async_call_id] = call;
  msg.asyncCallId = next_async_call_id;
  ++next_async_call_id;

  extension.postMessage(JSON.stringify(msg));

  Object.defineProperty(obj, 'zones', { get: function() { return obj._zones } });
}

function VehicleInterface(attname) {
  vehicleInterfaceCommonContructor(this, attname);
}

VehicleInterface.prototype.get = function(zone) {
  var msg = {};
  msg['method'] = 'get';
  msg['name'] = this.attributeName;
  msg['zone'] = zone;

  return createPromise(msg);
};

function VehicleSignalInterface(attname) {
  vehicleInterfaceCommonContructor(this, attname);
}

VehicleSignalInterface.prototype = VehicleInterface.prototype;

VehicleSignalInterface.prototype.subscribe = function(callback, zone) {
  if (!zone) zone = new Zone();

  var msg = {};
  msg['method'] = 'subscribe';
  msg['name'] = this.attributeName;
  msg['zone'] = zone;

  extension.postMessage(JSON.stringify(msg));

  msg['callback'] = callback;

  subscriptions.push(msg);

  return subscriptions.length - 1;
};

VehicleSignalInterface.prototype.unsubscribe = function(handle) {
  var obj = subscriptions[handle];
  subscriptions.splice(handle, 1);

  var unsubscribe = true;

  for (var i = 0; i < subscriptions.length; i++) {
    var testObj = subscriptions[i];

    if (testObj.name === obj.name && testObj.zone.equals(obj.zone)) {
      unsubscribe = false;
      break;
    }
  }

  if (unsubscribe) {
    var msg = {};
    msg['method'] = 'unsubscribe';
    msg['name'] = this.attributeName;
    msg['zone'] = obj.zone;

    extension.postMessage(JSON.stringify(msg));
  }
};

function AsyncCall(resolve, reject) {
  this.resolve = resolve;
  this.reject = reject;
}

function createPromise(msg) {
  var promise = new Promise(function(resolve, reject) {
    async_calls[next_async_call_id] = new AsyncCall(resolve, reject);
  });

  msg.asyncCallId = next_async_call_id;
  extension.postMessage(JSON.stringify(msg));
  ++next_async_call_id;
  return promise;
}

window.Zone = function(zone) {
  this.value = zone ? zone : [];

  Object.defineProperty(this, 'driver',
                        { enumerable: false, get: function() {
                          return new Zone(['Front', 'Left']);
                        } });
};

window.Zone.prototype.equals = function(zone) {
  var is_equal = true;

  for (var i = 0; i < zone.value.length; i++) {
    is_equal &= this.value.indexOf(zone.value[i]) !== -1;
  }

  for (var i = 0; i < this.value.length; i++) {
    is_equal &= zone.value.indexOf(this.value[i]) !== -1;
  }

  return is_equal;
};

function _defineVehicleProperty(obj, prop) {
  Object.defineProperty(obj, prop, { enumerable: true, value: new VehicleInterface(prop) });
}

function _defineVehicleSignalProperty(obj, prop) {
  Object.defineProperty(obj, prop, { enumerable: true, value: new VehicleSignalInterface(prop) });
}

extension.setMessageListener(function(json) {
  try {
    var msg = JSON.parse(json);

    switch (msg.method) {
      case 'get':
        handleGetReply(msg);
        break;
      case 'zones':
        handleZonesReply(msg);
        break;
      case 'subscribe':
        handleSubscribeReply(msg);
        break;
      default:
        break;
    }
  } catch (error) {
    console.log('Error in message listener: ' + error);
  }
});

function handleGetReply(msg) {
  var cbobj = async_calls[msg.asyncCallId];

  if (msg.error) {
    var error = {};
    error.error = msg.value;
    switch (msg.value) {
      case 'permission_denied':
        error.message = 'Permission Denied';
        break;
      case 'invalid_operation':
        error.message = 'Invalid Operation';
        break;
      case 'timeout':
        error.message = 'Operation timed out';
        break;
      case 'invalid_zone':
        error.message = 'Zone invalid or not found';
        break;
      case 'unknown':
        error.message = 'An unknown error occured';
        break;
      default:
        break;
    }

    cbobj.reject(error);
  }
  else {
    cbobj.resolve(msg.value);
  }

  delete async_calls[msg.asyncCallId];
}

function handleZonesReply(msg) {
  var cbobj = async_calls[msg.asyncCallId];

  if (cbobj)
    cbobj.resolve(new Zone(msg.value));
}

function handleSubscribeReply(msg) {
  delete async_calls[msg.asyncCallId];
  var value = msg.value;
  value.zone = new Zone(value.zone);

  for (var i = 0; i < subscriptions.length; i++) {
    var itr = subscriptions[i];
    var ifaceIs = (value.interfaceName.toLowerCase() === itr.name.toLowerCase());
    if (ifaceIs === true && value.zone.equals(itr.zone)) {
      itr.callback(value);
    }
  }
}

_defineVehicleSignalProperty(exports, 'vehicleSpeed');
_defineVehicleSignalProperty(exports, 'engineSpeed');
_defineVehicleSignalProperty(exports, 'transmission');
_defineVehicleSignalProperty(exports, 'steeringWheel');
_defineVehicleSignalProperty(exports, 'throttlePosition');
_defineVehicleSignalProperty(exports, 'engineCoolant');
_defineVehicleSignalProperty(exports, 'nightMode');
_defineVehicleSignalProperty(exports, 'drivingMode');
_defineVehicleSignalProperty(exports, 'brakeOperation');
_defineVehicleSignalProperty(exports, 'tire');
_defineVehicleSignalProperty(exports, 'door');
_defineVehicleSignalProperty(exports, 'defrost');
_defineVehicleSignalProperty(exports, 'climateControl');
_defineVehicleSignalProperty(exports, 'lightStatus');
_defineVehicleSignalProperty(exports, 'batteryStatus');
_defineVehicleSignalProperty(exports, 'fuel');
_defineVehicleSignalProperty(exports, 'temperature');
_defineVehicleSignalProperty(exports, 'odometer');
