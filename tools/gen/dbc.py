"""Importer for the supported DBC subset (GEN-037): BO_ and SG_ lines with plain signals.

Converts each message into the model's message dictionary form (transport: can). Signal names
are converted from DBC CamelCase to snake_case for C++ fields; the original name is kept as
`dbc_name` (used in status mappings and Gherkin).
"""
from __future__ import annotations

import re

from tools.gen.errors import Diagnostics

_BO_RE = re.compile(r"^BO_\s+(\d+)\s+([A-Za-z_][A-Za-z0-9_]*)\s*:\s*(\d+)\s+([A-Za-z_][A-Za-z0-9_]*)$")
_SG_RE = re.compile(
    r"^SG_\s+([A-Za-z_][A-Za-z0-9_]*)\s*:\s*(\d+)\|(\d+)@([01])([+-])\s*"
    r"\(\s*([-+0-9.eE]+)\s*,\s*([-+0-9.eE]+)\s*\)\s*"
    r"\[\s*([-+0-9.eE]+)\s*\|\s*([-+0-9.eE]+)\s*\]\s*"
    r"\"([^\"]*)\"\s*(.*)$"
)
_SG_MUX_RE = re.compile(r"^SG_\s+[A-Za-z_][A-Za-z0-9_]*\s+(M|m\d+)\s*:")
_IGNORED = ("VERSION", "NS_", "BS_", "BU_", "CM_", "VAL_", "BA_", "BA_DEF_", "BA_DEF_DEF_", "VAL_TABLE_")


def snake(name: str) -> str:
    s = re.sub(r"(?<=[a-z0-9])([A-Z])", r"_\1", name)
    s = re.sub(r"(?<=[A-Z])([A-Z][a-z])", r"_\1", s)
    return s.lower()


def _number(text: str) -> float | int:
    value = float(text)
    return int(value) if value.is_integer() and "." not in text and "e" not in text.lower() else value


def parse(text: str, path: str, diags: Diagnostics) -> list[dict]:
    messages: list[dict] = []
    current: dict | None = None
    in_ns_block = False
    for lineno, raw in enumerate(text.split("\n"), start=1):
        line = raw.strip()
        if not line:
            in_ns_block = False
            continue
        if line.startswith("NS_"):
            in_ns_block = True
            continue
        if in_ns_block and not line.startswith(("BO_", "SG_", "BU_", "BS_")):
            continue  # indented symbol list of NS_
        in_ns_block = False
        if line.startswith("BO_ "):
            m = _BO_RE.match(line)
            if not m:
                diags.error("GEN-037", path, f"unsupported BO_ line: {line}", lineno)
                current = None
                continue
            can_id = int(m.group(1))
            if can_id & 0x80000000:
                diags.error("GEN-037", path, "extended CAN ids are not supported yet", lineno)
            current = {
                "name": m.group(2),
                "transport": "can",
                "can_id": can_id & 0x1FFFFFFF,
                "payload_length": int(m.group(3)),
                "signals": [],
                "source": f"{path}:{lineno}",
            }
            messages.append(current)
            continue
        if line.startswith("SG_ "):
            if _SG_MUX_RE.match(line):
                diags.error("GEN-037", path, "multiplexed signals are not supported", lineno)
                continue
            m = _SG_RE.match(line)
            if not m or current is None:
                diags.error("GEN-037", path, f"unsupported SG_ line: {line}", lineno)
                continue
            current["signals"].append(
                {
                    "name": snake(m.group(1)),
                    "dbc_name": m.group(1),
                    "start_bit": int(m.group(2)),
                    "length": int(m.group(3)),
                    "byte_order": "big_endian" if m.group(4) == "0" else "little_endian",
                    "signed": m.group(5) == "-",
                    "factor": _number(m.group(6)),
                    "offset": _number(m.group(7)),
                    "min": _number(m.group(8)),
                    "max": _number(m.group(9)),
                    "unit": m.group(10),
                }
            )
            continue
        if line.startswith("SIG_VALTYPE_"):
            diags.error("GEN-037", path, "float signal value types are not supported", lineno)
            continue
        if line.split()[0].rstrip(":") in _IGNORED or line.startswith(_IGNORED):
            continue
        diags.error("GEN-037", path, f"unsupported DBC construct: {line.split()[0]}", lineno)
    for msg in messages:
        if not msg["signals"]:
            diags.error("GEN-037", path, f"message {msg['name']} has no signals")
    return messages
