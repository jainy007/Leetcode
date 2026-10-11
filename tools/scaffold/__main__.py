"""Scaffold a new can_function feature that builds and passes its own SIL immediately.

    python3 -m tools.scaffold feature <name> [--component NameController] [--can-id 800] [--root DIR]

Creates (and registers in ea_projects.yaml and sil/CMakeLists.txt):
    tools/gen/features/<name>.py                      feature module: states, interface, calibration
    templates/features/<name>/logic.{hpp,cpp}.j2      C++ logic (Inputs -> Outputs)
    sil/steps/features/<name>.yaml                    feature observation steps
    sil/plant/include/ea/plant/<name>_{can,observer}.hpp, sil/plant/src/<name>_observer.cpp
    examples/<name>/{requirements.yaml, model.yaml, interfaces/<name>.dbc, features/basic.feature, mutations.yaml}
The skeleton feature is minimal on purpose (ACTIVE above an activation speed, FAULT on input
timeout); replace it step by step, keeping `ctest` green. See docs/extending.md.
"""
from __future__ import annotations

import argparse
import pathlib
import re
import sys
from string import Template

from tools.gen.render import REPO_ROOT

FEATURE_PY = Template('''"""Feature '$name': TODO one-line description."""
from __future__ import annotations

NAME = "$name"
# State codes = list index; index 0 is the state before the first cycle. Sent in 'status'.
STATES = ["STANDBY", "ACTIVE", "FAULT"]
OBSERVER = ("ea/plant/${name}_observer.hpp", "ea::plant::${Camel}Observer")

CALIBRATION = {
    "activation_speed_kph": "number",
    "max_speed_kph": "number",
}

# signal: (direction, discrete, lambda cal: (lo, hi) physical range the bound DBC signal must cover)
INTERFACE = {
    "ego_speed": ("rx", False, lambda c: (0, c["max_speed_kph"])),
    "status": ("tx", True, lambda c: (0, len(STATES) - 1)),
}


def check(cal: dict) -> list[str]:
    rules = [
        (0 < cal["activation_speed_kph"] < cal["max_speed_kph"], "need 0 < activation speed < max speed"),
    ]
    return [why for ok, why in rules if not ok]


def derive(cal: dict, bound: dict, cycle_ms: int) -> tuple[dict, list[str]]:
    return {}, []
''')

LOGIC_HPP = Template('''#pragma once

#include <cstdint>

#include "{{ inc }}/{{ snake }}_types.hpp"

namespace {{ ns }} {

// Feature '$name' logic. TODO: describe.
class {{ component.name }}Logic {
public:
    static constexpr double kActivationSpeedKph = {{ cal.activation_speed_kph | cpp_double }};

    void step(const Inputs& in, Outputs& out) noexcept;
    State state() const noexcept { return state_; }

private:
    State state_ = State::kStandby;
};

}  // namespace {{ ns }}
''')

LOGIC_CPP = Template('''#include "{{ inc }}/{{ snake }}_logic.hpp"

namespace {{ ns }} {

void {{ component.name }}Logic::step(const Inputs& in, Outputs& out) noexcept {
    if (!in.all_ok) {
        state_ = State::kFault;
    } else {
        state_ = (in.ego_speed >= kActivationSpeedKph) ? State::kActive : State::kStandby;
    }
    out.status = static_cast<double>(static_cast<std::uint8_t>(state_));
}

}  // namespace {{ ns }}
''')

STEPS = Template('''# Step vocabulary of feature '$name' (observations via ea::plant::${Camel}Observer,
# reached as sil_.feature()). Combined with sil/steps/can_function.yaml.
schema_version: 1
pattern: $name
steps:
  - id: $PREFIX-S01
    keywords: [then]
    pattern: '^the $UPPER state is (?P<s>STANDBY|ACTIVE|FAULT)$$'
    types: {s: 'enum:[STANDBY, ACTIVE, FAULT]'}
    cpp: 'EXPECT_EQ(sil_.feature().last_status(), ea::plant::${Camel}Status::k{{ s | camel }});'
  - id: $PREFIX-S02
    keywords: [then]
    pattern: '^every command frame passed the E2E check$$'
    cpp: 'EXPECT_GT(sil_.feature().command_frames(), 0U); EXPECT_EQ(sil_.feature().command_e2e_errors(), 0U);'
''')

