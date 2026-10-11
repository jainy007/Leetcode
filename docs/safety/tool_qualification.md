# Tool classification and qualification plan: ea-gen

Standard: ISO 26262-8:2018, clause 11 (confidence in the use of software tools).
Status: **draft for assessment**. Owner: tool chain maintainer (see CODEOWNERS).

## 1. Tool and use cases
| Item | Content |
|---|---|
| Tool | `ea-gen` (`python3 -m tools.gen`) and its configuration: `templates/`, `schemas/`, `sil/steps/` |
| Version identification | `GENERATOR_VERSION` in `tools/gen/__init__.py`, plus the SHA-256 of all tool files stamped into every generated file (`Inputs: sha256:...`) |
| UC-1 | Generate C++ source for ASIL A–D software units (patterns `input_gate`, `gpu_stage`) from a reviewed model |
| UC-2 | Generate SIL test cases from Gherkin scenarios, plus the trace data (`trace.json`) |
| UC-3 | Check requirements, scenarios and models for consistency (`python3 -m tools.lint`) |
| Excluded | Any AI or LLM component. Agents may draft inputs, which are reviewed by humans, but they are not part of the tool. |

## 2. Tool impact (TI) and tool error detection (TD)
| Use case | Possible malfunction | TI | Detection measures in the process | TD |
|---|---|---|---|---|
| UC-1 | Wrong code generated (wrong transition, codec bit position, range) | TI2 | SIL tests generated independently from Gherkin (UC-2) and executed against the code; MC/DC coverage; review of the patterns; compiler and static analysis of the output | TD2 |
| UC-2 | Missing or weakened test | TI2 | Trace report requires every SIL requirement to have passing tests; scenario-level review; mutation checks (see §4) | TD2 |
| UC-3 | Rule not enforced (inconsistency passes) | TI2 | Negative test for every rule; independent review of the inputs | TD2 |

Result: **TCL2** for UC-1 to UC-3. ASIL D use needs method 1c (validation) or 1b (evaluation of
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

## 4. Validation of the tool (method 1c)
1. **Rule validation:** `python3 -m unittest discover -s tools/gen/tests -t .`, run in CI on every change.
2. **Output validation (per project):**
   - Compile the generated SIL suites with the production flags and run them.
   - Mutate the model (for example change a debounce threshold) and confirm that the SIL suite
     fails. Example run: threshold 3 → 4 made exactly the 3 tests of REQ-GATE-003 fail.
3. **Back-to-back testing** of the GPU backends against `cpu_ref` within the tolerance declared in the model (GPU-007).
4. **Known limitations:**
   - Gherkin subset only (no Background, data tables or doc strings).
   - DBC subset only (no multiplexing, no float signals).
   - The generated state machine takes at most one transition per cycle, by design.

## 5. Change control
- Changes to `tools/gen/`, `templates/`, `schemas/` or `sil/steps/` need two human approvals
  (CODEOWNERS and branch protection) and a passing qualification suite.
- Agents never modify these paths (CLAUDE.md). The CI check fails if generated output changes
  without a matching change to its inputs.
- Releases bump `GENERATOR_VERSION` and archive the qualification run (CI artefacts).
