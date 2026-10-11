"""Registry of generator rules. Each rule is enforced by exactly one check and covered by at
least one negative test in tools/gen/tests (enforced by test_rule_coverage)."""

RULES: dict[str, str] = {
    # Inputs
    "GEN-001": "YAML input must not contain duplicate keys or non-plain constructs",
    "GEN-002": "Input must validate against its JSON schema",
    "GEN-003": "Referenced input files must exist inside the project directory",
    # Requirements
    "GEN-010": "Requirement IDs must be unique",
    "GEN-011": "Requirement text must follow the EARS template of its declared type",
    "GEN-012": "Requirements file must belong to the model's component",
    "GEN-013": "Every requirement verified by SIL must be covered by at least one scenario",
    "GEN-014": "Every scenario must carry at least one tag, each naming an existing requirement",
    "GEN-015": "Every 'satisfies' reference must name an existing requirement",
    "GEN-016": "Every requirement must be satisfied by at least one model element",
    # Gherkin
    "GEN-020": "Gherkin must use the supported subset (Feature, Scenario, Scenario Outline, Examples, steps)",
    "GEN-021": "Every step must match exactly one entry of the pattern's step catalogue",
    "GEN-022": "A step may only be used with the keywords its catalogue entry allows",
    "GEN-023": "Step parameters must name existing model elements or allowed literals",
    "GEN-024": "Scenario names must be unique within a project",
    "GEN-025": "Scenario Outline placeholders must be defined by Examples with at least one row",
    # Messages and codecs
    "GEN-030": "Message names must be unique, and signal names unique within a message",
    "GEN-031": "Every signal must lie inside the message payload",
    "GEN-032": "Signals must not overlap each other or the E2E header",
    "GEN-033": "Physical range must be ordered and representable by the raw signal with factor/offset",
    "GEN-034": "SIL default values must lie inside the physical range",
    "GEN-035": "Transport parameters must match the transport (someip block / CAN id)",
    "GEN-036": "The E2E header must lie inside the payload",
    "GEN-037": "DBC files must use the supported subset (no multiplexing, no extended value types)",
    # input_gate
    "GEN-040": "The gate input must be an existing E2E-protected message",
    "GEN-041": "Debounce counter names must be unique",
    "GEN-042": "Initial state and transitions must reference declared states and debounce events",
    "GEN-043": "Every state must be reachable from the initial state",
    "GEN-044": "Transitions must be deterministic; latched states have no exits; other states need one",
    "GEN-045": "Latched and valid-output states must be declared states",
    "GEN-046": "The status output must map every signal of an existing message from a valid source that fits",
    "GEN-047": "The task WCET budget must be smaller than its period",
    # gpu_stage
    "GEN-050": "Tensor names must be unique with exactly one input and at least one output",
    "GEN-051": "Weight tensors need file and sha256; other tensors must not have them",
    "GEN-052": "Weight files must exist, match their SHA-256 and their declared shape and dtype",
    "GEN-053": "Nodes must form a valid single-assignment dataflow with type-correct operations",
    "GEN-054": "The number of classes must equal the element count of the output tensor",
    "GEN-055": "Plausibility checks must reference output tensors and carry their parameters",
    "GEN-056": "Graph memory must fit the static arena limit",
    "GEN-057": "Monitor timeout must not be shorter than the stage deadline",
    # acc_controller
    "GEN-060": "Every interface signal must be bound to a DBC signal of a message in the right direction",
    "GEN-061": "A bound signal must cover the physical range the pattern needs (discrete: factor 1, offset 0)",
    "GEN-062": "E2E protection needs a CRC byte and a counter nibble that no interface signal uses",
    "GEN-063": "Calibration must be complete, consistent and representable in the command resolution",
    "GEN-064": "Receive timing must be consistent: multiples of the cycle, timeout >= 2 periods, E2E max delta >= timeout/period",
}
