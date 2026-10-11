// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc (pattern: acc_controller)
// Inputs:    sha256:cce2b25a21d5b0053b9588be55a9c9be1a00be09e870c4dedb9dbbf5638be09e
// Regenerate: python3 -m tools.gen

#pragma once

#include <cstddef>
#include <cstdint>

namespace adas::acc {

// Controller modes. The numeric codes are part of the CAN interface (status signal).
enum class Mode : std::uint8_t {
    kOff = 0U,
    kStandby = 1U,
    kActive = 2U,
    kOverride = 3U,
    kFault = 4U,
};
const char* to_string(Mode m) noexcept;

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
