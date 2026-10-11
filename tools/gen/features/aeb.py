"""Feature 'aeb': forward collision warning + two-stage autonomous emergency braking (TTC based)."""
from __future__ import annotations

NAME = "aeb"
STATES = ["STANDBY", "WARNING", "PARTIAL_BRAKE", "FULL_BRAKE", "FAULT"]
OBSERVER = ("ea/plant/aeb_observer.hpp", "ea::plant::AebObserver")

CALIBRATION = {
    "min_speed_kph": "number", "max_speed_kph": "number", "max_target_range_m": "number",
    "lane_half_width_m": "number", "min_closing_speed_mps": "number", "fcw_ttc_s": "number",
    "warning_release_ttc_s": "number", "partial_brake_ttc_s": "number", "full_brake_ttc_s": "number",
    "min_warning_time_s": "number", "partial_decel_mps2": "number", "full_decel_mps2": "number",
    "kickdown_pedal_pct": "number", "standstill_speed_kph": "number",
}

INTERFACE = {
    "target_valid": ("rx", True, lambda c: (0, 1)),
    "target_range": ("rx", False, lambda c: (0, c["max_target_range_m"])),
    "target_range_rate": ("rx", False, lambda c: (-c["max_speed_kph"] / 3.6, c["max_speed_kph"] / 3.6)),
    "target_lateral": ("rx", False, lambda c: (-2 * c["lane_half_width_m"], 2 * c["lane_half_width_m"])),
    "ego_speed": ("rx", False, lambda c: (0, c["max_speed_kph"])),
    "brake_pedal": ("rx", True, lambda c: (0, 1)),
    "accel_pedal": ("rx", False, lambda c: (0, 100)),
    "status": ("tx", True, lambda c: (0, len(STATES) - 1)),
    "fcw_warning": ("tx", True, lambda c: (0, 1)),
    "brake_request": ("tx", False, lambda c: (0, c["full_decel_mps2"])),
}


def check(cal: dict) -> list[str]:
    rules = [
        (0 < cal["full_brake_ttc_s"] < cal["partial_brake_ttc_s"] < cal["fcw_ttc_s"] < cal["warning_release_ttc_s"],
         "need 0 < full brake TTC < partial brake TTC < FCW TTC < warning release TTC"),
        (0 < cal["partial_decel_mps2"] < cal["full_decel_mps2"], "need 0 < partial decel < full decel"),
        (0 <= cal["standstill_speed_kph"] < cal["min_speed_kph"] < cal["max_speed_kph"],
         "need 0 <= standstill speed < min speed < max speed"),
        (all(cal[k] > 0 for k in ("max_target_range_m", "lane_half_width_m", "min_closing_speed_mps",
                                  "min_warning_time_s")), "ranges, closing speed and warning time must be positive"),
        (0 < cal["kickdown_pedal_pct"] <= 100, "kickdown threshold must lie in (0, 100] %"),
    ]
    return [why for ok, why in rules if not ok]


def derive(cal: dict, bound: dict, cycle_ms: int) -> tuple[dict, list[str]]:
    """The minimum warning time is counted in whole cycles."""
    errors: list[str] = []
    cycles = cal["min_warning_time_s"] * 1000.0 / cycle_ms
    if abs(cycles - round(cycles)) > 1e-6:
        errors.append(f"min_warning_time_s must be a whole number of {cycle_ms} ms cycles")
    return {"min_warning_cycles": int(round(cycles))}, errors