CAN_HPP = Template('''// Hand-written layout of the $name output message ${Camel}Command (examples/$name/interfaces/$name.dbc).
#pragma once

#include "ea/plant/vehicle_can.hpp"

namespace ea::plant::can {

inline constexpr std::uint32_t k${Camel}CommandId = ${can_id}U;
inline constexpr e2e::P11Config k${Camel}CommandE2e{${data_id}U, 0U, 8U, 8U, 2U};

namespace ${name}_command {
inline constexpr Sig k${Camel}Status{12U, 3U, false, 1.0};
}  // namespace ${name}_command

}  // namespace ea::plant::can
''')

OBS_HPP = Template('''// Observer for feature '$name': decodes ${Camel}Command with the plant's own layout and E2E check.
#pragma once

#include <cstdint>

#include "ea/e2e/p11.hpp"
#include "ea/plant/plant.hpp"

namespace ea::plant {

// State codes as specified by the requirements (not taken from generated code).
enum class ${Camel}Status : std::uint8_t { kStandby = 0, kActive = 1, kFault = 2, kInvalid = 255 };

class ${Camel}Observer final : public FeatureObserver {
public:
    ${Camel}Observer() noexcept;
    void observe(const WorldView& world, Span<const BusFrame> frames) noexcept override;
    bool longitudinal_request(double& accel_mps2) const noexcept override;
    std::int32_t status_code() const noexcept override { return static_cast<std::int32_t>(status_); }

    ${Camel}Status last_status() const noexcept { return status_; }
    std::uint32_t command_frames() const noexcept { return frames_; }
    std::uint32_t command_e2e_errors() const noexcept { return e2e_errors_; }

private:
    e2e::P11Checker rx_;
    ${Camel}Status status_ = ${Camel}Status::kInvalid;
    std::uint32_t frames_ = 0U;
    std::uint32_t e2e_errors_ = 0U;
};

}  // namespace ea::plant
''')

OBS_CPP = Template('''#include "ea/plant/${name}_observer.hpp"

#include "ea/plant/${name}_can.hpp"
#include "ea/plant/signals.hpp"

namespace ea::plant {

${Camel}Observer::${Camel}Observer() noexcept : rx_(can::k${Camel}CommandE2e) {}

void ${Camel}Observer::observe(const WorldView& /*world*/, Span<const BusFrame> frames) noexcept {
    for (const BusFrame& bf : frames) {
        if (bf.can_id != can::k${Camel}CommandId) {
            continue;
        }
        const ConstByteSpan f = bf.payload();
        ++frames_;
        if (!e2e::is_usable(rx_.check(f))) {
            ++e2e_errors_;
            continue;
        }
        const std::int64_t st = get_raw(f, can::${name}_command::k${Camel}Status);
        status_ = (st <= 2) ? static_cast<${Camel}Status>(st) : ${Camel}Status::kInvalid;
    }
}

bool ${Camel}Observer::longitudinal_request(double& /*accel_mps2*/) const noexcept {
    return false;  // TODO: return the commanded acceleration if the feature actuates
}

}  // namespace ea::plant
''')

REQUIREMENTS = Template('''# TODO: requirements of feature '$name' (EARS). Expected values in Gherkin must come from here.
schema_version: 1
component: $Component
requirements:
  - id: REQ-$PREFIX-001
    asil: QM
    ears: event
    text: >-
      When the ego speed reaches 30 km/h, the $Component shall enter ACTIVE.
    verification: [sil]
  - id: REQ-$PREFIX-002
    asil: QM
    ears: unwanted
    text: >-
      If no valid EgoState frame is received for 60 ms, then the $Component shall enter FAULT.
    verification: [sil]
  - id: REQ-$PREFIX-003
    asil: QM
    ears: ubiquitous
    text: >-
      The $Component shall send the ${Camel}Command message every 20 ms with E2E protection.
    verification: [sil]
''')

