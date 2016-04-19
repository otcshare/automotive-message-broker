// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

var next_async_call_id = 0;
var async_calls = {};

var subscriptions = [];

function makeCall(call, msg) {
	async_calls[next_async_call_id] = call;
	msg.asyncCallId = next_async_call_id;
	++next_async_call_id;

	extension.postMessage(JSON.stringify(msg));
}

function vehicleInterfaceCommonContructor(obj, attname) {
	obj.attributeName = attname;

	var msg = {};
	msg['method'] = 'zones';
	msg['name'] = obj.attributeName;

	obj._zones = new Zone;
	obj._supported = false;

	var call = new AsyncCall(function(data) {
		obj._zones = data;
	});

	makeCall(call, msg);

	var supportedMessage = {};
	supportedMessage['method'] = 'supported';
	supportedMessage['name'] = obj.attributeName;

	var supportedCall = new AsyncCall(function(data) {
		obj._supported = data;
	});

	makeCall(supportedCall, supportedMessage);

	Object.defineProperty(obj, 'zones', { get: function() { return obj._zones } });
	Object.defineProperty(obj, 'supported', { get: function() { return obj._supported; } });
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

VehicleInterface.prototype.availableForRetrieval = function(attName) {
	return isAvailable(this, attName);
}

VehicleInterface.prototype.availabilityChangedListener = function(callback) {
	if(this.changedListenerCount) {
		this.changedListenerCount++;
	}
	else {
		this.changedListenerCount = 0;
	}
	return this.changedListenerCount;
}

VehicleInterface.prototype.removeAvailabilityChangedListener = function(handle) {

}

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

VehicleSignalInterface.prototype.set = function (value, zone) {
	var msg = {};
	msg['method'] = 'set';
	msg['name'] = this.attributeName;
	msg['zone'] = zone;
	msg['value'] = value;

	return createPromise(msg);
}

VehicleSignalInterface.prototype.availableForSubscription = function(attName) {
	return isAvailable(this, attName);
}

VehicleSignalInterface.prototype.availableForSetting = function(attName) {
	return isAvailable(this, attName);
}

function isAvailable(obj, attName)
{
	var msg = {};
	msg["method"] = 'availableForRetrieval';
	msg["name"] = obj.attributeName;
	msg["attName"] = attName;

	var reply = extension.internal.sendSyncMessage(JSON.stringify(msg));

	if (reply === "true") {
		return "available";
	} else {
		return "not_supported";
	}
}

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
		is_equal = is_equal && this.value.indexOf(zone.value[i]) !== -1;
	}

	for (var i = 0; i < this.value.length; i++) {
		is_equal = is_equal && zone.value.indexOf(this.value[i]) !== -1;
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
			handlePromiseReply(msg);
			break;
		case 'zones':
			handleZonesReply(msg);
			break;
		case 'subscribe':
			handleSubscribeReply(msg);
			break;
		case 'set':
			handlePromiseReply(msg);
			break;
		case 'supported':
			handleSupportedReply(msg)
			break;
		case 'vehicleSupportedAttributes':
			handleVehicleSupported(msg);
			break;
		default:
			break;
		}
	} catch (error) {
		console.log('Error in message listener: ' + error);
		console.log("msg: " + JSON.stringify(msg))
	}
});

function handlePromiseReply(msg) {
	var cbobj = async_calls[msg.asyncCallId];

	if (msg.error) {
		var error = {};
		error.error = msg.value;
		switch (msg.value) {
		case 'permission_denied':
			error.message = 'Permission denied';
			break;
		case 'invalid_operation':
			error.message = 'Invalid operation';
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
			error.message = 'Unknown';
			break;
		}

		cbobj.reject(error);
	} else {
		if (msg.value && msg.value.zone) {
			msg.value.zone = new Zone(msg.value.zone);
		}
		cbobj.resolve(msg.value);
	}

	delete async_calls[msg.asyncCallId];
}

function handleZonesReply(msg) {
	var cbobj = async_calls[msg.asyncCallId];

	if (cbobj)
		cbobj.resolve(new Zone(msg.value));
}

