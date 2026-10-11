"""Negative tests: one mutation per rule. Each must be rejected with (at least) that rule.

Together with test_rule_coverage this demonstrates that every rule of the generator is effective
(tool qualification: validation of the software tool, ISO 26262-8 11.4.9)."""
from __future__ import annotations

import unittest
from unittest import mock

from tools.gen.patterns import gpu_stage
from tools.gen.tests.helpers import MutatedProject

R = "requirements.yaml"
M = "model.yaml"
F_GATE = "features/gate.feature"
F_GPU = "features/stage.feature"
DBC = "interfaces/gate_status.dbc"

# rule -> (example, list of mutations (kind, file, args...))
CASES: dict[str, list[tuple]] = {
    "GEN-001": [("gate", [("append", M, "pattern: input_gate\n")]),
                ("gate", [("replace", M, "period_ms: 20", "period_ms: .inf")])],
    "GEN-002": [("gate", [("replace", M, "  profile: asil\n", "  profile: asil\n  colour: red\n")])],
    "GEN-003": [("gate", [("replace", M, "features: [features/gate.feature]", "features: [features/missing.feature]")]),
                ("gate", [("symlink", "features/escape.feature", "/etc/hostname"),
                          ("replace", M, "features: [features/gate.feature]", "features: [features/escape.feature]")])],
    "GEN-010": [("gate", [("replace", R, "id: REQ-GATE-002", "id: REQ-GATE-001")])],
    "GEN-011": [("gate", [("replace", R, "The AdasInputGate shall report one alive", "It reports one alive")])],
    "GEN-012": [("gate", [("replace", R, "component: AdasInputGate", "component: OtherGate")])],
    "GEN-013": [("gate", [("replace", F_GATE, "@REQ-GATE-008 @REQ-GATE-009", "@REQ-GATE-008")])],
    "GEN-014": [("gate", [("replace", F_GATE, "@REQ-GATE-007", "@REQ-GATE-999")]),
                ("gate", [("replace", F_GATE, "  @REQ-GATE-007\n", "")])],
    "GEN-015": [("gate", [("replace", M, "satisfies: [REQ-GATE-009]", "satisfies: [REQ-GATE-009, REQ-GATE-999]")])],
    "GEN-016": [("gate", [("append", R, "  - id: REQ-GATE-010\n    asil: B\n    ears: ubiquitous\n"
                                        "    text: The AdasInputGate shall log faults.\n    verification: [review]\n")])],
    "GEN-020": [("gate", [("replace", F_GATE, "  @REQ-GATE-001 @REQ-GATE-002\n", "  Background:\n  @REQ-GATE-001 @REQ-GATE-002\n")])],
    "GEN-021": [("gate", [("replace", F_GATE, "    Then the state is SAFE\n\n  @REQ-GATE-006", "    Then the moon is full\n\n  @REQ-GATE-006")])],
    "GEN-022": [("gate", [("replace", F_GATE, "    Given a freshly started component\n    Then the state is INIT",
                           "    Given the state is INIT")])],
    "GEN-023": [("gate", [("replace", F_GATE, "Then the state is INIT", "Then the state is FLYING")]),
                ("gate", [("replace", F_GATE, "output signal nearest_distance equals 42.5 within 0.01\n    And the output signal nearest_rel_speed",
                           "output signal warp_speed equals 42.5 within 0.01\n    And the output signal nearest_rel_speed")])],
    "GEN-024": [("gate", [("replace", F_GATE, "Scenario: Corrupted frames do not start the gate",
                           "Scenario: First valid frame brings the gate into RUN with valid output")])],
    "GEN-025": [("gate", [("replace", F_GATE, "receives a frame with <signal> set to <value>", "receives a frame with <signal> set to <amount>")]),
                ("gpu", [("replace", F_GPU, "      | fc1_w  |\n      | fc1_b  |\n", "")])],
    "GEN-030": [("gate", [("replace", M, "{name: nearest_confidence,", "{name: object_count,")])],
    "GEN-031": [("gate", [("replace", M, "byte_offset: 17", "byte_offset: 18")])],
    "GEN-032": [("gate", [("replace", M, "byte_offset: 13", "byte_offset: 12")]),
                ("gate", [("replace", M, "byte_offset: 12", "byte_offset: 11")])],
    "GEN-033": [("gate", [("replace", M, "min: 0, max: 64,", "min: 0, max: 300,")]),
                ("gate", [("replace", M, "min: 0, max: 64,", "min: 10, max: 5,")])],
    "GEN-034": [("gate", [("replace", M, "sil_default: 3,", "sil_default: 99,")])],
    "GEN-035": [("gate", [("replace", M, "    someip: {service_id: 0x1234, event_id: 0x8001, interface_version: 1}\n", "")])],
    "GEN-036": [("gate", [("replace", M, "offset: 0, max_delta_counter", "offset: 10, max_delta_counter")])],
    "GEN-037": [("gate", [("replace", DBC, " SG_ E2eErrorCount : 31|8@0+", " SG_ E2eErrorCount m1 : 31|8@0+")])],
    "GEN-040": [("gate", [("replace", M, "  input: ObjectSummary", "  input: GateStatus")])],
    "GEN-041": [("gate", [("replace", M, "{name: recovered,", "{name: timeout,")])],
    "GEN-042": [("gate", [("replace", M, "{from: INIT, to: RUN,", "{from: INIT, to: FLYING,")]),
                ("gate", [("replace", M, "on: recovered,", "on: never,")])],
    "GEN-043": [("gate", [("replace", M, "states: [INIT, RUN, DEGRADED, SAFE]", "states: [INIT, RUN, DEGRADED, SAFE, ORPHAN]"),
                          ("replace", M, "      - {from: DEGRADED, to: RUN,", "      - {from: ORPHAN, to: SAFE, on: timeout}\n      - {from: DEGRADED, to: RUN,")])],
    "GEN-044": [("gate", [("replace", M, "{from: RUN, to: DEGRADED, on: range_fault,", "{from: RUN, to: DEGRADED, on: e2e_fault,")]),
                ("gate", [("replace", M, "latched: [SAFE]", "latched: []")])],
    "GEN-045": [("gate", [("replace", M, "latched: [SAFE]", "latched: [SAFE, LIMBO]")])],
    "GEN-046": [("gate", [("replace", M, "      E2eErrorCount: e2e_error_count\n", "")]),
                ("gate", [("replace", M, "NearestDistance: input.nearest_distance", "NearestDistance: input.nearest_rel_speed")])],
    "GEN-047": [("gate", [("replace", M, "wcet_budget_us: 500", "wcet_budget_us: 20000")])],
    "GEN-050": [("gpu", [("replace", M, "{name: logits, dtype: f32, shape: [1, 4], role: intermediate}",
                          "{name: logits, dtype: f32, shape: [1, 4], role: output}")])],
    "GEN-051": [("gpu", [("replace", M, "{name: normalized, dtype: f32, shape: [1, 64], role: intermediate}",
                          "{name: normalized, dtype: f32, shape: [1, 64], role: intermediate, file: weights/fc1_b.bin, sha256: "
                          + "a" * 64 + "}")])],
    "GEN-052": [("gpu", [("write_bytes", "weights/fc1_b.bin", b"\x00" * 16)]),
                ("gpu", [("replace", M, "shape: [4], role: weight", "shape: [8], role: weight")])],
    "GEN-053": [("gpu", [("replace", M, "{name: relu1, op: relu, input: logits,", "{name: relu1, op: relu, input: scores,")]),
                ("gpu", [("replace", M, "{name: head, op: softmax,", "{name: head, op: normalize,")])],
    "GEN-054": [("gpu", [("replace", M, "classes: [car, pedestrian, cyclist, background]", "classes: [car, pedestrian, cyclist]")])],
    "GEN-055": [("gpu", [("replace", M, "    - {check: finite, tensor: scores, satisfies: [REQ-OBS-005]}\n", ""),
                         ("append", M, "")]),
                ("gpu", [("replace", M, "{check: sum, tensor: scores, value: 1.0,", "{check: sum, tensor: logits, value: 1.0,")])],
    "GEN-056": [("gpu", [("patch_limit",)])],
    "GEN-057": [("gpu", [("replace", M, "timeout_ms: 66", "timeout_ms: 10")])],
}


