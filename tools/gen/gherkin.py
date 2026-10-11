"""Parser for the supported Gherkin subset (GEN-020, GEN-025).

Supported: comments, tags, one Feature with free description, Scenario, Scenario Outline
(alias Scenario Template) with one or more Examples tables, Given/When/Then/And/But steps.
Rejected on purpose: Background, Rule, doc strings, data tables on steps, i18n keywords.
Keeping the grammar small keeps the qualified tool small.
"""
from __future__ import annotations

import re
from dataclasses import dataclass, field

from tools.gen.errors import Diagnostics

_STEP_RE = re.compile(r"^(Given|When|Then|And|But) (.+)$")
_TAG_RE = re.compile(r"^@[A-Za-z0-9_-]+$")
_PLACEHOLDER_RE = re.compile(r"<([A-Za-z0-9_]+)>")


@dataclass(frozen=True)
class Step:
    keyword: str  # as written
    kind: str  # given | when | then (And/But resolved)
    text: str
    line: int


@dataclass(frozen=True)
class Scenario:
    name: str
    tags: tuple[str, ...]
    steps: tuple[Step, ...]
    line: int
    path: str
    example: tuple[tuple[str, str], ...] = ()  # (column, value) for expanded outline rows


@dataclass
class Feature:
    name: str
    path: str
    scenarios: list[Scenario] = field(default_factory=list)


@dataclass
class _Block:
    name: str
    tags: tuple[str, ...]
    line: int
    outline: bool
    steps: list[Step] = field(default_factory=list)
    examples: list[list[list[str]]] = field(default_factory=list)  # tables of rows
    in_examples: bool = False


def _split_row(line: str) -> list[str] | None:
    if not (line.startswith("|") and line.endswith("|")) or len(line) < 2:
        return None
    return [c.strip() for c in line[1:-1].split("|")]


def parse(text: str, path: str, diags: Diagnostics) -> Feature | None:
    feature: Feature | None = None
    pending_tags: list[str] = []
    blocks: list[_Block] = []
    current: _Block | None = None
    in_description = False

    for lineno, raw in enumerate(text.split("\n"), start=1):
        line = raw.strip()
        if not line or line.startswith("#"):
            continue
        if line.startswith("@"):
            tags = line.split()
            bad = [t for t in tags if not _TAG_RE.match(t)]
            if bad:
                diags.error("GEN-020", path, f"malformed tag(s): {' '.join(bad)}", lineno)
            pending_tags.extend(tags)
            continue
        if line.startswith("Feature:"):
            if feature is not None:
                diags.error("GEN-020", path, "only one Feature per file is supported", lineno)
                return None
            feature = Feature(name=line[len("Feature:"):].strip(), path=path)
            pending_tags = []
            in_description = True
            continue
        if feature is None:
            diags.error("GEN-020", path, "content before 'Feature:'", lineno)
            return None
        head = re.match(r"^(Scenario Outline|Scenario Template|Scenario):\s*(.*)$", line)
        if head:
            name = head.group(2).strip()
            if not name:
                diags.error("GEN-020", path, "scenario without a name", lineno)
            current = _Block(name=name, tags=tuple(pending_tags), line=lineno, outline=head.group(1) != "Scenario")
            blocks.append(current)
            pending_tags = []
            in_description = False
            continue
        if re.match(r"^(Background|Rule|Example|Scenarios):", line):
            diags.error("GEN-020", path, f"unsupported keyword: {line.split(':')[0]}", lineno)
            continue
        if line.startswith("Examples:"):
            if current is None or not current.outline:
                diags.error("GEN-020", path, "Examples outside a Scenario Outline", lineno)
                continue
            current.examples.append([])
            current.in_examples = True
            continue
        if line.startswith('"""') or line.startswith("```"):
            diags.error("GEN-020", path, "doc strings are not supported", lineno)
            continue
        row = _split_row(line)
        if row is not None:
            if current is None or not current.in_examples:
                diags.error("GEN-020", path, "data tables on steps are not supported", lineno)
                continue
            current.examples[-1].append(row)
            continue
        step = _STEP_RE.match(line)
        if step:
            if current is None:
                diags.error("GEN-020", path, "step outside a scenario", lineno)
                continue
            if current.in_examples:
                diags.error("GEN-020", path, "step after Examples", lineno)
                continue
            keyword, body = step.group(1), step.group(2).strip()
            if keyword in ("And", "But"):
                if not current.steps:
                    diags.error("GEN-020", path, f"'{keyword}' cannot start a scenario", lineno)
                    continue
                kind = current.steps[-1].kind
            else:
                kind = keyword.lower()
            current.steps.append(Step(keyword, kind, body, lineno))
            continue
        if in_description:
            continue  # free-form feature description
        diags.error("GEN-020", path, f"unrecognized line: {line}", lineno)

    if feature is None:
        diags.error("GEN-020", path, "missing 'Feature:'")
        return None
    if pending_tags:
        diags.error("GEN-020", path, "tags at end of file are not attached to a scenario")

    for block in blocks:
        if not block.steps:
            diags.error("GEN-020", path, f"scenario '{block.name}' has no steps", block.line)
            continue
        if not block.outline:
            feature.scenarios.append(Scenario(block.name, block.tags, tuple(block.steps), block.line, path))
            continue
        feature.scenarios.extend(_expand_outline(block, path, diags))
    return feature


def _expand_outline(block: _Block, path: str, diags: Diagnostics) -> list[Scenario]:
    rows: list[tuple[str, ...]] = []
    header: list[str] | None = None
    for table in block.examples:
        if len(table) < 2:
            diags.error("GEN-025", path, f"Examples of '{block.name}' need a header and at least one row", block.line)
            continue
        if header is None:
            header = table[0]
        elif table[0] != header:
            diags.error("GEN-025", path, f"Examples tables of '{block.name}' must share one header", block.line)
            continue
        for r in table[1:]:
            if len(r) != len(header):
                diags.error("GEN-025", path, f"Examples row width mismatch in '{block.name}'", block.line)
                continue
            rows.append(tuple(r))
    if header is None:
        diags.error("GEN-025", path, f"Scenario Outline '{block.name}' has no Examples", block.line)
        return []
    if len(set(header)) != len(header):
        diags.error("GEN-025", path, f"duplicate Examples column in '{block.name}'", block.line)
        return []
    used: set[str] = set()
    for st in block.steps:
        used.update(_PLACEHOLDER_RE.findall(st.text))
    undefined = sorted(used - set(header))
    if undefined:
        diags.error("GEN-025", path, f"undefined placeholder(s) {undefined} in '{block.name}'", block.line)
        return []
    unused = sorted(set(header) - used)
    if unused:
        diags.error("GEN-025", path, f"Examples column(s) {unused} not used in '{block.name}'", block.line)
        return []
    out = []
    for idx, row in enumerate(rows, start=1):
        values = dict(zip(header, row))
        steps = tuple(
            Step(s.keyword, s.kind, _PLACEHOLDER_RE.sub(lambda m: values[m.group(1)], s.text), s.line)
            for s in block.steps
        )
        out.append(
            Scenario(
                name=f"{block.name} #{idx}",
                tags=block.tags,
                steps=steps,
                line=block.line,
                path=path,
                example=tuple((h, values[h]) for h in header),
            )
        )
    return out
