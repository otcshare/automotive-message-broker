// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VEHICLE_VEHICLE_H_
#define VEHICLE_VEHICLE_H_

#include <abstractpropertytype.h>
#include <gio/gio.h>
#include <glib.h>

#include <string>
#include <thread> // NOLINT
#include <vector>

#include "common/picojson.h"

namespace common {

class Instance;

}  // namespace common

struct ObjectZone {
  ObjectZone():zone(0) {}

  ObjectZone(const std::string& o, int z, const std::string& op)
	: name(o), zone(z), object_path(op) {}

  ObjectZone(const ObjectZone& other)
	: name(other.name), zone(other.zone), object_path(other.object_path) {}

  std::string name;
  int zone;
  uint handle;

  std::string object_path;

  bool operator ==(const ObjectZone& other) const {
	return object_path == other.object_path;
  }

  picojson::object value;
};

struct ObjectZoneCompare {
  bool operator() (const ObjectZone& lhs, const ObjectZone& rhs) {
	return (lhs.name == rhs.name && lhs.zone < rhs.zone) ||
		(lhs.name < rhs.name);
  }
};

typedef std::function<void (picojson::object)> GetReply;
typedef std::function<void (std::string)> ErrorReply;

class Vehicle {
 public:
  struct CallbackInfo {
	std::string method;
	static common::Instance* instance;
	double callback_id;
  };

  explicit Vehicle(common::Instance* i);
  ~Vehicle();

  void Get(const std::string& property, Zone::Type zone, double ret_id);
  void GetZones(const std::string& property, double ret_id);
  void Subscribe(const std::string& property, Zone::Type zone);
  void Unsubscribe(const std::string& property, Zone::Type zone);
  void Set(const std::string& property, picojson::object value, Zone::Type zone, double ret_id);

 private:
  std::string FindProperty(const std::string& object_name, int zone, string &error_str);
  GDBusProxy* GetAutomotiveManager();

  static void SetupMainloop(void* data);
  GMainLoop* main_loop_;
  std::thread thread_;
  common::Instance* instance_;

  std::vector<ObjectZone*> amb_objects_;
};

#endif  // VEHICLE_VEHICLE_H_
