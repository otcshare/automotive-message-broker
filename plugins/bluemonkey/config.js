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

amb.createCustomProperty("BluemonkeySuperProperty", "It's true!");
amb.createCustomProperty("AnswerToTheUniverse", 42);

dbusConnected = amb.subscribeTo("DBusConnected");

dbusConnected.changed.connect(function () {
	if(dbusConnected.value !== true)
		return;

	amb.exportInterface("Bluemonkey",[{'BluemonkeySuperProperty' : 'SuperProperty'},
							   {'AnswerToTheUniverse' : 'AnswerToTheUniverse'}]);
});

amb.createCustomProperty("VehicleSpeed", 10);
amb.createCustomProperty("EngineSpeed", 5000);
amb.createCustomProperty("PowertrainTorque", 324);
amb.createCustomProperty("AcceleratorPedalPosition", 50);
amb.createCustomProperty("ThrottlePosition", 50);
amb.createCustomProperty("TransmissionModeW3C", "park");
amb.createCustomProperty("TransmissionGearPosition", 1);
amb.createCustomProperty("CruiseControlActive", false);
amb.createCustomProperty("CruiseControlSpeed", 10);

///LightStatus:
amb.createCustomProperty("LightHead", false);
amb.createCustomProperty("LightRightTurn", false);
amb.createCustomProperty("LightLeftTurn", false);
amb.createCustomProperty("LightBrake", false);
amb.createCustomProperty("LightFog", true);
amb.createCustomProperty("LightHazard", false);
amb.createCustomProperty("LightParking", false);
amb.createCustomProperty("LightHighBeam", false);
amb.createCustomProperty("LightAutomatic", true);
amb.createCustomProperty("LightDynamicHighBeam", false);

amb.createCustomProperty("Horn", false);
amb.createCustomProperty("Chime", false);

///Fuel:
amb.createCustomProperty("FuelLevel", 90);
amb.createCustomProperty("FuelRange", 100);
amb.createCustomProperty("FuelConsumption", 10);
amb.createCustomProperty("FuelEconomy", 10);
amb.createCustomProperty("FuelAverageEconomy", 10);
amb.createCustomProperty("FuelAverageConsumption", 10);
amb.createCustomProperty("FuelConsumptionSinceRestart", 10);
amb.createCustomProperty("LightHighBeam", false);
amb.createCustomProperty("LightAutomatic", true);
amb.createCustomProperty("FuelTimeSinceRestart", 10000);

///EngineOil:
amb.createCustomProperty("EngineOilTemperature", 60);
amb.createCustomProperty("EngineOilPressure", 60);
amb.createCustomProperty("EngineOilChangeIndicator", false);
amb.createCustomProperty("EngineOilLifeRemaining", 30);

amb.createCustomProperty("AccelerationX", 0);
amb.createCustomProperty("AccelerationY", 0);
amb.createCustomProperty("AccelerationZ", 10);

amb.createCustomProperty("SteeringWheelAngleW3C", 10);
amb.createCustomProperty("WheelTick", 1000);

amb.createCustomProperty("EngineCoolantLevel", 100);
amb.createCustomProperty("EngineCoolantTemperature", 60);

amb.createCustomProperty("IgnitionTimeOn", 5000);
amb.createCustomProperty("IgnitionTimeOff", 5000);

amb.createCustomProperty("YawRate", 128);
amb.createCustomProperty("WheelBrake", false);

amb.createCustomProperty("BrakesWorn", false, Zone.Front | Zone.Right)
amb.createCustomProperty("BrakeFluidLevel", 100, Zone.Front | Zone.Right)
amb.createCustomProperty("BrakeFluidLevelLow", false, Zone.Front | Zone.Right)
amb.createCustomProperty("BrakePadWear", 0, Zone.Front | Zone.Right)

amb.createCustomProperty("WasherFluidLevel", 100);
amb.createCustomProperty("WasherFluidLevelLow", false);

amb.createCustomProperty("MalfunctionIndicatorOn", false);

amb.createCustomProperty("AccumulatedEngineRuntime", 0);
amb.createCustomProperty("DistanceWithMILOn", 0);
amb.createCustomProperty("DistanceSinceCodeCleared", 0);
amb.createCustomProperty("TimeRunMILOn", 0);
amb.createCustomProperty("TimeTroubleCodeClear", 0);

amb.createCustomProperty("BatteryVoltage", 13);
amb.createCustomProperty("BatteryCurrent", 1);
amb.createCustomProperty("BatteryChargeLevel", 100);

amb.createCustomProperty("TirePressure", 200, Zone.Front | Zone.Right);
amb.createCustomProperty("TirePressureLow", false, Zone.Front | Zone.Right);
amb.createCustomProperty("TireTemperature", 20, Zone.Front | Zone.Right);

amb.createCustomProperty("TirePressure", 200, Zone.Front | Zone.Left);
amb.createCustomProperty("TirePressureLow", false, Zone.Front | Zone.Left);
amb.createCustomProperty("TireTemperature", 20, Zone.Front | Zone.Left);

