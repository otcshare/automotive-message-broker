// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VEHICLE_VEHICLE_EXTENSION_H_
#define VEHICLE_VEHICLE_EXTENSION_H_

#include "common/extension.h"

class VehicleExtension : public common::Extension {
 public:
  VehicleExtension();
  virtual ~VehicleExtension();

 private:
  // common::Extension implementation.
  virtual common::Instance* CreateInstance();
};

#endif  // VEHICLE_VEHICLE_EXTENSION_H_
