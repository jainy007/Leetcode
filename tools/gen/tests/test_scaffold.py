"""The scaffold produces a complete, well-formed feature skeleton (end-to-end build of a scaffolded
feature is demonstrated in docs/extending.md and by CI's scaffold job)."""
import pathlib
import py_compile
import tempfile
import unittest

import yaml

from tools.gen import gherkin
from tools.gen.errors import Diagnostics
from tools.scaffold.__main__ import scaffold_feature


class Scaffold(unittest.TestCase):
    def test_creates_well_formed_files(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = pathlib.Path(tmp)
            files = scaffold_feature(root, "demo_lka", "DemoLkaController", 816)
            rels = {p.relative_to(root).as_posix() for p in files}
            for expected in ("tools/gen/features/demo_lka.py", "templates/features/demo_lka/logic.cpp.j2",
                             "sil/steps/features/demo_lka.yaml", "sil/plant/src/demo_lka_observer.cpp",
                             "examples/demo_lka/model.yaml", "examples/demo_lka/interfaces/demo_lka.dbc"):
                self.assertIn(expected, rels)
            py_compile.compile(str(root / "tools/gen/features/demo_lka.py"), doraise=True)
            for rel in ("sil/steps/features/demo_lka.yaml", "examples/demo_lka/model.yaml",
                        "examples/demo_lka/requirements.yaml", "examples/demo_lka/mutations.yaml"):
                self.assertIsInstance(yaml.safe_load((root / rel).read_text()), dict, rel)
            d = Diagnostics()
            feature = gherkin.parse((root / "examples/demo_lka/features/basic.feature").read_text(), "basic.feature", d)
            self.assertTrue(d.ok(), d.sorted())
            self.assertEqual(len(feature.scenarios), 5)

    def test_refuses_to_overwrite(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = pathlib.Path(tmp)
            scaffold_feature(root, "demo", "DemoController", 816)
            with self.assertRaises(SystemExit):
                scaffold_feature(root, "demo", "DemoController", 816)


if __name__ == "__main__":
    unittest.main()
