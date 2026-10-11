# Tool classification and qualification plan: ea-gen

Standard: ISO 26262-8:2018, clause 11 (confidence in the use of software tools).
Status: **draft for assessment**. Owner: tool chain maintainer (see CODEOWNERS).

## 1. Tool and use cases
| Item | Content |
|---|---|
| Tool | `ea-gen` (`python3 -m tools.gen`) and its configuration: `templates/`, `schemas/`, `sil/steps/` |
| Version identification | `GENERATOR_VERSION` in `tools/gen/__init__.py`, plus the SHA-256 of all tool files stamped into every generated file (`Inputs: sha256:...`) |
| UC-1 | Generate C++ source for ASIL A–D software units (patterns `input_gate`, `gpu_stage`, `can_function` with its feature plug-ins) from a reviewed model |
| UC-2 | Generate SIL test cases from Gherkin scenarios, plus the trace data (`trace.json`) |
| UC-3 | Check requirements, scenarios and models for consistency (`python3 -m tools.lint`), including system-level checks of decomposed ASIL D functions (pattern `can_system`: FTTI budget GEN-065, node interface consistency GEN-066) |
| UC-4 | Verification tools: `tools.trace` (trace matrix), `tools.coverage` (structural coverage gate), `tools.mutate` (mutation campaigns) |
| Excluded | Any AI or LLM component at generation time. The tool may be *developed* with AI assistance (see change control); its execution is deterministic and AI-free. |

## 2. Tool impact (TI) and tool error detection (TD)
| Use case | Possible malfunction | TI | Detection measures in the process | TD |
|---|---|---|---|---|
| UC-1 | Wrong code generated (wrong transition, codec bit position, range) | TI2 | SIL tests generated independently from Gherkin (UC-2) and executed against the code; MC/DC coverage; review of the patterns; compiler and static analysis of the output | TD2 |
| UC-2 | Missing or weakened test | TI2 | Trace report requires every SIL requirement to have passing tests; scenario-level review; mutation checks (see §4) | TD2 |
| UC-3 | Rule not enforced (inconsistency passes) | TI2 | Negative test for every rule; independent review of the inputs; for GEN-065 also the measured fault reaction in system SIL | TD2 |
| UC-4 | A verification tool reports a pass that is not one (missed coverage, missed mutant, wrong trace status) | TI2 | Gates are self-tested: a removed justification fails the coverage gate, a calibration value without a mutant fails `tools.mutate`, the trace tool reads the GoogleTest XML directly; reports are reviewed | TD2 |

Result: **TCL2** for UC-1 to UC-4. ASIL D use needs method 1c (validation) or 1b (evaluation of
the development process). This plan uses **1c** for ASIL A–C and **1b + 1c** for ASIL D.

## 3. Requirements on the tool (excerpt)
| ID | Requirement | Verified by |
|---|---|---|
| TR-01 | The same inputs and tool version give byte-identical outputs, independent of hash seed, locale, time zone and working directory. | `test_determinism.py` |
| TR-02 | No output is written unless every rule passes. | `__main__.py` design, `test_rules.py` |
| TR-03 | Each rule GEN-xxx rejects inputs that violate it. | `test_rules.py` (one or more mutations per rule) |
| TR-04 | Every rule is implemented, documented and tested. | `test_rule_coverage.py` |
| TR-05 | Committed outputs equal the tool output; hand edits are detected. | `--check` in CI, `test_committed_output_is_current` |
| TR-06 | Generated production code contains no constructs forbidden by the asil profile. | `test_generated_code.py`, `-fno-exceptions -fno-rtti -Werror` build |
| TR-07 | YAML is interpreted with YAML 1.2 booleans, without duplicate keys, without non-finite numbers. | GEN-001 tests |
| TR-08 | Third-party dependencies are pinned. | `tools/requirements.txt` |
| TR-09 | Generated codecs agree bit for bit with the runtime's generic codec at the ends and middle of every signal range, and reject values one step outside. | generated `*_codec_test.cpp` (back-to-back), run in CI |
| TR-10 | For a decomposed ASIL D function, every safety goal's mechanisms detect within FTTI minus the actuator reaction time, computed from the node models. | GEN-065 tests; system SIL measures fault → safe state |
| TR-11 | Generated product code of ASIL D-derived components reaches 100 % MC/DC, or each uncovered condition is justified. | `python3 -m tools.coverage` in CI |

## 4. Validation of the tool (method 1c)
1. **Rule validation:** `python3 -m unittest discover -s tools/gen/tests -t .`, run in CI on every change.
2. **Output validation (per project):**
   - Compile the generated SIL suites with the production flags and run them.
   - Mutate the model (for example change a debounce threshold) and confirm that the SIL suite
     fails. Example run: threshold 3 → 4 made exactly the 3 tests of REQ-GATE-003 fail.
3. **Back-to-back testing:**
   - the GPU backends against `cpu_ref`, within the tolerance declared in the model (GPU-007);
   - every generated codec against the runtime's generic bit codec (TR-09).
4. **Mutation campaigns** (`python3 -m tools.mutate <project>`):
   - deliberate defects in models, DBC files, feature templates, the framework, the shared codec
     template and the system template;
   - every calibration value has at least one mutant;
   - each mutant must be detected by a rule, a codec test or SIL, or be declared equivalent or
     tolerated with a written reason;
   - results are in the SIL reports under `docs/`.
5. **Structural coverage gate:** see TR-11.
6. **Known limitations:**
   - Gherkin subset only (no Background, data tables or doc strings).
   - DBC subset only (no multiplexing, no float signals).
   - The generated state machine takes at most one transition per cycle, by design.

## 5. Change control
- Changes to `tools/gen/`, `templates/`, `schemas/` or `sil/steps/` need two human approvals
  (CODEOWNERS and branch protection) and a passing qualification suite.
- The tool may be developed by an AI agent (CLAUDE.md). This does not change the evidence, which
  rests on validation, not on who wrote the code: the qualification suite, determinism tests,
  mutation campaigns and owner review are required for every change. The CI check fails if
  generated output changes without a matching change to its inputs.
- Releases bump `GENERATOR_VERSION` and archive the qualification run (CI artefacts).
