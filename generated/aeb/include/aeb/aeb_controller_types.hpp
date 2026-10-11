// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   aeb (pattern: can_function)
// Inputs:    sha256:6606fd57f798610e5c596e91f29bec679e2b42414be7c909658cc37bce340b0d
// Regenerate: python3 -m tools.gen

#pragma once

#include <cstddef>
#include <cstdint>

namespace adas::aeb {

// States of feature 'aeb'. The numeric codes are part of the CAN interface (status signal).
enum class State : std::uint8_t {
    kStandby = 0U,
    kWarning = 1U,
    kPartialBrake = 2U,
    kFullBrake = 3U,
    kFault = 4U,
};
const char* to_string(State s) noexcept;

// Inputs of the feature logic: newest valid value of every bound rx signal (0 until received)
// and the health of the message carrying it (a valid frame within the message's timeout).
struct Inputs {
    double accel_pedal;  // DriverInput.AccelPedalPos [0.0, 100.0] %
    double brake_pedal;  // DriverInput.BrakePedal [0.0, 1.0] 
    double ego_speed;  // EgoState.VehicleSpeed [0.0, 655.35] km/h
    double target_lateral;  // RadarObject.ObjLateralPos [-10.24, 10.22] m
    double target_range;  // RadarObject.ObjRange [0.0, 409.5] m
    double target_range_rate;  // RadarObject.ObjRangeRate [-102.4, 102.35] m/s
    double target_valid;  // RadarObject.ObjValid [0.0, 1.0] 
    bool accel_pedal_ok;
    bool brake_pedal_ok;
    bool ego_speed_ok;
    bool target_lateral_ok;
    bool target_range_ok;
    bool target_range_rate_ok;
    bool target_valid_ok;
    bool all_ok;  // every rx message is healthy
};

// Outputs of the feature logic, encoded into the tx messages every cycle.
struct Outputs {
    double brake_request;  // AebCommand.AebBrakeRequest [0.0, 10.0] m/s2
    double fcw_warning;  // AebCommand.FcwWarning [0.0, 1.0] 
    double status;  // AebCommand.AebStatus [0.0, 4.0] 
};

// Message AebCommand (CAN id 0x310U, 8 bytes). Source: examples/aeb/interfaces/aeb.dbc:35
struct AebCommand {
    double crc;  // [0.0, 255.0] 
    double counter;  // [0.0, 15.0] 
    double aeb_status;  // [0.0, 4.0] 
    double fcw_warning;  // [0.0, 1.0] 
    double aeb_brake_request;  // [0.0, 10.0] m/s2
};

// Message DriverInput (CAN id 0x220U, 8 bytes). Source: examples/aeb/interfaces/aeb.dbc:22
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

// Message EgoState (CAN id 0x210U, 8 bytes). Source: examples/aeb/interfaces/aeb.dbc:17
struct EgoState {
    double crc;  // [0.0, 255.0] 
    double counter;  // [0.0, 15.0] 
    double vehicle_speed;  // [0.0, 655.35] km/h
};

// Message RadarObject (CAN id 0x200U, 8 bytes). Source: examples/aeb/interfaces/aeb.dbc:9
struct RadarObject {
    double crc;  // [0.0, 255.0] 
    double counter;  // [0.0, 15.0] 
    double obj_valid;  // [0.0, 1.0] 
    double obj_range;  // [0.0, 409.5] m
    double obj_range_rate;  // [-102.4, 102.35] m/s
    double obj_lateral_pos;  // [-10.24, 10.22] m
};

}  // namespace adas::aeb
