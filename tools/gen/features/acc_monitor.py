"""Feature 'acc_monitor': independent safety monitor B(D) of the ACC controller B(D).

It reads the same sensor messages as the controller plus the controller's AccCommand, checks
every new command against a safety envelope and approves it (permit + echoed E2E counter) for
the actuator ECU. The first violation latches SAFE_STATE. The checks are written independently
of the controller logic and use only the envelope values, never the control law."""
from __future__ import annotations

NAME = "acc_monitor"
STATES = ["INIT", "MONITORING", "SAFE_STATE"]
# Hand-written plant layout of the output message: (header, signal namespace, id, E2E config).
PLANT_TABLES = ("ea/plant/monitor_can.hpp", "ea::plant::can::monitor", "ea::plant::can::kAccMonitorId", "ea::plant::can::kAccMonitorE2e")
OBSERVER = ("ea/plant/monitored_actuator.hpp", "ea::plant::AccActuator")
EXTRA_STEPS = ["sil/steps/monitored_actuator.yaml"]
STIMULUS = ("ea/plant/command_stub.hpp", "ea::plant::AccCommandStub")
REASONS = ["NONE", "COMMAND_TIMEOUT", "REQUEST_LIMIT", "ACCEL_AND_BRAKE", "REQUEST_OUTSIDE_ACTIVE",
           "REQUEST_RATE", "UNINTENDED_ACCEL"]

CALIBRATION = {
    "active_status_code": "number", "accel_max_mps2": "number", "decel_max_mps2": "number",
    "request_tolerance_mps2": "number", "jerk_increase_max_mps3": "number", "jerk_decrease_max_mps3": "number",
    "jerk_tolerance_mps3": "number", "min_ttc_for_accel_s": "number", "max_target_range_m": "number",
    "lane_half_width_m": "number", "accel_decay_min_mps3": "number",
}

INTERFACE = {
    "target_valid": ("rx", True, lambda c: (0, 1)),
    "target_range": ("rx", False, lambda c: (0, c["max_target_range_m"])),
    "target_range_rate": ("rx", False, lambda c: (-50, 50)),
    "target_lateral": ("rx", False, lambda c: (-2 * c["lane_half_width_m"], 2 * c["lane_half_width_m"])),
    "cmd_status": ("rx", True, lambda c: (0, 4)),
    # The monitor must see requests beyond the limits it enforces.
    "cmd_accel": ("rx", False, lambda c: (0, c["accel_max_mps2"] + 2 * c["request_tolerance_mps2"])),
    "cmd_brake": ("rx", False, lambda c: (0, c["decel_max_mps2"] + 2 * c["request_tolerance_mps2"])),
    "cmd_counter": ("rx", True, lambda c: (0, 14)),
    "status": ("tx", True, lambda c: (0, len(STATES) - 1)),
    "permit": ("tx", True, lambda c: (0, 1)),
    "approved_counter": ("tx", True, lambda c: (0, 14)),
    "fault_reason": ("tx", True, lambda c: (0, len(REASONS) - 1)),
}


def _cycle(cal, timeout_of, cycle_ms):
    return cycle_ms


# Diagnostic mechanisms and their fault detection time (FDTI, ms) for rule GEN-065.
MECHANISMS = {
    "command_timeout": lambda cal, timeout_of, cycle_ms: timeout_of("cmd_status"),
    "request_limit": _cycle,
    "accel_and_brake": _cycle,
    "request_outside_active": _cycle,
    "request_rate": _cycle,
    "unintended_accel": _cycle,  # every new command must show the decay
}


def check(cal: dict) -> list[str]:
    positive = ("accel_max_mps2", "decel_max_mps2", "request_tolerance_mps2", "jerk_increase_max_mps3",
                "jerk_decrease_max_mps3", "jerk_tolerance_mps3", "min_ttc_for_accel_s", "max_target_range_m",
                "lane_half_width_m", "accel_decay_min_mps3")
    rules = [
        (all(cal[k] > 0 for k in positive), "limits, tolerances, TTC and decay must be positive"),
        (cal["active_status_code"] in (0, 1, 2, 3, 4), "active_status_code must be a controller state code (0..4)"),
        (cal["request_tolerance_mps2"] < min(cal["accel_max_mps2"], cal["decel_max_mps2"]),
         "the request tolerance must be smaller than the limits"),
    ]
    return [why for ok, why in rules if not ok]


def derive(cal: dict, bound: dict, cycle_ms: int) -> tuple[dict, list[str]]:
    """Per-cycle rate limits and the minimum acceleration decay per cycle, in m/s2."""
    errors: list[str] = []
    dt = cycle_ms / 1000.0
    return {
        "accel_decay_per_cycle": cal["accel_decay_min_mps3"] * dt,
        "accel_rate_per_cycle": (cal["jerk_increase_max_mps3"] + cal["jerk_tolerance_mps3"]) * dt,
        "brake_rate_per_cycle": (cal["jerk_decrease_max_mps3"] + cal["jerk_tolerance_mps3"]) * dt,
        "reasons": REASONS,
    }, errors