amb.createCustomProperty("TirePressure", 200, Zone.Rear| Zone.Right);
amb.createCustomProperty("TirePressureLow", false, Zone.Rear | Zone.Right);
amb.createCustomProperty("TireTemperature", 20, Zone.Rear | Zone.Right);

amb.createCustomProperty("TirePressure", 200, Zone.Rear | Zone.Left);
amb.createCustomProperty("TirePressureLow", false, Zone.Rear | Zone.Left);
amb.createCustomProperty("TireTemperature", 20, Zone.Rear | Zone.Left);

amb.createCustomProperty("ActiveNoiseControlMode", false);
amb.createCustomProperty("AvailableSounds", ["LightSpeed", "v8"]);
amb.createCustomProperty("EngineSoundEnhancementMode", "");

amb.createCustomProperty("SeatPositionBackCushion", 0, Zone.Front | Zone.Right);
amb.createCustomProperty("SeatPositionRecline", 0, Zone.Front | Zone.Right);
amb.createCustomProperty("SeatPositionSlide", 0, Zone.Front | Zone.Right);
amb.createCustomProperty("SeatPositionCushionHeight", 0, Zone.Front | Zone.Right);
amb.createCustomProperty("SeatPositionHeadrest", 0, Zone.Front | Zone.Right);
amb.createCustomProperty("SeatPositionSideCushion", 0, Zone.Front | Zone.Right);

amb.createCustomProperty("SeatPositionBackCushion", 0, Zone.Front | Zone.Left);
amb.createCustomProperty("SeatPositionRecline", 0, Zone.Front | Zone.Left);
amb.createCustomProperty("SeatPositionSlide", 0, Zone.Front | Zone.Left);
amb.createCustomProperty("SeatPositionCushionHeight", 0, Zone.Front | Zone.Left);
amb.createCustomProperty("SeatPositionHeadrest", 0, Zone.Front | Zone.Left);
amb.createCustomProperty("SeatPositionSideCushion", 0, Zone.Front | Zone.Left);

amb.createCustomProperty("VehicleDriveMode", "auto");

amb.createCustomProperty("MirrorSettingPan", 0, Zone.Front | Zone.Left);
amb.createCustomProperty("MirrorSettingTilt", 0, Zone.Front | Zone.Left);

amb.createCustomProperty("MirrorSettingPan", 0, Zone.Front | Zone.Right);
amb.createCustomProperty("MirrorSettingTilt", 0, Zone.Front | Zone.Right);

amb.createCustomProperty("DashboardIllumination", 5);

amb.createCustomProperty("AntilockBreakingSystem", false);
amb.createCustomProperty("AntilockBreakingSystemEnabled", true);

amb.createCustomProperty("TractionControlSystem", true);
amb.createCustomProperty("TractionControlSystemEnabled", true);

amb.createCustomProperty("ElectronicStabilityControlEnabled", true);
amb.createCustomProperty("ElectronicStabilityControlEngaged", true);

amb.createCustomProperty("VehicleTopSpeedLimit", 150);

amb.createCustomProperty("AirbagActivated", true, Zone.Front | Zone.Left);
amb.createCustomProperty("AirbagDeployed", false, Zone.Front | Zone.Left);
amb.createCustomProperty("AirbagActivated", true, Zone.Front | Zone.Right);
amb.createCustomProperty("AirbagDeployed", false, Zone.Front | Zone.Right);

amb.createCustomProperty("ChildSafetyLock", false, Zone.Rear | Zone.Left);
amb.createCustomProperty("ChildSafetyLock", true, Zone.Rear | Zone.Right);

amb.createCustomProperty("AntilockBrakingSystem", false);
amb.createCustomProperty("AntilockBrakingSystemEnabled", true);

amb.createCustomProperty("DoorStatusW3C", "closed", Zone.Front | Zone.Right);
amb.createCustomProperty("DoorStatusW3C", "closed", Zone.Front | Zone.Left);
amb.createCustomProperty("DoorStatusW3C", "closed", Zone.Rear | Zone.Right);
amb.createCustomProperty("DoorStatusW3C", "closed", Zone.Rear | Zone.Left);
amb.createCustomProperty("DoorLockStatus", true, Zone.Front | Zone.Right);
amb.createCustomProperty("DoorLockStatus", true, Zone.Front | Zone.Left);
amb.createCustomProperty("DoorLockStatus", true, Zone.Rear | Zone.Right);
amb.createCustomProperty("DoorLockStatus", true, Zone.Rear | Zone.Left);

