"""Helpers for negative tests: copy an example into a temp dir, mutate it, run the generator."""
from __future__ import annotations

import pathlib
import shutil
import tempfile

from tools.gen import project
from tools.gen.errors import Diagnostics
from tools.gen.render import REPO_ROOT

EXAMPLES = {
    "gate": REPO_ROOT / "examples" / "adas_input_gate",
    "gpu": REPO_ROOT / "examples" / "gpu_inference_stage",
    "acc": REPO_ROOT / "examples" / "acc",
    "aeb": REPO_ROOT / "examples" / "aeb",
    "acc_monitor": REPO_ROOT / "examples" / "acc_monitor",
    "aeb_monitor": REPO_ROOT / "examples" / "aeb_monitor",
}


class MutatedProject:
    def __init__(self, example: str):
        self._tmp = tempfile.TemporaryDirectory(prefix="ea_gen_test_")
        self.dir = pathlib.Path(self._tmp.name) / EXAMPLES[example].name
        shutil.copytree(EXAMPLES[example], self.dir)

    def replace(self, rel: str, old: str, new: str, count: int = 1) -> "MutatedProject":
        path = self.dir / rel
        text = path.read_text(encoding="utf-8")
        if old not in text:
            raise AssertionError(f"mutation anchor not found in {rel}: {old!r}")
        path.write_text(text.replace(old, new, count), encoding="utf-8")
        return self

    def append(self, rel: str, text: str) -> "MutatedProject":
        path = self.dir / rel
        path.write_text(path.read_text(encoding="utf-8") + text, encoding="utf-8")
        return self

    def write_bytes(self, rel: str, data: bytes) -> "MutatedProject":
        (self.dir / rel).write_bytes(data)
        return self

    def run(self) -> Diagnostics:
        diags = Diagnostics()
        project.generate("mutant", str(self.dir / "model.yaml"), "generated/mutant", diags)
        return diags

    def close(self) -> None:
        self._tmp.cleanup()
