import unittest

from tools.gen import gherkin
from tools.gen.errors import Diagnostics

TEXT = """
# comment
Feature: Demo
  Free description line.

  @REQ-A-001
  Scenario: Plain
    Given a thing
    And another thing
    When it happens
    Then it is done
    But not twice

  @REQ-A-002 @REQ-A-003
  Scenario Outline: Outline <x>
    Given value <x>
    Then result <y>

    Examples:
      | x | y |
      | 1 | 2 |
      | 3 | 4 |
"""


class GherkinParser(unittest.TestCase):
    def test_parses_scenarios_and_resolves_and_but(self):
        d = Diagnostics()
        f = gherkin.parse(TEXT, "demo.feature", d)
        self.assertTrue(d.ok(), d.sorted())
        self.assertEqual(f.name, "Demo")
        self.assertEqual(len(f.scenarios), 3)
        plain = f.scenarios[0]
        self.assertEqual([s.kind for s in plain.steps], ["given", "given", "when", "then", "then"])
        self.assertEqual(plain.tags, ("@REQ-A-001",))

    def test_outline_expansion(self):
        d = Diagnostics()
        f = gherkin.parse(TEXT, "demo.feature", d)
        ex2 = f.scenarios[2]
        self.assertEqual(ex2.name, "Outline <x> #2")
        self.assertEqual([s.text for s in ex2.steps], ["value 3", "result 4"])
        self.assertEqual(ex2.example, (("x", "3"), ("y", "4")))

    def test_rejects_unsupported_constructs(self):
        for bad in ("Feature: X\n  Background:\n    Given a\n",
                    "Feature: X\n  Scenario: S\n    Given a\n      | t |\n",
                    "Feature: X\n  Scenario: S\n    And a\n",
                    "Scenario: S\n",
                    "Feature: X\n  Scenario: S\n    Given a\n    \"\"\"\n"):
            with self.subTest(text=bad):
                d = Diagnostics()
                gherkin.parse(bad, "bad.feature", d)
                self.assertIn("GEN-020", d.rules())


if __name__ == "__main__":
    unittest.main()
