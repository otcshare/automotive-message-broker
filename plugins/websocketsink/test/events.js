/*
 * Copyright (c) 2012, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

/* ---------------------- vehicle event typedef --------------------------- */

function VehicleEventType()
{
    this.event = [
        "vehicle_info",
        "vehicle_info_wmi",
        "vehicle_info_vin",
        "vehicle_info_vehicle_type",
        "vehicle_info_door_type",
        "vehicle_info_door_type_1st_row",
        "vehicle_info_door_type_2nd_row",
        "vehicle_info_door_type_3rd_row",
        "vehicle_info_fuel_type",
        "vehicle_info_transmission_gear_type",
        "vehicle_info_wheel_info",
        "vehicle_info_wheel_info_radius",
        "vehicle_info_wheel_info_track",
        "running_status",
        "running_status_vehicle_power_mode",
        "running_status_speedometer",
        "running_status_engine_speed",
        "running_status_trip_meter",
        "running_status_trip_meter_1",
        "running_status_trip_meter_2",
        "running_status_trip_meter_1_mileage",
        "running_status_trip_meter_2_mileage",
        "running_status_trip_meter_1_average_speed",
        "running_status_trip_meter_2_average_speed",
        "running_status_trip_meter_1_fuel_consumption",
        "running_status_trip_meter_2_fuel_consumption",
        "running_status_transmission_gear_status",
        "running_status_cruise_control",
        "running_status_cruise_control_status",
        "running_status_cruise_control_speed",
        "running_status_wheel_brake",
        "running_status_lights_status",
        "running_status_lights_status_head",
        "running_status_lights_status_high_beam",
        "running_status_lights_status_turn_left",
        "running_status_lights_status_turn_right",
        "running_status_lights_status_brake",
        "running_status_lights_status_fog_front",
        "running_status_lights_status_fog_rear",
        "running_status_lights_status_hazard",
        "running_status_lights_status_parking",
        "running_status_interior_lights_status",
        "running_status_interior_lights_status_driver",
        "running_status_interior_lights_status_passenger",
        "running_status_interior_lights_status_center",
        "running_status_automatic_headlights",
        "running_status_dynamic_high_beam",
        "running_status_horn",
        "running_status_chime",
        "running_status_fuel",
        "running_status_estimated_range",
        "running_status_engine_oil",
        "running_status_engine_oil_remaining",
        "running_status_engine_oil_change",
        "running_status_engine_oil_temp",
        "running_status_engine_coolant",
        "running_status_engine_coolant_level",
        "running_status_engine_coolant_temp",
        "running_status_steering_wheel_angle",
        "maintenance",
        "maintenance_odometer",
        "maintenance_transmission_oil",
        "maintenance_transmission_oil_life_level",
        "maintenance_transmission_oil_temp",
        "maintenance_brake_fluid_level",
        "maintenance_washer_fluid_level",
        "maintenance_malfunction_indicator_lamp",
        "maintenance_battery",
        "maintenance_battery_voltage",
        "maintenance_battery_current",
        "maintenance_tire_pressure",
        "maintenance_tire_pressure_front_left",
        "maintenance_tire_pressure_front_right",
        "maintenance_tire_pressure_rear_left",
        "maintenance_tire_pressure_rear_right",
        "maintenance_tire_pressure_status",
        "maintenance_tire_pressure_status_front_left",
        "maintenance_tire_pressure_status_front_right",
        "maintenance_tire_pressure_status_rear_left",
        "maintenance_tire_pressure_status_rear_right",
        "personalization",
        "personalization_key_id",
        "personalization_language",
        "personalization_measurement_system",
        "personalization_measurement_system_string",
        "personalization_measurement_system_string_fuel",
        "personalization_measurement_system_string_distance",
        "personalization_measurement_system_string_speed",
        "personalization_measurement_system_string_consumption",
        "personalization_mirror",
        "personalization_mirror_driver",
        "personalization_mirror_passenger",
        "personalization_mirror_inside",
        "personalization_steering_wheel_position",
        "personalization_steering_wheel_position_slide",
        "personalization_steering_wheel_position_tilt",
        "personalization_driving_mode",
        "personalization_driver_seat_position",
        "personalization_driver_seat_position_recline_seatback",
        "personalization_driver_seat_position_slide",
        "personalization_driver_seat_position_cushion_height",
        "personalization_driver_seat_position_headrest",
        "personalization_driver_seat_position_back_cushion",
        "personalization_driver_seat_position_side_cushion",
        "personalization_passenger_seat_position",
        "personalization_passenger_seat_position_recline_seatback",
        "personalization_passenger_seat_position_slide",
        "personalization_passenger_seat_position_cushion_height",
        "personalization_passenger_seat_position_headrest",
        "personalization_passenger_seat_position_back_cushion",
        "personalization_passenger_seat_position_side_cushion",
        "personalization_dashboard_illumination",
        "personalization_generated_vehicle_sound_mode",
        "driving_safety",
        "driving_safety_antilock_braking_system",
        "driving_safety_traction_control_system",
        "driving_safety_electronic_stability_control",
        "driving_safety_vehicle_top_speed_limit",
        "driving_safety_airbag_status",
        "driving_safety_airbag_status_driver",
        "driving_safety_airbag_status_passenger",
        "driving_safety_airbag_status_side",
        "driving_safety_door_open_status",
        "driving_safety_door_open_status_driver",
        "driving_safety_door_open_status_passenger",
        "driving_safety_door_open_status_rear_left",
        "driving_safety_door_open_status_rear_right",
        "driving_safety_door_open_status_trunk",
        "driving_safety_door_open_status_fuel_filter_cap",
        "driving_safety_door_open_status_hood",
        "driving_safety_door_lock_status",
        "driving_safety_door_lock_status_driver",
        "driving_safety_door_lock_status_passenger",
        "driving_safety_door_lock_status_rear_left",
        "driving_safety_door_lock_status_rear_right",
        "driving_safety_child_safety_lock",
        "driving_safety_occupants_status",
        "driving_safety_occupants_status_driver",
        "driving_safety_occupants_status_passenger",
        "driving_safety_occupants_status_rear_left",
        "driving_safety_occupants_status_rear_right",
        "driving_safety_seat_belt",
        "driving_safety_seat_belt_driver",
        "driving_safety_seat_belt_passenger",
        "driving_safety_seat_belt_rear_left",
        "driving_safety_seat_belt_rear_right",
        "driving_safety_window_lock",
        "driving_safety_window_lock_driver",
        "driving_safety_window_lock_passenger",
        "driving_safety_window_lock_rear_left",
        "driving_safety_window_lock_rear_right",
        "driving_safety_obstacle_distance",
        "driving_safety_obstacle_distance_sensor_status",
        "driving_safety_obstacle_distance_front_center",
        "driving_safety_obstacle_distance_rear_center",
        "driving_safety_obstacle_distance_front_left",
        "driving_safety_obstacle_distance_front_right",
        "driving_safety_obstacle_distance_middle_left",
        "driving_safety_obstacle_distance_middle_right",
        "driving_safety_obstacle_distance_rear_left",
        "driving_safety_obstacle_distance_rear_right",
        "driving_safety_front_collision_detection",
        "driving_safety_front_collision_detection_status",
        "driving_safety_front_collision_detection_distance",
        "driving_safety_front_collision_detection_time",
        "vision_system",
        "vision_system_lane_departure_detection_status",
        "vision_system_lane_departed",
        "parking",
        "parking_security_alert",
        "parking_parking_brake",
        "parking_parking_lights",
        "climate_environment_interior_temp",
        "climate_environment_exterior_temp",
        "climate_environment_exterior_brightness",
        "climate_environment_rain_sensor",
        "climate_environment_windshield_wiper",
        "climate_environment_rear_wiper",
        "climate_environment_hvac_fan",
        "climate_environment_hvac_fan_direction",
        "climate_environment_hvac_fan_speed",
        "climate_environment_hvac_fan_target_temp",
        "climate_environment_air_conditioning",
        "climate_environment_air_recirculation",
        "climate_environment_heater",
        "climate_environment_defrost",
        "climate_environment_defrost_windshield",
        "climate_environment_defrost_rear_window",
        "climate_environment_defrost_side_mirrors",
        "climate_environment_steering_wheel_heater",
        "climate_environment_seat_heater",
        "climate_environment_seat_cooler",
        "climate_environment_window",
        "climate_environment_window_driver",
        "climate_environment_window_passenger",
        "climate_environment_window_rear_left",
        "climate_environment_window_rear_right",
        "climate_environment_sunroof",
        "climate_environment_sunroof_openness",
        "climate_environment_sunroof_tilt",
        "climate_environment_convertible_roof"
    ];
    this.value = [];

    /* set random initial values for all the props */
    for(i in this.event)
    {
        var prop = this.event[i];
        this.value[prop] = Math.floor(Math.random() * 1000000);
    }
}

