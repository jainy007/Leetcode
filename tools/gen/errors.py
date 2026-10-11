"""Diagnostics. Every finding names the rule it violates (see rules.py / docs/rules/generator.md)."""
from __future__ import annotations

from dataclasses import dataclass, field


@dataclass(frozen=True, order=True)
class Finding:
    path: str
    line: int
    rule: str
    message: str

    def format(self) -> str:
        loc = f"{self.path}:{self.line}" if self.line else self.path
        return f"{loc}: [{self.rule}] {self.message}"


@dataclass
class Diagnostics:
    findings: list[Finding] = field(default_factory=list)

    def error(self, rule: str, path: str, message: str, line: int = 0) -> None:
        from tools.gen.rules import RULES

        if rule not in RULES:
            raise KeyError(f"unregistered rule id {rule}")
        self.findings.append(Finding(str(path), line, rule, message))

    def ok(self) -> bool:
        return not self.findings

    def rules(self) -> set[str]:
        return {f.rule for f in self.findings}

    def sorted(self) -> list[Finding]:
        return sorted(set(self.findings))


class GenerationFailed(Exception):
    def __init__(self, diagnostics: Diagnostics):
        super().__init__("\n".join(f.format() for f in diagnostics.sorted()))
        self.diagnostics = diagnostics