amb.createCustomProperty("OccupantStatusW3C", "adult", Zone.Front | Zone.Left);
amb.createCustomProperty("OccupantStatusW3C", "adult", Zone.Front | Zone.Right);
amb.createCustomProperty("OccupantStatusW3C", "child", Zone.Rear | Zone.Right);
amb.createCustomProperty("OccupantStatusW3C", "child", Zone.Rear | Zone.Left);
amb.createCustomProperty("SeatBeltStatus", true, Zone.Front | Zone.Right);
amb.createCustomProperty("SeatBeltStatus", true, Zone.Front | Zone.Left);
amb.createCustomProperty("SeatBeltStatus", true, Zone.Rear | Zone.Right);
amb.createCustomProperty("SeatBeltStatus", true, Zone.Rear | Zone.Left);
amb.createCustomProperty("OccupantName", "Kevron", Zone.Front | Zone.Left);
amb.createCustomProperty("OccupantName", "Irene", Zone.Front | Zone.Right);
amb.createCustomProperty("OccupantName", "Miggie", Zone.Rear | Zone.Right);
amb.createCustomProperty("OccupantName", "Emma", Zone.Rear | Zone.Left);
amb.createCustomProperty("OccupantIdentificationType", "camera", Zone.Front | Zone.Left);
amb.createCustomProperty("OccupantIdentificationType", "Bluetooth", Zone.Front | Zone.Right);
amb.createCustomProperty("OccupantIdentificationType", "pin", Zone.Rear | Zone.Right);
amb.createCustomProperty("OccupantIdentificationType", "pin", Zone.Rear | Zone.Left);

amb.createCustomProperty("TargetTemperature", 20);
amb.createCustomProperty("FanSpeed", 1);
amb.createCustomProperty("AirConditioning", false);
amb.createCustomProperty("Heater", false);
amb.createCustomProperty("AirflowDirection", 0);
amb.createCustomProperty("AirRecirculation", false);

amb.createCustomProperty("TargetTemperature", 20, Zone.Front | Zone.Left);
amb.createCustomProperty("AirRecirculation", false, Zone.Front | Zone.Left);
amb.createCustomProperty("SteeringWheelHeater", 0, Zone.Front | Zone.Left);
amb.createCustomProperty("SeatHeater", 0, Zone.Front | Zone.Left);
amb.createCustomProperty("SeatCooler", 0, Zone.Front | Zone.Left);

amb.createCustomProperty("AirflowDirection", 0, Zone.Front | Zone.Right);
amb.createCustomProperty("TargetTemperature", 25, Zone.Front | Zone.Right);
amb.createCustomProperty("SeatHeater", 0, Zone.Front | Zone.Right);
amb.createCustomProperty("SeatCooler", 0, Zone.Front | Zone.Right);

amb.createCustomProperty("ExteriorTemperature", 30.0);
amb.createCustomProperty("InteriorTemperature", 23.0);

amb.createCustomProperty("DefrostWindow", false, Zone.Front | Zone.Right);
amb.createCustomProperty("DefrostMirrors", false, Zone.Front | Zone.Right);
amb.createCustomProperty("DefrostWindow", false, Zone.Front | Zone.Left);
amb.createCustomProperty("DefrostMirrors", false, Zone.Front | Zone.Left);

amb.createCustomProperty("RainSensor", 0);

amb.createCustomProperty("WindsheildWiperSpeed", "off");
amb.createCustomProperty("WindsheildWiperSetting", "auto");

amb.createCustomProperty("ConvertableRoof", false);
amb.createCustomProperty("ConvertableRoofStatus", "closed");

amb.createCustomProperty("Sunroof", 0);
amb.createCustomProperty("SunroofTilt", 0);

amb.createCustomProperty("WindowStatus", 100, Zone.Driver);
amb.createCustomProperty("WindowLockStatus", false, Zone.Driver);
amb.createCustomProperty("WindowStatus", 100, Zone.Front | Zone.Right);
amb.createCustomProperty("WindowLockStatus", false, Zone.Front | Zone.Right);
amb.createCustomProperty("WindowStatus", 100, Zone.Rear| Zone.Right);
amb.createCustomProperty("WindowLockStatus", true, Zone.Rear | Zone.Right);
amb.createCustomProperty("WindowStatus", 100, Zone.Rear| Zone.Left);
amb.createCustomProperty("WindowLockStatus", true, Zone.Rear | Zone.Left);

amb.createCustomProperty("AtmosphericPressure", 1013);

amb.createCustomProperty("LaneDepartureStatus", "running");
amb.createCustomProperty("AlarmStatus", "prearmed");
amb.createCustomProperty("ParkingBrakeStatusW3C", "inactive");

amb.createCustomProperty("Altitude", 200);
amb.createCustomProperty("Latitude", 45.541174);
amb.createCustomProperty("Longitude", -122.958967);


var speedProperty = amb.subscribeTo("VehicleSpeed");

var testTimer = bluemonkey.createTimer();
 testTimer.interval = 3000;
 testTimer.singleShot = false;
 testTimer.timeout.connect(function() {
  //VehicleSpeed
  speedProperty.value = Math.floor((Math.random() * 100) + 1);
 });

//testTimer.start();