VehicleEventType.prototype.getSupportedEventList = function(val)
{
    /* for undefined just assume everything */
    if((val == undefined)||(val === ""))
        return this.event;

    /* grab every event with case insensitive prefix of val */
    var value = val.toLowerCase();
    var list = [];
    for(i in this.event)
    {
        var prop = this.event[i].toLowerCase();
        if(prop.indexOf(value) === 0)
        {
            list[list.length] = prop;
        }
    }

    /* if the target val isn't alone, remove it, it's a grouping */
    var idx = list.indexOf(value);
    if((idx >= 0)&&(list.length > 1))
    {
        list.splice(idx, 1);
    }
    return list;
}

VehicleEventType.prototype.getValueEventList = function(val)
{
    var i, j, list = this.getSupportedEventList(val);
    for(i = 0; i < list.length; i++)
    {
        for(j = i + 1; j < list.length; j++)
        {
            if(list[j].indexOf(list[i]) === 0)
            {
                list.splice(i, 1);
                i--;
            }
        }
    }
    return list;
}

VehicleEventType.prototype.getValuesEventList = function(vals)
{
    var i, j, list = [];
    for(i = 0; i < vals.length; i++)
    {
        var sublist = this.getValueEventList(vals[i]);
        for(j = 0; j < sublist.length; j++)
        {
            if(list.indexOf(sublist[j]) < 0)
            {
                list[list.length] = sublist[j];
            }
        }
    }
    return list;
}

VehicleEventType.prototype.isValueEvent = function(val)
{
    var list = this.getValueEventList(val);
    return(list.length === 1);
}

VehicleEventType.prototype.getValue = function(prop)
{
    return this.value[prop];
}

VehicleEventType.prototype.isValid = function(prop)
{
    return (this.event.indexOf(prop) >= 0);
}

VehicleEventType.prototype.setValue = function(prop, newval)
{
    this.value[prop] = newval;
}
