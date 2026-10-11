"""Determinism: identical inputs give byte-identical outputs, independent of hash seed, locale,
working directory and repetition. Also the committed generated/ must equal the generator output."""
from __future__ import annotations

import filecmp
import os
import pathlib
import subprocess
import sys
import tempfile
import unittest

from tools.gen.render import REPO_ROOT


def _generate_into(out_root: pathlib.Path, env_overrides: dict[str, str], cwd: pathlib.Path) -> None:
    env = dict(os.environ)
    env.update(env_overrides)
    env["PYTHONPATH"] = str(REPO_ROOT)
    subprocess.run([sys.executable, "-m", "tools.gen", "--out-root", str(out_root)], check=True, env=env, cwd=cwd,
                   stdout=subprocess.DEVNULL)


def _tree(root: pathlib.Path) -> dict[str, bytes]:
    return {p.relative_to(root).as_posix(): p.read_bytes() for p in sorted(root.rglob("*")) if p.is_file()}


class Determinism(unittest.TestCase):
    def test_repeatable_across_seeds_locales_and_cwd(self):
        with tempfile.TemporaryDirectory() as a, tempfile.TemporaryDirectory() as b, tempfile.TemporaryDirectory() as c:
            _generate_into(pathlib.Path(a), {"PYTHONHASHSEED": "0", "LC_ALL": "C"}, REPO_ROOT)
            _generate_into(pathlib.Path(b), {"PYTHONHASHSEED": "4242", "LC_ALL": "C.UTF-8", "TZ": "Asia/Kolkata"},
                           pathlib.Path(c))
            ta, tb = _tree(pathlib.Path(a)), _tree(pathlib.Path(b))
            self.assertTrue(ta)
            self.assertEqual(sorted(ta), sorted(tb))
            for rel in ta:
                with self.subTest(file=rel):
                    self.assertEqual(ta[rel], tb[rel])

    def test_committed_output_is_current(self):
        with tempfile.TemporaryDirectory() as a:
            _generate_into(pathlib.Path(a), {}, REPO_ROOT)
            cmp = filecmp.dircmp(pathlib.Path(a) / "generated", REPO_ROOT / "generated")
            self.assertEqual([], cmp.left_only + cmp.right_only + cmp.diff_files,
                             "generated/ differs from generator output; run python3 -m tools.gen")
            for sub in cmp.subdirs.values():
                self.assertEqual([], sub.left_only + sub.right_only + sub.diff_files)

    def test_no_volatile_content(self):
        for path in (REPO_ROOT / "generated").rglob("*"):
            if path.is_file():
                text = path.read_text(encoding="utf-8")
                with self.subTest(file=path.name):
                    self.assertNotIn(str(REPO_ROOT), text, "absolute paths make output machine dependent")
                    self.assertNotIn("\r", text)


if __name__ == "__main__":
    unittest.main()