MODEL = Template('''# Model for feature '$name' (pattern can_function).
schema_version: 1
pattern: can_function
feature: $name

component:
  name: $Component
  namespace: adas::$name
  asil: QM
  profile: asil

inputs:
  requirements: requirements.yaml
  features: [features/basic.feature]
  dbc: [interfaces/$name.dbc]

cycle_ms: 20

rx:
  - {message: RadarObject, period_ms: 40, timeout_ms: 120,
     e2e: {data_id: 0x0200, crc: Crc, counter: Counter, max_delta_counter: 3}}
  - {message: EgoState, period_ms: 20, timeout_ms: 60, satisfies: [REQ-$PREFIX-002],
     e2e: {data_id: 0x0210, crc: Crc, counter: Counter, max_delta_counter: 3}}
  - {message: DriverInput, period_ms: 20, timeout_ms: 60,
     e2e: {data_id: 0x0220, crc: Crc, counter: Counter, max_delta_counter: 3}}

tx:
  - {message: ${Camel}Command, satisfies: [REQ-$PREFIX-003],
     e2e: {data_id: $data_id, crc: Crc, counter: Counter, max_delta_counter: 2}}

bindings:
  ego_speed: EgoState.VehicleSpeed
  status: ${Camel}Command.${Camel}Status

logic:
  satisfies: [REQ-$PREFIX-001, REQ-$PREFIX-002]

calibration:
  activation_speed_kph: {value: 30, satisfies: [REQ-$PREFIX-001]}
  max_speed_kph: {value: 250}
''')

FEATURE_FILE = Template('''# Steps come from sil/steps/can_function.yaml and sil/steps/features/$name.yaml only.
Feature: $Component basic behaviour

  @REQ-$PREFIX-001
  Scenario Outline: ACTIVE from the activation speed on
    Given the ego vehicle drives at <speed> km/h
    When 0.2 s pass
    Then the $UPPER state is <state>

    Examples:
      | speed | state   |
      | 29    | STANDBY |
      | 30    | ACTIVE  |

  @REQ-$PREFIX-002
  Scenario Outline: EgoState timeout leads to FAULT
    Given the ego vehicle drives at 50 km/h
    When 1 s passes
    And the ego state sender pauses for <n> frames
    And 0.06 s pass
    Then the $UPPER state is <state>

    Examples:
      | n | state  |
      | 2 | ACTIVE |
      | 3 | FAULT  |

  @REQ-$PREFIX-003
  Scenario: Command frames are E2E protected
    Given the ego vehicle drives at 50 km/h
    When 1 s passes
    Then every command frame passed the E2E check
''')

MUTATIONS = Template('''# Deliberate defects the chain must detect (python3 -m tools.mutate $name).
mutations:
  - {name: "activation speed 30 -> 31 km/h", file: examples/$name/model.yaml, old: "activation_speed_kph: {value: 30", new: "activation_speed_kph: {value: 31"}
  - {name: "FAULT never entered", file: templates/features/$name/logic.cpp.j2, old: "if (!in.all_ok) {", new: "if (false) {"}
''')


def dbc_text(name: str, camel: str, can_id: int) -> str:
    base = (REPO_ROOT / "examples" / "acc" / "interfaces" / "acc.dbc").read_text(encoding="utf-8")
    shared = base[: base.index("BO_ 768 AccCommand")]
    node = name.upper()
    shared = shared.replace("BU_: RADAR VEHICLE ACC", f"BU_: RADAR VEHICLE {node}").replace(" ACC\n", f" {node}\n")
    return shared + (f'BO_ {can_id} {camel}Command: 8 {node}\n'
                     f' SG_ Crc : 0|8@1+ (1,0) [0|255] "" VEHICLE\n'
                     f' SG_ Counter : 8|4@1+ (1,0) [0|15] "" VEHICLE\n'
                     f' SG_ {camel}Status : 12|3@1+ (1,0) [0|2] "" VEHICLE\n\n'
                     f'CM_ SG_ {can_id} {camel}Status "0=STANDBY 1=ACTIVE 2=FAULT";\n')


