var Zone = {
  None: 0,
  Front: 1,
  Middle : 1 << 1,
  Right : 1 << 2,
  Left : 1 << 3,
  Rear : 1 << 4,
  Center : 1 << 5,
  LeftSide : 1 << 6,
  RightSide : 1 << 7,
  FrontSide : 1 << 8,
  BackSide : 1 << 9
};

Zone.Driver = Zone.Front | Zone.Left;

bluemonkey.createCustomProperty("BluemonkeySuperProperty", "It's true!");
bluemonkey.createCustomProperty("AnswerToTheUniverse", 42);

dbusConnected = bluemonkey.subscribeTo("DBusConnected");

dbusConnected.changed.connect(function () {
    bluemonkey.log("WEEEEEEEEEEEEEEEEEEEEEEEEEEEEE!" + dbusConnected.value);

    if(dbusConnected.value !== true)
        return;

    bluemonkey.exportInterface("Bluemonkey",[{'BluemonkeySuperProperty' : 'SuperProperty'},
                               {'AnswerToTheUniverse' : 'AnswerToTheUniverse'}]);
});



bluemonkey.createCustomProperty("VehicleSpeed", 10);
bluemonkey.createCustomProperty("EngineSpeed", 5000);
bluemonkey.createCustomProperty("PowertrainTorque", 324);
bluemonkey.createCustomProperty("AcceleratorPedalPosition", 50);
bluemonkey.createCustomProperty("ThrottlePosition", 50);
bluemonkey.createCustomProperty("TransmissionModeW3C", "park");
bluemonkey.createCustomProperty("TransmissionGearPosition", 1);
bluemonkey.createCustomProperty("CruiseControlActive", false);
bluemonkey.createCustomProperty("CruiseControlSpeed", 10);

///LightStatus:
bluemonkey.createCustomProperty("LightHead", false);
bluemonkey.createCustomProperty("LightRightTurn", false);
bluemonkey.createCustomProperty("LightLeftTurn", false);
bluemonkey.createCustomProperty("LightBrake", false);
bluemonkey.createCustomProperty("LightFog", true);
bluemonkey.createCustomProperty("LightHazard", false);
bluemonkey.createCustomProperty("LightParking", false);
bluemonkey.createCustomProperty("LightHighBeam", false);
bluemonkey.createCustomProperty("LightAutomatic", true);
bluemonkey.createCustomProperty("LightDynamicHighBeam", false);

bluemonkey.createCustomProperty("Horn", false);
bluemonkey.createCustomProperty("Chime", false);

///Fuel:
bluemonkey.createCustomProperty("FuelLevel", 90);
bluemonkey.createCustomProperty("FuelRange", 100);
bluemonkey.createCustomProperty("FuelConsumption", 10);
bluemonkey.createCustomProperty("FuelEconomy", 10);
bluemonkey.createCustomProperty("FuelAverageEconomy", 10);
bluemonkey.createCustomProperty("FuelAverageConsumption", 10);
bluemonkey.createCustomProperty("FuelConsumptionSinceRestart", 10);
bluemonkey.createCustomProperty("LightHighBeam", false);
bluemonkey.createCustomProperty("LightAutomatic", true);
bluemonkey.createCustomProperty("FuelTimeSinceRestart", 10000);

///EngineOil:
bluemonkey.createCustomProperty("EngineOilTemperature", 60);
bluemonkey.createCustomProperty("EngineOilPressure", 60);
bluemonkey.createCustomProperty("EngineOilChangeIndicator", false);
bluemonkey.createCustomProperty("EngineOilLifeRemaining", 30);

bluemonkey.createCustomProperty("AccelerationX", 0);
bluemonkey.createCustomProperty("AccelerationY", 0);
bluemonkey.createCustomProperty("AccelerationZ", 10);

bluemonkey.createCustomProperty("SteeringWheelAngleW3C", 10);
bluemonkey.createCustomProperty("WheelTick", 1000);

bluemonkey.createCustomProperty("EngineCoolantLevel", 100);
bluemonkey.createCustomProperty("EngineCoolantTemperature", 60);

bluemonkey.createCustomProperty("IgnitionTimeOn", 5000);
bluemonkey.createCustomProperty("IgnitionTimeOff", 5000);

