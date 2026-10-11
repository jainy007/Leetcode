"""Safety analysis used by pattern can_system: ASIL determination, kinematic FTTI derivation and
fault tree minimal cut sets. Deterministic and rule-based like the rest of the generator.

ASIL (ISO 26262-3:2018, table 4): S0, E0 or C0 gives QM; otherwise the class indices add up and
10 -> D, 9 -> C, 8 -> B, 7 -> A, below -> QM. This reproduces table 4 cell by cell.

FTTI models. Each returns the longest fault duration T in seconds (from the fault until the
actuator enters its safe state) for which the hazardous event (a collision) is still avoided. The
models are point-mass kinematics with the assumptions stated in their parameters. The search is a
bisection with 1 ms resolution, integrated with a fixed 2 ms step.

  rear_end_by_follower   ego brakes with fault_decel for T, then ramps the braking out at
                         ramp_out_mps3. A follower at headway_s reacts after reaction_s (counted
                         from the start of the ego braking) and brakes at follower_decel.
  approach_with_accel    ego approaches a vehicle (closing_kph, gap = ttc_s * closing speed, or
                         a following gap when closing_kph is 0) and accelerates with fault_accel
                         for T. At T the safe state cuts the acceleration and requests a takeover;
                         the driver brakes at driver_decel after reaction_s.
"""
from __future__ import annotations

import itertools

SEVERITY = ("S0", "S1", "S2", "S3")
EXPOSURE = ("E0", "E1", "E2", "E3", "E4")
CONTROLLABILITY = ("C0", "C1", "C2", "C3")
ASIL_ORDER = ("QM", "A", "B", "C", "D")

_DT = 0.002
_HORIZON = 60.0
_T_MAX = 10.0


def asil(s: str, e: str, c: str) -> str:
    si, ei, ci = SEVERITY.index(s), EXPOSURE.index(e), CONTROLLABILITY.index(c)
    if 0 in (si, ei, ci):
        return "QM"
    return {10: "D", 9: "C", 8: "B", 7: "A"}.get(si + ei + ci, "QM")


def asil_rank(level: str) -> int:
    """Rank of an ASIL, X(Y) counting as X (what the element itself is developed to)."""
    return ASIL_ORDER.index(level.split("(")[0])


# ---------------------------------------------------------------------------------------------
# Kinematic models: True if the collision is avoided for fault duration T.

def _rear_end(p: dict, t_fault: float) -> bool:
    v0 = p["speed_kph"] / 3.6
    x_ego, v_ego, a_ego = v0 * p["headway_s"], v0, 0.0
    x_fol, v_fol = 0.0, v0
    t = 0.0
    while t < _HORIZON:
        if t < t_fault:
            a_ego = p["fault_decel_mps2"]
        else:
            a_ego = max(0.0, a_ego - p["ramp_out_mps3"] * _DT)
        v_ego = max(0.0, v_ego - a_ego * _DT)
        if t >= p["reaction_s"]:
            v_fol = max(0.0, v_fol - p["follower_decel_mps2"] * _DT)
        x_ego += v_ego * _DT
        x_fol += v_fol * _DT
        t += _DT
        if x_fol >= x_ego:
            return False
        if t > t_fault and a_ego == 0.0 and t > p["reaction_s"] and v_fol <= v_ego:
            return True
    return True


def _approach(p: dict, t_fault: float) -> bool:
    v_ego = p["speed_kph"] / 3.6
    closing = p["closing_kph"] / 3.6
    v_lead = v_ego - closing
    gap = p["ttc_s"] * closing if closing > 0.0 else p.get("standstill_m", 5.0) + p["time_gap_s"] * v_ego
    t = 0.0
    while t < _HORIZON:
        if t < t_fault:
            a = p["fault_accel_mps2"]
        elif t >= t_fault + p["reaction_s"]:
            a = -p["driver_decel_mps2"]
        else:
            a = 0.0
        v_ego = max(0.0, v_ego + a * _DT)
        gap -= (v_ego - v_lead) * _DT
        t += _DT
        if gap <= 0.0:
            return False
        if t > t_fault + p["reaction_s"] and v_ego <= v_lead:
            return True
    return True


MODELS = {
    "rear_end_by_follower": (_rear_end, {"speed_kph", "headway_s", "fault_decel_mps2", "ramp_out_mps3",
                                         "reaction_s", "follower_decel_mps2"}),
    "approach_with_accel": (_approach, {"speed_kph", "closing_kph", "ttc_s", "time_gap_s", "fault_accel_mps2",
                                        "reaction_s", "driver_decel_mps2"}),
}


def derive_ftti_ms(model: str, params: dict) -> int | None:
    """Longest tolerable fault duration in whole ms (rounded down to 10 ms); None if the hazard
    occurs even without a fault (the situation is not a valid reference)."""
    fn = MODELS[model][0]
    if not fn(params, 0.0):
        return None
    if fn(params, _T_MAX):
        return int(_T_MAX * 1000)
    lo, hi = 0, int(_T_MAX * 1000)  # integer ms keep the bisection exact and deterministic
    while hi - lo > 1:
        mid = (lo + hi) // 2
        if fn(params, mid / 1000.0):
            lo = mid
        else:
            hi = mid
    return lo // 10 * 10


# ---------------------------------------------------------------------------------------------
# Fault tree: gates {and|or: [...]} over basic event ids.

def minimal_cut_sets(node, events: dict, gates: dict, _seen: tuple = ()) -> list[frozenset]:
    if isinstance(node, str):
        if node in events:
            return [frozenset([node])]
        if node in _seen:
            raise ValueError(f"cycle at gate '{node}'")
        return minimal_cut_sets(gates[node], events, gates, _seen + (node,))
    kind = "and" if "and" in node else "or"
    children = [minimal_cut_sets(c, events, gates, _seen) for c in node[kind]]
    if kind == "or":
        sets = [s for c in children for s in c]
    else:
        sets = [frozenset().union(*combo) for combo in itertools.product(*children)]
    unique = sorted(set(sets), key=lambda s: (len(s), sorted(s)))
    return [s for s in unique if not any(o < s for o in unique)]
