// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "vehicle_extension.h"

#include "vehicle_instance.h"

common::Extension* CreateExtension() {
  return new VehicleExtension();
}

extern const char kSource_vehicle_api[];

VehicleExtension::VehicleExtension() {
  SetExtensionName("navigator.vehicle");
  SetJavaScriptAPI(kSource_vehicle_api);

  const char* entry_points[] = {"Zone", NULL};

  SetExtraJSEntryPoints(entry_points);
}

VehicleExtension::~VehicleExtension() {
}

common::Instance* VehicleExtension::CreateInstance() {
  return new VehicleInstance;
}
