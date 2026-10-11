"""Feature 'acc': adaptive cruise control (longitudinal, 30..180 km/h, no stop-and-go)."""
from __future__ import annotations

NAME = "acc"
STATES = ["OFF", "STANDBY", "ACTIVE", "OVERRIDE", "FAULT"]
# Hand-written plant layout of the output message: (header, signal namespace, id, E2E config).
PLANT_TABLES = ("ea/plant/acc_can.hpp", "ea::plant::can::acc_command", "ea::plant::can::kAccCommandId", "ea::plant::can::kAccCommandE2e")
OBSERVER = ("ea/plant/acc_observer.hpp", "ea::plant::AccObserver")

CALIBRATION = {
    "min_activation_speed_kph": "number", "disengage_speed_kph": "number", "max_set_speed_kph": "number",
    "set_speed_step_kph": "number", "time_gaps_s": "list", "standstill_distance_m": "number",
    "max_target_range_m": "number", "lane_half_width_m": "number", "accel_max_mps2": "number",
    "decel_max_mps2": "number", "jerk_increase_max_mps3": "number", "jerk_decrease_max_mps3": "number",
    "override_pedal_pct": "number", "speed_gain_per_s": "number", "gap_gain_per_s2": "number",
    "range_rate_gain_per_s": "number",
}

INTERFACE = {
    "target_valid": ("rx", True, lambda c: (0, 1)),
    "target_range": ("rx", False, lambda c: (0, c["max_target_range_m"])),
    "target_range_rate": ("rx", False, lambda c: (-50, 50)),
    "target_lateral": ("rx", False, lambda c: (-2 * c["lane_half_width_m"], 2 * c["lane_half_width_m"])),
    "ego_speed": ("rx", False, lambda c: (0, c["max_set_speed_kph"])),
    "main_switch": ("rx", True, lambda c: (0, 1)),
    "set_button": ("rx", True, lambda c: (0, 1)),
    "resume_button": ("rx", True, lambda c: (0, 1)),
    "cancel_button": ("rx", True, lambda c: (0, 1)),
    "speed_up_button": ("rx", True, lambda c: (0, 1)),
    "speed_down_button": ("rx", True, lambda c: (0, 1)),
    "gap_setting": ("rx", True, lambda c: (1, len(c["time_gaps_s"]))),
    "brake_pedal": ("rx", True, lambda c: (0, 1)),
    "accel_pedal": ("rx", False, lambda c: (0, 100)),
    "status": ("tx", True, lambda c: (0, len(STATES) - 1)),
    "accel_request": ("tx", False, lambda c: (0, c["accel_max_mps2"])),
    "brake_request": ("tx", False, lambda c: (0, c["decel_max_mps2"])),
    "steering_request": ("tx", False, lambda c: (0, 0)),
    "set_speed": ("tx", False, lambda c: (0, c["max_set_speed_kph"])),
    "time_gap_setting": ("tx", True, lambda c: (1, len(c["time_gaps_s"]))),
}


def check(cal: dict) -> list[str]:
    gaps = cal["time_gaps_s"]
    positive = ("accel_max_mps2", "decel_max_mps2", "jerk_increase_max_mps3", "jerk_decrease_max_mps3",
                "speed_gain_per_s", "gap_gain_per_s2", "range_rate_gain_per_s", "max_target_range_m",
                "lane_half_width_m", "standstill_distance_m")
    rules = [
        (all(g > 0 for g in gaps) and all(a < b for a, b in zip(gaps, gaps[1:])), "time gaps must be positive and ascending"),
        (0 < cal["disengage_speed_kph"] < cal["min_activation_speed_kph"] <= cal["max_set_speed_kph"],
         "need 0 < disengage < min activation <= max set speed"),
        (cal["set_speed_step_kph"] > 0, "set speed step must be positive"),
        (all(cal[k] > 0 for k in positive), "limits and gains must be positive"),
        (0 < cal["override_pedal_pct"] < 100, "override threshold must lie in (0, 100) %"),
    ]
    return [why for ok, why in rules if not ok]


def derive(cal: dict, bound: dict, cycle_ms: int) -> tuple[dict, list[str]]:
    """Jerk limits become whole steps of the request resolution per cycle (exact on the bus)."""
    errors: list[str] = []
    res = bound["accel_request"].factor
    if bound["brake_request"].factor != res:
        errors.append("accel_request and brake_request need the same resolution")
    jerk = {"resolution": res}
    for key in ("jerk_increase_max_mps3", "jerk_decrease_max_mps3"):
        steps = cal[key] * cycle_ms / 1000.0 / res
        if abs(steps - round(steps)) > 1e-6 or round(steps) < 1:
            errors.append(f"{key} * cycle must be a whole number (>= 1) of {res} m/s2 steps")
        jerk[key] = int(round(steps))
    return {"jerk": jerk, "default_gap": (len(cal["time_gaps_s"]) + 1) // 2}, errors
