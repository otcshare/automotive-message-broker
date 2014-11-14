// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "vehicle_instance.h"

#include <abstractpropertytype.h>

#include <algorithm>
#include <string>

#include "vehicle.h"

VehicleInstance::VehicleInstance(): vehicle_(new Vehicle(this)) {
	DebugOut::setDebugThreshhold(5);
}

void VehicleInstance::HandleMessage(const char* message) {
	DebugOut() << "VehicleInstance message received " << message << endl;

	picojson::value v;

	std::string err;
	picojson::parse(v, message, message + strlen(message), &err);
	if (!err.empty()) {
		return;
	}

	std::string method = v.get("method").to_str();

	Zone::Type amb_zone = 0;
	if (v.contains("zone")) {
		picojson::value zone = v.get("zone");
		if (zone.is<picojson::object>() && zone.contains("value")) {
			picojson::array zones = zone.get("value").get<picojson::array>();
			amb_zone = ZoneToAMBZone(zones);
		} else {
			int callback_id = -1;
			if (v.contains("asyncCallId"))
				callback_id = v.get("asyncCallId").get<double>();
			PostError(callback_id, method, "invalid_zone");
			return;
		}
	}

	if (method == "get") {
		std::string attribute = v.get("name").to_str();
		int callback_id = v.get("asyncCallId").get<double>();
		Zone::Type amb_zone = 0;

		std::transform(attribute.begin(), attribute.begin() + 1, attribute.begin(),
					   ::toupper);

		vehicle_->Get(attribute, amb_zone, callback_id);
	} else if (method == "zones") {
		std::string attribute = v.get("name").to_str();
		int callback_id = v.get("asyncCallId").get<double>();
		std::transform(attribute.begin(), attribute.begin() + 1, attribute.begin(),
					   ::toupper);

		vehicle_->GetZones(attribute, callback_id);
	} else if (method == "subscribe") {
		std::string attribute = v.get("name").to_str();
		std::transform(attribute.begin(), attribute.begin() + 1, attribute.begin(),
					   ::toupper);

		vehicle_->Subscribe(attribute, amb_zone);
	} else if (method == "unsubscribe") {
		std::string attribute = v.get("name").to_str();
		std::transform(attribute.begin(), attribute.begin() + 1, attribute.begin(),
					   ::toupper);

		vehicle_->Unsubscribe(attribute, amb_zone);
	} else if (method == "set") {
		std::string attribute = v.get("name").to_str();
		int callback_id = v.get("asyncCallId").get<double>();
		Zone::Type amb_zone = 0;

		std::transform(attribute.begin(), attribute.begin() + 1, attribute.begin(),
					   ::toupper);

		if (!v.get("value").is<picojson::object>()) {
			PostError(callback_id, "set", "invalid_operation");
		}
		picojson::object value = v.get("value").get<picojson::object>();

		vehicle_->Set(attribute, value, amb_zone, callback_id);
	} else if (method == "supported") {
		std::string attribute = v.get("name").to_str();
		int callback_id = v.get("asyncCallId").get<double>();
		Zone::Type amb_zone = 0;

		std::transform(attribute.begin(), attribute.begin() + 1, attribute.begin(),
					   ::toupper);
		vehicle_->Supported(attribute, callback_id);
	}
}

void VehicleInstance::HandleSyncMessage(const char* message) {
}

int VehicleInstance::ZoneToAMBZone(picojson::array zones) {
	Zone::Type amb_zone = 0;

	for (auto zone : zones) {
		std::string tempzone = zone.to_str();

		if (tempzone == "Front") {
			amb_zone |= Zone::Front;
		} else if (tempzone == "Middle") {
			amb_zone |= Zone::Middle;
		} else if (tempzone == "Right") {
			amb_zone |= Zone::Right;
		} else if (tempzone == "Left") {
			amb_zone |= Zone::Left;
		} else if (tempzone == "Rear") {
			amb_zone |= Zone::Rear;
		} else if (tempzone == "Center") {
			amb_zone |= Zone::Center;
		}
	}

	return amb_zone;
}

void VehicleInstance::PostError(double callback_id, const std::string& method,
								const std::string& error) {
	picojson::object msg;
	msg["method"] = picojson::value(method);
	msg["error"] = picojson::value(true);
	msg["value"] = picojson::value(error);
	if (callback_id != -1) {
		msg["asyncCallId"] =
				picojson::value(static_cast<double>(callback_id));
	}

	std::string message = picojson::value(msg).serialize();

	DebugOut() << "Error Reply message: " << message << endl;

	PostMessage(message.c_str());
}
