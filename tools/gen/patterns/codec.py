"""Message/signal IR and codec rules shared by patterns (GEN-030..GEN-036)."""
from __future__ import annotations

import math
from dataclasses import dataclass

from tools.gen.errors import Diagnostics

E2E_P4_HEADER = 12
SOMEIP_HEADER = 16
_EPS = 1e-9

_TYPES = {
    "uint8": (8, False), "int8": (8, True), "uint16": (16, False),
    "int16": (16, True), "uint32": (32, False), "int32": (32, True),
}


@dataclass(frozen=True)
class Signal:
    name: str  # snake_case C++ field
    cname: str  # CamelCase enumerator
    dbc_name: str | None
    start_bit: int
    length: int
    big_endian: bool
    signed: bool
    factor: float
    offset: float
    min: float
    max: float
    unit: str
    sil_default: float | None
    satisfies: tuple[str, ...]
    raw_type_min: int
    raw_type_max: int
    raw_lo: int  # raw bounds of the physical range (inclusive)
    raw_hi: int


@dataclass(frozen=True)
class Message:
    name: str
    snake: str
    transport: str
    payload_length: int
    signals: tuple[Signal, ...]
    someip: dict | None
    can_id: int | None
    e2e: dict | None
    satisfies: tuple[str, ...]
    source: str


def to_camel(snake: str) -> str:
    return "".join(p[:1].upper() + p[1:] for p in snake.split("_") if p)


def to_snake(camel: str) -> str:
    out = []
    for i, ch in enumerate(camel):
        if ch.isupper() and i > 0 and (not camel[i - 1].isupper() or (i + 1 < len(camel) and camel[i + 1].islower())):
            out.append("_")
        out.append(ch.lower())
    return "".join(out)


def bit_positions(start_bit: int, length: int, big_endian: bool) -> list[int]:
    """Same walk as ea::codec (runtime/src/codec/bits.cpp)."""
    if not big_endian:
        return [start_bit + i for i in range(length)]
    pos, out = start_bit, []
    for i in range(length):
        out.append(pos)
        if i + 1 < length:
            pos = pos + 15 if pos % 8 == 0 else pos - 1
    return out


def build_message(doc: dict, display: str, diags: Diagnostics) -> Message | None:
    name = doc["name"]
    payload = doc["payload_length"]
    signals: list[Signal] = []
    seen: set[str] = set()
    for s in doc["signals"]:
        if s["name"] in seen:
            diags.error("GEN-030", display, f"{name}: duplicate signal '{s['name']}'")
            continue
        seen.add(s["name"])
        if "byte_offset" in s:
            length, signed = _TYPES[s["type"]]
            start, big = s["byte_offset"] * 8 + 7, True
        else:
            length, signed, start, big = s["length"], s["signed"], s["start_bit"], s["byte_order"] == "big_endian"
        tmin = -(1 << (length - 1)) if signed else 0
        tmax = (1 << (length - 1)) - 1 if signed else (1 << length) - 1
        factor, offset = float(s["factor"]), float(s["offset"])
        smin, smax = float(s["min"]), float(s["max"])
        where = f"{name}.{s['name']}"
        if smin > smax:
            diags.error("GEN-033", display, f"{where}: min {smin} > max {smax}")
            continue
        lo = math.ceil((smin - offset) / factor - _EPS)
        hi = math.floor((smax - offset) / factor + _EPS)
        if lo < tmin or hi > tmax or lo > hi:
            diags.error("GEN-033", display,
                        f"{where}: range [{smin}, {smax}] needs raw [{lo}, {hi}], {length}-bit "
                        f"{'signed' if signed else 'unsigned'} holds [{tmin}, {tmax}]")
            continue
        default = s.get("sil_default")
        if default is not None and not (smin <= float(default) <= smax):
            diags.error("GEN-034", display, f"{where}: sil_default {default} outside [{smin}, {smax}]")
        positions = bit_positions(start, length, big)
        if min(positions) < 0 or max(positions) >= payload * 8:
            diags.error("GEN-031", display, f"{where}: bits {min(positions)}..{max(positions)} outside {payload}-byte payload")
            continue
        signals.append(Signal(
            name=s["name"], cname=to_camel(s["name"]), dbc_name=s.get("dbc_name"), start_bit=start, length=length,
            big_endian=big, signed=signed, factor=factor, offset=offset, min=smin, max=smax, unit=s["unit"],
            sil_default=None if default is None else float(default), satisfies=tuple(s.get("satisfies", [])),
            raw_type_min=tmin, raw_type_max=tmax, raw_lo=lo, raw_hi=hi,
        ))

    transport = doc["transport"]
    if transport == "someip" and "someip" not in doc:
        diags.error("GEN-035", display, f"{name}: transport someip requires a 'someip' block")
    if transport == "can" and ("someip" in doc or "can_id" not in doc):
        diags.error("GEN-035", display, f"{name}: transport can requires 'can_id' and no 'someip' block")
    if transport == "someip" and "can_id" in doc:
        diags.error("GEN-035", display, f"{name}: transport someip must not have a 'can_id'")

    occupied: dict[int, str] = {}
    e2e = doc.get("e2e")
    if e2e is not None:
        end = e2e["offset"] + E2E_P4_HEADER
        if end > payload or payload > 0xFFFF:
            diags.error("GEN-036", display, f"{name}: E2E header [{e2e['offset']}, {end}) outside {payload}-byte payload")
        for b in range(e2e["offset"] * 8, end * 8):
            occupied[b] = "E2E header"
    for sig in signals:
        for b in bit_positions(sig.start_bit, sig.length, sig.big_endian):
            if b in occupied:
                diags.error("GEN-032", display, f"{name}.{sig.name}: bit {b} overlaps {occupied[b]}")
                break
            occupied[b] = sig.name
    return Message(
        name=name, snake=to_snake(name), transport=transport, payload_length=payload, signals=tuple(signals),
        someip=doc.get("someip"), can_id=doc.get("can_id"), e2e=e2e, satisfies=tuple(doc.get("satisfies", [])),
        source=doc.get("source", display),
    )


def codec_vectors(messages) -> list[dict]:
    """Test vectors for the generated back-to-back codec test (common/codec_test.cpp.j2).

    For every message: the signals at the low, high and middle raw value of their physical
    range, the expected raw bit pattern of each, and the values just outside the range (for
    encode in the physical domain, for decode as raw bits when the field can hold them)."""
    out = []
    for m in messages:
        sigs = []
        for s in m.signals:
            def bits(raw: int, s=s) -> int:
                return raw & ((1 << s.length) - 1)

            raws = {"lo": s.raw_lo, "hi": s.raw_hi, "mid": (s.raw_lo + s.raw_hi) // 2}
            sigs.append({
                "name": s.name, "start": s.start_bit, "length": s.length,
                "order": "kBigEndian" if s.big_endian else "kLittleEndian", "factor": s.factor, "offset": s.offset,
                "raw": raws, "bits": {k: bits(v) for k, v in raws.items()},
                "phys": {k: v * s.factor + s.offset for k, v in raws.items()},
                "above": (s.raw_hi + 1) * s.factor + s.offset, "below": (s.raw_lo - 1) * s.factor + s.offset,
                "raw_above": bits(s.raw_hi + 1) if s.raw_hi + 1 <= s.raw_type_max else None,
                "raw_below": bits(s.raw_lo - 1) if s.raw_lo - 1 >= s.raw_type_min else None,
            })
        out.append({"name": m.name, "snake": m.snake, "length": m.payload_length, "signals": sigs})
    return out
