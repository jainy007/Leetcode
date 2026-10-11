// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc_monitor (pattern: can_function)
// Inputs:    sha256:7aee4feecefbc18e2ad7476e4d93efffab091cab221dd8b1b9699ac5d31b6524
// Regenerate: python3 -m tools.gen

#pragma once

#include <cstddef>
#include <cstdint>

namespace adas::acc_monitor {

// States of feature 'acc_monitor'. The numeric codes are part of the CAN interface (status signal).
enum class State : std::uint8_t {
    kInit = 0U,
    kMonitoring = 1U,
    kSafeState = 2U,
};
const char* to_string(State s) noexcept;

// Inputs of the feature logic: newest valid value of every bound rx signal (0 until received)
// and the health of the message carrying it (a valid frame within the message's timeout).
struct Inputs {
    double cmd_accel;  // AccCommand.AccelRequest [0.0, 5.0] m/s2
    double cmd_brake;  // AccCommand.BrakeRequest [0.0, 10.0] m/s2
    double cmd_counter;  // AccCommand.Counter [0.0, 15.0] 
    double cmd_status;  // AccCommand.AccStatus [0.0, 4.0] 
    double target_lateral;  // RadarObject.ObjLateralPos [-10.24, 10.22] m
    double target_range;  // RadarObject.ObjRange [0.0, 409.5] m
    double target_range_rate;  // RadarObject.ObjRangeRate [-102.4, 102.35] m/s
    double target_valid;  // RadarObject.ObjValid [0.0, 1.0] 
    bool cmd_accel_ok;
    bool cmd_brake_ok;
    bool cmd_counter_ok;
    bool cmd_status_ok;
    bool target_lateral_ok;
    bool target_range_ok;
    bool target_range_rate_ok;
    bool target_valid_ok;
    bool all_ok;  // every rx message is healthy
};

// Outputs of the feature logic, encoded into the tx messages every cycle.
struct Outputs {
    double approved_counter;  // AccMonitorStatus.ApprovedCounter [0.0, 15.0] 
    double fault_reason;  // AccMonitorStatus.FaultReason [0.0, 15.0] 
    double permit;  // AccMonitorStatus.Permit [0.0, 1.0] 
    double status;  // AccMonitorStatus.MonStatus [0.0, 2.0] 
};

// Message AccCommand (CAN id 0x300U, 8 bytes). Source: examples/acc_monitor/interfaces/acc_monitor.dbc:17
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

// Message AccMonitorStatus (CAN id 0x301U, 4 bytes). Source: examples/acc_monitor/interfaces/acc_monitor.dbc:27
struct AccMonitorStatus {
    double crc;  // [0.0, 255.0] 
    double counter;  // [0.0, 15.0] 
    double mon_status;  // [0.0, 2.0] 
    double permit;  // [0.0, 1.0] 
    double approved_counter;  // [0.0, 15.0] 
    double fault_reason;  // [0.0, 15.0] 
};

// Message RadarObject (CAN id 0x200U, 8 bytes). Source: examples/acc_monitor/interfaces/acc_monitor.dbc:9
struct RadarObject {
    double crc;  // [0.0, 255.0] 
    double counter;  // [0.0, 15.0] 
    double obj_valid;  // [0.0, 1.0] 
    double obj_range;  // [0.0, 409.5] m
    double obj_range_rate;  // [-102.4, 102.35] m/s
    double obj_lateral_pos;  // [-10.24, 10.22] m
};

}  // namespace adas::acc_monitor
