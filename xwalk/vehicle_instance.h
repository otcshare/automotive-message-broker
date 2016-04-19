// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VEHICLE_VEHICLE_INSTANCE_H_
#define VEHICLE_VEHICLE_INSTANCE_H_

#include <string>

#include <picojson.h>

#include "common/extension.h"
#include "vehicle.h"

class VehicleInstance : public common::Instance {
 public:
  VehicleInstance();
  virtual ~VehicleInstance() {}

 private:
  // common::Instance implementation.
  virtual void HandleMessage(const char* msg);
  virtual void HandleSyncMessage(const char* msg);

  int ZoneToAMBZone(picojson::array);

  void PostError(double callback_id, const std::string& method,
				 const std::string& error);

  Vehicle* vehicle_;
};

#endif  // VEHICLE_VEHICLE_INSTANCE_H_
