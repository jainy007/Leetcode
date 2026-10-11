"""Pattern library. A pattern is a pre-qualified C++ architecture ("block") whose parameters come
from the model, comparable to a library block in model-based design."""

from tools.gen.patterns import acc_controller, gpu_stage, input_gate

PATTERNS = {
    "input_gate": input_gate,
    "gpu_stage": gpu_stage,
    "acc_controller": acc_controller,
}