def run_case(example: str, mutations: list[tuple]):
    p = MutatedProject(example)
    try:
        patched = False
        for m in mutations:
            if m[0] == "replace":
                p.replace(*m[1:])
            elif m[0] == "append":
                p.append(*m[1:])
            elif m[0] == "write_bytes":
                p.write_bytes(*m[1:])
            elif m[0] == "symlink":
                (p.dir / m[1]).symlink_to(m[2])
            elif m[0] == "patch_limit":
                patched = True
        if patched:
            with mock.patch.object(gpu_stage, "ARENA_LIMIT", 256):
                return p.run()
        return p.run()
    finally:
        p.close()


class RuleTests(unittest.TestCase):
    def test_unmodified_examples_pass(self):
        for example in ("gate", "gpu"):
            with self.subTest(example=example):
                diags = run_case(example, [])
                self.assertTrue(diags.ok(), "\n".join(f.format() for f in diags.sorted()))

    def test_each_rule_rejects_its_mutation(self):
        for rule, cases in sorted(CASES.items()):
            for i, (example, mutations) in enumerate(cases):
                with self.subTest(rule=rule, case=i):
                    diags = run_case(example, mutations)
                    self.assertIn(rule, diags.rules(), "\n".join(f.format() for f in diags.sorted()) or "accepted")


if __name__ == "__main__":
    unittest.main()