bluemonkey.createCustomProperty("YawRate", 128);
bluemonkey.createCustomProperty("WheelBrake", false);

bluemonkey.createCustomProperty("BrakesWorn", false, Zone.Front | Zone.Right)
bluemonkey.createCustomProperty("BrakeFluidLevel", 100, Zone.Front | Zone.Right)
bluemonkey.createCustomProperty("BrakeFluidLevelLow", false, Zone.Front | Zone.Right)
bluemonkey.createCustomProperty("BrakePadWear", 0, Zone.Front | Zone.Right)

bluemonkey.createCustomProperty("WasherFluidLevel", 100);
bluemonkey.createCustomProperty("WasherFluidLevelLow", false);

bluemonkey.createCustomProperty("MalfunctionIndicatorOn", false);

bluemonkey.createCustomProperty("AccumulatedEngineRuntime", 0);
bluemonkey.createCustomProperty("DistanceWithMILOn", 0);
bluemonkey.createCustomProperty("DistanceSinceCodeCleared", 0);
bluemonkey.createCustomProperty("TimeRunMILOn", 0);
bluemonkey.createCustomProperty("TimeTroubleCodeClear", 0);

bluemonkey.createCustomProperty("BatteryVoltage", 13);
bluemonkey.createCustomProperty("BatteryCurrent", 1);
bluemonkey.createCustomProperty("BatteryChargeLevel", 100);

bluemonkey.createCustomProperty("TirePressure", 200, Zone.Front | Zone.Right);
bluemonkey.createCustomProperty("TirePressureLow", false, Zone.Front | Zone.Right);
bluemonkey.createCustomProperty("TireTemperature", 20, Zone.Front | Zone.Right);

bluemonkey.createCustomProperty("TirePressure", 200, Zone.Front | Zone.Left);
bluemonkey.createCustomProperty("TirePressureLow", false, Zone.Front | Zone.Left);
bluemonkey.createCustomProperty("TireTemperature", 20, Zone.Front | Zone.Left);

bluemonkey.createCustomProperty("TirePressure", 200, Zone.Rear| Zone.Right);
bluemonkey.createCustomProperty("TirePressureLow", false, Zone.Rear | Zone.Right);
bluemonkey.createCustomProperty("TireTemperature", 20, Zone.Rear | Zone.Right);

bluemonkey.createCustomProperty("TirePressure", 200, Zone.Rear | Zone.Left);
bluemonkey.createCustomProperty("TirePressureLow", false, Zone.Rear | Zone.Left);
bluemonkey.createCustomProperty("TireTemperature", 20, Zone.Rear | Zone.Left);

bluemonkey.createCustomProperty("ActiveNoiseControlMode", false);
bluemonkey.createCustomProperty("AvailableSounds", ["LightSpeed", "v8"]);
bluemonkey.createCustomProperty("EngineSoundEnhancementMode", "");

bluemonkey.createCustomProperty("SeatPositionBackCushion", 0, Zone.Front | Zone.Right);
bluemonkey.createCustomProperty("SeatPositionRecline", 0, Zone.Front | Zone.Right);
bluemonkey.createCustomProperty("SeatPositionSlide", 0, Zone.Front | Zone.Right);
bluemonkey.createCustomProperty("SeatPositionCushionHeight", 0, Zone.Front | Zone.Right);
bluemonkey.createCustomProperty("SeatPositionHeadrest", 0, Zone.Front | Zone.Right);
bluemonkey.createCustomProperty("SeatPositionSideCushion", 0, Zone.Front | Zone.Right);

bluemonkey.createCustomProperty("SeatPositionBackCushion", 0, Zone.Front | Zone.Left);
bluemonkey.createCustomProperty("SeatPositionRecline", 0, Zone.Front | Zone.Left);
bluemonkey.createCustomProperty("SeatPositionSlide", 0, Zone.Front | Zone.Left);
bluemonkey.createCustomProperty("SeatPositionCushionHeight", 0, Zone.Front | Zone.Left);
bluemonkey.createCustomProperty("SeatPositionHeadrest", 0, Zone.Front | Zone.Left);
bluemonkey.createCustomProperty("SeatPositionSideCushion", 0, Zone.Front | Zone.Left);

bluemonkey.createCustomProperty("VehicleDriveMode", "auto");

