"""Feature 'aeb_monitor': independent safety monitor B(D) of the AEB controller B(D).

Hazard covered: unjustified emergency braking. A braking request is justified only if, at its
onset, the monitor's own evaluation of the radar target shows a relevant in-lane target with a
time to collision within the partial (or, for full braking, the full) braking threshold plus a
margin. A justified braking stays justified while the target stays valid and in lane. The first
violation latches SAFE_STATE. Missed braking is not covered (lower ASIL, availability)."""
from __future__ import annotations

NAME = "aeb_monitor"
STATES = ["INIT", "MONITORING", "SAFE_STATE"]
# Hand-written plant layout of the output message: (header, signal namespace, id, E2E config).
PLANT_TABLES = ("ea/plant/monitor_can.hpp", "ea::plant::can::monitor", "ea::plant::can::kAebMonitorId", "ea::plant::can::kAebMonitorE2e")
OBSERVER = ("ea/plant/monitored_actuator.hpp", "ea::plant::AebActuator")
EXTRA_STEPS = ["sil/steps/monitored_actuator.yaml"]
STIMULUS = ("ea/plant/command_stub.hpp", "ea::plant::AebCommandStub")
REASONS = ["NONE", "COMMAND_TIMEOUT", "REQUEST_LIMIT", "REQUEST_OUTSIDE_BRAKING", "UNJUSTIFIED_BRAKING",
           "UNJUSTIFIED_FULL_BRAKING"]

CALIBRATION = {
    "partial_status_code": "number", "full_status_code": "number", "partial_decel_mps2": "number",
    "full_decel_mps2": "number", "request_tolerance_mps2": "number", "partial_brake_ttc_s": "number",
    "full_brake_ttc_s": "number", "ttc_margin_s": "number", "max_target_range_m": "number",
    "lane_half_width_m": "number", "lane_margin_m": "number",
}

INTERFACE = {
    "target_valid": ("rx", True, lambda c: (0, 1)),
    "target_range": ("rx", False, lambda c: (0, c["max_target_range_m"])),
    "target_range_rate": ("rx", False, lambda c: (-50, 50)),
    "target_lateral": ("rx", False, lambda c: (-2 * c["lane_half_width_m"], 2 * c["lane_half_width_m"])),
    "cmd_status": ("rx", True, lambda c: (0, 4)),
    "cmd_brake": ("rx", False, lambda c: (0, c["full_decel_mps2"] + 2 * c["request_tolerance_mps2"])),
    "cmd_counter": ("rx", True, lambda c: (0, 14)),
    "status": ("tx", True, lambda c: (0, len(STATES) - 1)),
    "permit": ("tx", True, lambda c: (0, 1)),
    "approved_counter": ("tx", True, lambda c: (0, 14)),
    "fault_reason": ("tx", True, lambda c: (0, len(REASONS) - 1)),
}


def _cycle(cal, timeout_of, cycle_ms):
    return cycle_ms


MECHANISMS = {
    "command_timeout": lambda cal, timeout_of, cycle_ms: timeout_of("cmd_status"),
    "request_limit": _cycle,
    "request_outside_braking": _cycle,
    "braking_justification": _cycle,
}


def check(cal: dict) -> list[str]:
    positive = ("partial_decel_mps2", "full_decel_mps2", "request_tolerance_mps2", "partial_brake_ttc_s",
                "full_brake_ttc_s", "ttc_margin_s", "max_target_range_m", "lane_half_width_m")
    rules = [
        (all(cal[k] > 0 for k in positive), "decelerations, tolerances, TTC values and ranges must be positive"),
        (cal["lane_margin_m"] >= 0, "lane margin must not be negative"),
        (cal["partial_decel_mps2"] < cal["full_decel_mps2"], "need partial decel < full decel"),
        (cal["full_brake_ttc_s"] < cal["partial_brake_ttc_s"], "need full brake TTC < partial brake TTC"),
        (cal["partial_status_code"] in (0, 1, 2, 3, 4) and cal["full_status_code"] in (0, 1, 2, 3, 4)
         and cal["partial_status_code"] != cal["full_status_code"], "status codes must be distinct codes 0..4"),
    ]
    return [why for ok, why in rules if not ok]


def derive(cal: dict, bound: dict, cycle_ms: int) -> tuple[dict, list[str]]:
    return {"reasons": REASONS}, []