function handleSupportedReply(msg) {
	var cbobj = async_calls[msg.asyncCallId];

	if (cbobj)
		cbobj.resolve(msg.value);
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

function handleVehicleSupported(msg) {
	var value = msg.value;
	for(i in value) {
		if(exports[i] !== undefined) {
			_defineVehicleSignalProperty(exports, i);
		}
	}
}

/// Runningstatus attributes:
_defineVehicleSignalProperty(exports, 'vehicleSpeed');
_defineVehicleSignalProperty(exports, 'engineSpeed');
_defineVehicleSignalProperty(exports, 'powertrainTorque');
_defineVehicleSignalProperty(exports, 'acceleratorPedalPosition');
_defineVehicleSignalProperty(exports, 'throttlePosition');
_defineVehicleSignalProperty(exports, 'transmission');
_defineVehicleSignalProperty(exports, 'cruiseControlStatus');
_defineVehicleSignalProperty(exports, 'lightStatus');
_defineVehicleSignalProperty(exports, 'horn');
_defineVehicleSignalProperty(exports, 'chime');
_defineVehicleSignalProperty(exports, 'fuel');
_defineVehicleSignalProperty(exports, 'engineOil');
_defineVehicleSignalProperty(exports, 'acceleration');
_defineVehicleSignalProperty(exports, 'engineCoolant');
_defineVehicleSignalProperty(exports, 'steeringWheel');
_defineVehicleSignalProperty(exports, 'wheelTick');
_defineVehicleSignalProperty(exports, 'ignitionTime');
_defineVehicleSignalProperty(exports, 'yawRate');
_defineVehicleSignalProperty(exports, 'brakeOperation');
_defineVehicleSignalProperty(exports, 'buttonEvent');
_defineVehicleSignalProperty(exports, 'nightMode');
_defineVehicleSignalProperty(exports, 'drivingMode');

/// Maintenance attributes:
_defineVehicleSignalProperty(exports, 'odometer');
_defineVehicleSignalProperty(exports, 'transmissionOil');
_defineVehicleSignalProperty(exports, 'transmissionClutch');
_defineVehicleSignalProperty(exports, 'brakeMaintenance');
_defineVehicleSignalProperty(exports, 'washerFluid');
_defineVehicleSignalProperty(exports, 'malfunctionIndicator');
_defineVehicleSignalProperty(exports, 'batteryStatus');
_defineVehicleSignalProperty(exports, 'tire');
_defineVehicleSignalProperty(exports, 'diagnostic');

/// Personalization attributes:
_defineVehicleSignalProperty(exports, 'mirror');
_defineVehicleSignalProperty(exports, 'seatAdjustment');
_defineVehicleSignalProperty(exports, 'driveMode');
_defineVehicleSignalProperty(exports, 'dashboardIllumination');
_defineVehicleSignalProperty(exports, 'vehicleSound');

/// Driving Safety attributes:
_defineVehicleSignalProperty(exports, 'antilockBreakingSystem');
_defineVehicleSignalProperty(exports, 'tractionControlSystem');
_defineVehicleSignalProperty(exports, 'electronicStabilityControl');
_defineVehicleSignalProperty(exports, 'topSpeedLimit');
_defineVehicleSignalProperty(exports, 'airbagStatus');
_defineVehicleSignalProperty(exports, 'door');
_defineVehicleSignalProperty(exports, 'childSafetyLock');
_defineVehicleSignalProperty(exports, 'seat');

/// Climate attributes:
_defineVehicleSignalProperty(exports, 'temperature');
_defineVehicleSignalProperty(exports, 'rainSensor');
_defineVehicleSignalProperty(exports, 'wiperStatus');
_defineVehicleSignalProperty(exports, 'climateControl');
_defineVehicleSignalProperty(exports, 'defrost');
_defineVehicleSignalProperty(exports, 'sunroof');
_defineVehicleSignalProperty(exports, 'atmosphericPressure');

/// Vision and Parking Interfaces
_defineVehicleSignalProperty(exports, 'langeDepartureStatus');
_defineVehicleSignalProperty(exports, 'alarm');
_defineVehicleSignalProperty(exports, 'parkingBrake');


