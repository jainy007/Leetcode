// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc (pattern: can_function)
// Inputs:    sha256:6a5ccedef5c819b3983b9921989e0f2931536fb8d0b7f04299ea068c2c7eb440
// Regenerate: python3 -m tools.gen

#pragma once

#include <cstddef>
#include <cstdint>

namespace adas::acc {

// States of feature 'acc'. The numeric codes are part of the CAN interface (status signal).
enum class State : std::uint8_t {
    kOff = 0U,
    kStandby = 1U,
    kActive = 2U,
    kOverride = 3U,
    kFault = 4U,
};
const char* to_string(State s) noexcept;

// Inputs of the feature logic: newest valid value of every bound rx signal (0 until received)
// and the health of the message carrying it (a valid frame within the message's timeout).
struct Inputs {
    double accel_pedal;  // DriverInput.AccelPedalPos [0.0, 100.0] %
    double brake_pedal;  // DriverInput.BrakePedal [0.0, 1.0] 
    double cancel_button;  // DriverInput.CancelButton [0.0, 1.0] 
    double ego_speed;  // EgoState.VehicleSpeed [0.0, 655.35] km/h
    double gap_setting;  // DriverInput.GapSetting [1.0, 3.0] 
    double main_switch;  // DriverInput.MainSwitch [0.0, 1.0] 
    double resume_button;  // DriverInput.ResumeButton [0.0, 1.0] 
    double set_button;  // DriverInput.SetButton [0.0, 1.0] 
    double speed_down_button;  // DriverInput.SpeedDownButton [0.0, 1.0] 
    double speed_up_button;  // DriverInput.SpeedUpButton [0.0, 1.0] 
    double target_lateral;  // RadarObject.ObjLateralPos [-10.24, 10.22] m
    double target_range;  // RadarObject.ObjRange [0.0, 409.5] m
    double target_range_rate;  // RadarObject.ObjRangeRate [-102.4, 102.35] m/s
    double target_valid;  // RadarObject.ObjValid [0.0, 1.0] 
    bool accel_pedal_ok;
    bool brake_pedal_ok;
    bool cancel_button_ok;
    bool ego_speed_ok;
    bool gap_setting_ok;
    bool main_switch_ok;
    bool resume_button_ok;
    bool set_button_ok;
    bool speed_down_button_ok;
    bool speed_up_button_ok;
    bool target_lateral_ok;
    bool target_range_ok;
    bool target_range_rate_ok;
    bool target_valid_ok;
    bool all_ok;  // every rx message is healthy
};

// Outputs of the feature logic, encoded into the tx messages every cycle.
struct Outputs {
    double accel_request;  // AccCommand.AccelRequest [0.0, 5.0] m/s2
    double brake_request;  // AccCommand.BrakeRequest [0.0, 10.0] m/s2
    double set_speed;  // AccCommand.SetSpeed [0.0, 250.0] km/h
    double status;  // AccCommand.AccStatus [0.0, 4.0] 
    double steering_request;  // AccCommand.SteeringAngleRequest [-180.0, 180.0] deg
    double time_gap_setting;  // AccCommand.TimeGapSetting [0.0, 3.0] 
};

// Message AccCommand (CAN id 0x300U, 8 bytes). Source: examples/acc/interfaces/acc.dbc:35
struct AccCommand {
    double crc;  // [0.0, 255.0] 
    double counter;  // [0.0, 15.0] 
    double acc_status;  // [0.0, 4.0] 
    double accel_request;  // [0.0, 5.0] m/s2
    double brake_request;  // [0.0, 10.0] m/s2
    double steering_angle_request;  // [-180.0, 180.0] deg
    double set_speed;  // [0.0, 250.0] km/h
    double time_gap_setting;  // [0.0, 3.0] 
};

// Message DriverInput (CAN id 0x220U, 8 bytes). Source: examples/acc/interfaces/acc.dbc:22
struct DriverInput {
    double crc;  // [0.0, 255.0] 
    double counter;  // [0.0, 15.0] 
    double main_switch;  // [0.0, 1.0] 
    double set_button;  // [0.0, 1.0] 
    double resume_button;  // [0.0, 1.0] 
    double cancel_button;  // [0.0, 1.0] 
    double speed_up_button;  // [0.0, 1.0] 
    double speed_down_button;  // [0.0, 1.0] 
    double gap_setting;  // [1.0, 3.0] 
    double brake_pedal;  // [0.0, 1.0] 
    double accel_pedal_pos;  // [0.0, 100.0] %
};

// Message EgoState (CAN id 0x210U, 8 bytes). Source: examples/acc/interfaces/acc.dbc:17
struct EgoState {
    double crc;  // [0.0, 255.0] 
    double counter;  // [0.0, 15.0] 
    double vehicle_speed;  // [0.0, 655.35] km/h
};

// Message RadarObject (CAN id 0x200U, 8 bytes). Source: examples/acc/interfaces/acc.dbc:9
struct RadarObject {
    double crc;  // [0.0, 255.0] 
    double counter;  // [0.0, 15.0] 
    double obj_valid;  // [0.0, 1.0] 
    double obj_range;  // [0.0, 409.5] m
    double obj_range_rate;  // [-102.4, 102.35] m/s
    double obj_lateral_pos;  // [-10.24, 10.22] m
};

}  // namespace adas::acc
