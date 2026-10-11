"""ea-gen: deterministic, rule-based generator for the Req -> Gherkin -> Model -> C++ chain.

No randomness, no clocks, no network, no AI. The same inputs and the same generator version
always produce byte-identical output (verified by the qualification suite).
"""

GENERATOR_NAME = "ea-gen"
GENERATOR_VERSION = "0.1.0"
