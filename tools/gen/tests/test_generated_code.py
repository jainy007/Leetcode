"""Rule-based scan of generated production code for constructs the asil profile forbids."""
import re
import unittest

from tools.gen.render import REPO_ROOT

FORBIDDEN = {
    r"\bnew\s+[A-Za-z_]": "CPU-004 heap allocation",
    r"\b(malloc|calloc|realloc|free)\s*\(": "CPU-004 heap allocation",
    r"\bthrow\b|\btry\s*\{|\bcatch\s*\(": "CPU-001 exceptions",
    r"\bdynamic_cast\b|\btypeid\b": "CPU-002 RTTI",
    r"\bsystem_clock\b|CLOCK_REALTIME": "CPU-008 wall clock",
    r"\bgoto\b": "structured control flow",
    r"\bstd::(vector|string|map|unordered_map|function|shared_ptr)\b": "CPU-004 allocating types",
}


class GeneratedCodeRules(unittest.TestCase):
    def test_production_sources_are_clean(self):
        files = [p for p in (REPO_ROOT / "generated").rglob("*") if p.suffix in (".hpp", ".cpp") and "sil" not in p.parts]
        self.assertTrue(files)
        for path in files:
            code = re.sub(r"//.*", "", path.read_text(encoding="utf-8"))
            for pattern, why in FORBIDDEN.items():
                with self.subTest(file=path.name, rule=why):
                    self.assertIsNone(re.search(pattern, code), f"{path.name}: {why}")


if __name__ == "__main__":
    unittest.main()
