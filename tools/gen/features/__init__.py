"""Feature plug-ins for the can_function pattern.

A feature module provides (see docs/extending.md):
    NAME            short name, also the folder name under templates/features/ and sil/steps/features/
    STATES          state names; the code of a state is its index (sent in the 'status' signal)
    INTERFACE       {signal: (direction 'rx'|'tx', discrete: bool, range_fn(cal) -> (lo, hi))}
    CALIBRATION     {key: 'number' | 'list'}
    OBSERVER        (header, C++ type) of the plant observer that decodes the feature's output
    check(cal)      -> list of messages for inconsistent calibration (reported as GEN-063)
    derive(cal, bound, cycle_ms) -> (template variables, list of GEN-063 messages)
Features are discovered by module name in sorted order (deterministic).
"""
from __future__ import annotations

import importlib
import pkgutil
from types import ModuleType


def load_features() -> dict[str, ModuleType]:
    out: dict[str, ModuleType] = {}
    for info in sorted(pkgutil.iter_modules(__path__), key=lambda i: i.name):
        if info.name.startswith("_"):
            continue
        mod = importlib.import_module(f"{__name__}.{info.name}")
        out[mod.NAME] = mod
    return out
