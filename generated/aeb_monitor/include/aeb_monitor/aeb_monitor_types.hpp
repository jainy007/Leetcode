// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   aeb_monitor (pattern: can_function)
// Inputs:    sha256:31e2c7fbd93dd01fd60ce38fb1537e40edd8600b328b348b62317cb35b638c06
// Regenerate: python3 -m tools.gen

#pragma once

#include <cstddef>
#include <cstdint>

namespace adas::aeb_monitor {

// States of feature 'aeb_monitor'. The numeric codes are part of the CAN interface (status signal).
enum class State : std::uint8_t {
    kInit = 0U,
    kMonitoring = 1U,
    kSafeState = 2U,
};
const char* to_string(State s) noexcept;

// Inputs of the feature logic: newest valid value of every bound rx signal (0 until received)
// and the health of the message carrying it (a valid frame within the message's timeout).
struct Inputs {
    double cmd_brake;  // AebCommand.AebBrakeRequest [0.0, 10.0] m/s2
    double cmd_counter;  // AebCommand.Counter [0.0, 15.0] 
    double cmd_status;  // AebCommand.AebStatus [0.0, 4.0] 
    double target_lateral;  // RadarObject.ObjLateralPos [-10.24, 10.22] m
    double target_range;  // RadarObject.ObjRange [0.0, 409.5] m
    double target_range_rate;  // RadarObject.ObjRangeRate [-102.4, 102.35] m/s
    double target_valid;  // RadarObject.ObjValid [0.0, 1.0] 
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
    double approved_counter;  // AebMonitorStatus.ApprovedCounter [0.0, 15.0] 
    double fault_reason;  // AebMonitorStatus.FaultReason [0.0, 15.0] 
    double permit;  // AebMonitorStatus.Permit [0.0, 1.0] 
    double status;  // AebMonitorStatus.MonStatus [0.0, 2.0] 
};

// Message AebCommand (CAN id 0x310U, 8 bytes). Source: examples/aeb_monitor/interfaces/aeb_monitor.dbc:17
struct AebCommand {
    double crc;  // [0.0, 255.0] 
    double counter;  // [0.0, 15.0] 
    double aeb_status;  // [0.0, 4.0] 
    double fcw_warning;  // [0.0, 1.0] 
    double aeb_brake_request;  // [0.0, 10.0] m/s2
};

// Message AebMonitorStatus (CAN id 0x311U, 4 bytes). Source: examples/aeb_monitor/interfaces/aeb_monitor.dbc:24
struct AebMonitorStatus {
    double crc;  // [0.0, 255.0] 
    double counter;  // [0.0, 15.0] 
    double mon_status;  // [0.0, 2.0] 
    double permit;  // [0.0, 1.0] 
    double approved_counter;  // [0.0, 15.0] 
    double fault_reason;  // [0.0, 15.0] 
};

// Message RadarObject (CAN id 0x200U, 8 bytes). Source: examples/aeb_monitor/interfaces/aeb_monitor.dbc:9
struct RadarObject {
    double crc;  // [0.0, 255.0] 
    double counter;  // [0.0, 15.0] 
    double obj_valid;  // [0.0, 1.0] 
    double obj_range;  // [0.0, 409.5] m
    double obj_range_rate;  // [-102.4, 102.35] m/s
    double obj_lateral_pos;  // [-10.24, 10.22] m
};

}  // namespace adas::aeb_monitor