bluemonkey.createCustomProperty("MirrorSettingPan", 0, Zone.Front | Zone.Left);
bluemonkey.createCustomProperty("MirrorSettingTilt", 0, Zone.Front | Zone.Left);

bluemonkey.createCustomProperty("MirrorSettingPan", 0, Zone.Front | Zone.Right);
bluemonkey.createCustomProperty("MirrorSettingTilt", 0, Zone.Front | Zone.Right);

bluemonkey.createCustomProperty("DashboardIllumination", 5);

bluemonkey.createCustomProperty("AntilockBreakingSystem", false);
bluemonkey.createCustomProperty("AntilockBreakingSystemEnabled", true);

bluemonkey.createCustomProperty("TractionControlSystem", true);
bluemonkey.createCustomProperty("TractionControlSystemEnabled", true);

bluemonkey.createCustomProperty("ElectronicStabilityControlEnabled", true);
bluemonkey.createCustomProperty("ElectronicStabilityControlEngaged", true);

bluemonkey.createCustomProperty("VehicleTopSpeedLimit", 150);

bluemonkey.createCustomProperty("AirbagActivated", true, Zone.Front | Zone.Left);
bluemonkey.createCustomProperty("AirbagDeployed", false, Zone.Front | Zone.Left);
bluemonkey.createCustomProperty("AirbagActivated", true, Zone.Front | Zone.Right);
bluemonkey.createCustomProperty("AirbagDeployed", false, Zone.Front | Zone.Right);

bluemonkey.createCustomProperty("ChildSafetyLock", false, Zone.Rear | Zone.Left);
bluemonkey.createCustomProperty("ChildSafetyLock", true, Zone.Rear | Zone.Right);

bluemonkey.createCustomProperty("AntilockBrakingSystem", false);
bluemonkey.createCustomProperty("AntilockBrakingSystemEnabled", true);

bluemonkey.createCustomProperty("DoorStatusW3C", "closed", Zone.Front | Zone.Right);
bluemonkey.createCustomProperty("DoorStatusW3C", "closed", Zone.Front | Zone.Left);
bluemonkey.createCustomProperty("DoorStatusW3C", "closed", Zone.Rear | Zone.Right);
bluemonkey.createCustomProperty("DoorStatusW3C", "closed", Zone.Rear | Zone.Left);
bluemonkey.createCustomProperty("DoorLockStatus", true, Zone.Front | Zone.Right);
bluemonkey.createCustomProperty("DoorLockStatus", true, Zone.Front | Zone.Left);
bluemonkey.createCustomProperty("DoorLockStatus", true, Zone.Rear | Zone.Right);
bluemonkey.createCustomProperty("DoorLockStatus", true, Zone.Rear | Zone.Left);

bluemonkey.createCustomProperty("OccupantStatusW3C", "adult", Zone.Front | Zone.Left);
bluemonkey.createCustomProperty("OccupantStatusW3C", "adult", Zone.Front | Zone.Right);
bluemonkey.createCustomProperty("OccupantStatusW3C", "child", Zone.Rear | Zone.Right);
bluemonkey.createCustomProperty("OccupantStatusW3C", "child", Zone.Rear | Zone.Left);
bluemonkey.createCustomProperty("SeatBeltStatus", true, Zone.Front | Zone.Right);
bluemonkey.createCustomProperty("SeatBeltStatus", true, Zone.Front | Zone.Left);
bluemonkey.createCustomProperty("SeatBeltStatus", true, Zone.Rear | Zone.Right);
bluemonkey.createCustomProperty("SeatBeltStatus", true, Zone.Rear | Zone.Left);
bluemonkey.createCustomProperty("OccupantName", "Kevron", Zone.Front | Zone.Left);
bluemonkey.createCustomProperty("OccupantName", "Irene", Zone.Front | Zone.Right);
bluemonkey.createCustomProperty("OccupantName", "Miggie", Zone.Rear | Zone.Right);
bluemonkey.createCustomProperty("OccupantName", "Emma", Zone.Rear | Zone.Left);
bluemonkey.createCustomProperty("OccupantIdentificationType", "camera", Zone.Front | Zone.Left);
bluemonkey.createCustomProperty("OccupantIdentificationType", "Bluetooth", Zone.Front | Zone.Right);
bluemonkey.createCustomProperty("OccupantIdentificationType", "pin", Zone.Rear | Zone.Right);
bluemonkey.createCustomProperty("OccupantIdentificationType", "pin", Zone.Rear | Zone.Left);