def scaffold_feature(root: pathlib.Path, name: str, component: str, can_id: int) -> list[pathlib.Path]:
    camel = "".join(p.capitalize() for p in name.split("_"))
    prefix = re.sub(r"[^A-Z]", "", name.upper()) or "FEAT"
    data_id = f"0x{can_id:04X}"
    subst = {"name": name, "Camel": camel, "Component": component, "PREFIX": prefix,
             "UPPER": name.upper(), "can_id": can_id, "data_id": data_id}
    files = {
        f"tools/gen/features/{name}.py": FEATURE_PY,
        f"templates/features/{name}/logic.hpp.j2": LOGIC_HPP,
        f"templates/features/{name}/logic.cpp.j2": LOGIC_CPP,
        f"sil/steps/features/{name}.yaml": STEPS,
        f"sil/plant/include/ea/plant/{name}_can.hpp": CAN_HPP,
        f"sil/plant/include/ea/plant/{name}_observer.hpp": OBS_HPP,
        f"sil/plant/src/{name}_observer.cpp": OBS_CPP,
        f"examples/{name}/requirements.yaml": REQUIREMENTS,
        f"examples/{name}/model.yaml": MODEL,
        f"examples/{name}/features/basic.feature": FEATURE_FILE,
        f"examples/{name}/mutations.yaml": MUTATIONS,
    }
    existing = [rel for rel in files if (root / rel).exists()] + \
               ([f"examples/{name}/interfaces/{name}.dbc"] if (root / f"examples/{name}/interfaces/{name}.dbc").exists() else [])
    if existing:
        raise SystemExit(f"refusing to overwrite existing files: {existing}")
    written = []
    for rel, tpl in files.items():
        path = root / rel
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(tpl.substitute(subst), encoding="utf-8")
        written.append(path)
    dbc = root / f"examples/{name}/interfaces/{name}.dbc"
    dbc.parent.mkdir(parents=True, exist_ok=True)
    dbc.write_text(dbc_text(name, camel, can_id), encoding="utf-8")
    written.append(dbc)

    projects = root / "ea_projects.yaml"
    entry = f"  - {{name: {name}, model: examples/{name}/model.yaml, output: generated/{name}}}\n"
    if projects.is_file() and entry not in projects.read_text(encoding="utf-8"):
        projects.write_text(projects.read_text(encoding="utf-8") + entry, encoding="utf-8")
    cmake = root / "sil" / "CMakeLists.txt"
    if cmake.is_file():
        text = cmake.read_text(encoding="utf-8")
        src = f"    plant/src/{name}_observer.cpp"
        if src not in text:
            text = text.replace("    plant/src/plant.cpp\n", f"    plant/src/plant.cpp\n{src}\n", 1)
            cmake.write_text(text, encoding="utf-8")
    return written


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(prog="python3 -m tools.scaffold")
    sub = ap.add_subparsers(dest="kind", required=True)
    f = sub.add_parser("feature", help="new can_function feature")
    f.add_argument("name", help="snake_case feature name, e.g. lka")
    f.add_argument("--component", help="C++ class name (default: <Name>Controller)")
    f.add_argument("--can-id", type=int, default=800, help="CAN id of the output message (default 800)")
    f.add_argument("--root", type=pathlib.Path, default=REPO_ROOT)
    args = ap.parse_args(argv)
    if not re.fullmatch(r"[a-z][a-z0-9_]*", args.name):
        print("feature name must be snake_case", file=sys.stderr)
        return 2
    component = args.component or "".join(p.capitalize() for p in args.name.split("_")) + "Controller"
    for p in scaffold_feature(args.root, args.name, component, args.can_id):
        print(f"created {p.relative_to(args.root)}")
    print("registered in ea_projects.yaml and sil/CMakeLists.txt")
    print("next: python3 -m tools.gen && cmake --build --preset linux-host && ctest --preset linux-host")
    return 0


if __name__ == "__main__":
    sys.exit(main())
