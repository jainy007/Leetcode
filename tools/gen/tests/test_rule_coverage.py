"""Every registered rule has a negative test and is documented (qualification completeness)."""
import re
import unittest

from tools.gen.render import REPO_ROOT
from tools.gen.rules import RULES
from tools.gen.tests.test_rules import CASES


class RuleCoverage(unittest.TestCase):
    def test_every_rule_has_a_negative_test(self):
        self.assertEqual(sorted(RULES), sorted(CASES))

    def test_every_rule_is_documented(self):
        doc = (REPO_ROOT / "docs" / "rules" / "generator.md").read_text(encoding="utf-8")
        documented = set(re.findall(r"\bGEN-\d{3}\b", doc))
        self.assertEqual(sorted(RULES), sorted(documented))

    def test_every_rule_is_enforced_in_code(self):
        sources = "".join(p.read_text(encoding="utf-8") for p in (REPO_ROOT / "tools" / "gen").rglob("*.py")
                          if "tests" not in p.parts and p.name != "rules.py")
        for rule in RULES:
            with self.subTest(rule=rule):
                self.assertIn(f'"{rule}"', sources)


if __name__ == "__main__":
    unittest.main()