bluemonkey.createCustomProperty("TargetTemperature", 20);
bluemonkey.createCustomProperty("FanSpeed", 1);
bluemonkey.createCustomProperty("AirConditioning", false);
bluemonkey.createCustomProperty("Heater", false);
bluemonkey.createCustomProperty("AirflowDirection", 0);
bluemonkey.createCustomProperty("AirRecirculation", false);

bluemonkey.createCustomProperty("TargetTemperature", 20, Zone.Front | Zone.Left);
bluemonkey.createCustomProperty("AirRecirculation", false, Zone.Front | Zone.Left);
bluemonkey.createCustomProperty("SteeringWheelHeater", 0, Zone.Front | Zone.Left);
bluemonkey.createCustomProperty("SeatHeater", 0, Zone.Front | Zone.Left);
bluemonkey.createCustomProperty("SeatCooler", 0, Zone.Front | Zone.Left);

bluemonkey.createCustomProperty("AirflowDirection", 0, Zone.Front | Zone.Right);
bluemonkey.createCustomProperty("TargetTemperature", 25, Zone.Front | Zone.Right);
bluemonkey.createCustomProperty("SeatHeater", 0, Zone.Front | Zone.Right);
bluemonkey.createCustomProperty("SeatCooler", 0, Zone.Front | Zone.Right);

bluemonkey.createCustomProperty("ExteriorTemperature", 30.0);
bluemonkey.createCustomProperty("InteriorTemperature", 23.0);

bluemonkey.createCustomProperty("DefrostWindow", false, Zone.Front | Zone.Right);
bluemonkey.createCustomProperty("DefrostMirrors", false, Zone.Front | Zone.Right);
bluemonkey.createCustomProperty("DefrostWindow", false, Zone.Front | Zone.Left);
bluemonkey.createCustomProperty("DefrostMirrors", false, Zone.Front | Zone.Left);

bluemonkey.createCustomProperty("RainSensor", 0);

bluemonkey.createCustomProperty("WindsheildWiperSpeed", "off");
bluemonkey.createCustomProperty("WindsheildWiperSetting", "auto");

bluemonkey.createCustomProperty("ConvertableRoof", false);
bluemonkey.createCustomProperty("ConvertableRoofStatus", "closed");

bluemonkey.createCustomProperty("Sunroof", 0);
bluemonkey.createCustomProperty("SunroofTilt", 0);

bluemonkey.createCustomProperty("WindowStatus", 100, Zone.Driver);
bluemonkey.createCustomProperty("WindowLockStatus", false, Zone.Driver);
bluemonkey.createCustomProperty("WindowStatus", 100, Zone.Front | Zone.Right);
bluemonkey.createCustomProperty("WindowLockStatus", false, Zone.Front | Zone.Right);
bluemonkey.createCustomProperty("WindowStatus", 100, Zone.Rear| Zone.Right);
bluemonkey.createCustomProperty("WindowLockStatus", true, Zone.Rear | Zone.Right);
bluemonkey.createCustomProperty("WindowStatus", 100, Zone.Rear| Zone.Left);
bluemonkey.createCustomProperty("WindowLockStatus", true, Zone.Rear | Zone.Left);

bluemonkey.createCustomProperty("AtmosphericPressure", 1013);

bluemonkey.createCustomProperty("LaneDepartureStatus", "running");
bluemonkey.createCustomProperty("AlarmStatus", "prearmed");
bluemonkey.createCustomProperty("ParkingBrakeStatusW3C", "inactive");

bluemonkey.createCustomProperty("Altitude", 200);
bluemonkey.createCustomProperty("Latitude", 45.541174);
bluemonkey.createCustomProperty("Longitude", -122.958967);


var speedProperty = bluemonkey.subscribeTo("VehicleSpeed");

var testTimer = bluemonkey.createTimer();
 testTimer.interval = 3000;
 testTimer.singleShot = false;
 testTimer.timeout.connect(function() {
  //VehicleSpeed
  speedProperty.value = Math.floor((Math.random() * 100) + 1);
 });

//testTimer.start();
