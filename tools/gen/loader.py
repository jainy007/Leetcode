"""Strict YAML loading: duplicate keys are errors (GEN-001), only plain data types are built."""
from __future__ import annotations

import pathlib
import re
from typing import Any

import yaml

from tools.gen.errors import Diagnostics


class _StrictLoader(yaml.SafeLoader):
    pass


# YAML 1.1 turns yes/no/on/off into booleans (a key named `on` silently becomes True).
# Use YAML 1.2 semantics: only true/false are booleans.
_StrictLoader.yaml_implicit_resolvers = {
    first: [(tag, regexp) for tag, regexp in resolvers if tag != "tag:yaml.org,2002:bool"]
    for first, resolvers in yaml.SafeLoader.yaml_implicit_resolvers.items()
}
_StrictLoader.add_implicit_resolver(
    "tag:yaml.org,2002:bool", re.compile(r"^(?:true|True|TRUE|false|False|FALSE)$"), list("tTfF")
)


def _construct_mapping(loader: _StrictLoader, node: yaml.MappingNode, deep: bool = False) -> dict:
    mapping: dict[Any, Any] = {}
    for key_node, value_node in node.value:
        key = loader.construct_object(key_node, deep=deep)
        if key in mapping:
            raise yaml.constructor.ConstructorError(
                None, None, f"duplicate key {key!r}", key_node.start_mark
            )
        mapping[key] = loader.construct_object(value_node, deep=deep)
    return mapping


def _no_timestamps(loader: _StrictLoader, node: yaml.Node) -> str:
    # Dates would turn into datetime objects; keep them as plain strings.
    return loader.construct_scalar(node)


_StrictLoader.add_constructor(yaml.resolver.BaseResolver.DEFAULT_MAPPING_TAG, _construct_mapping)
_StrictLoader.add_constructor("tag:yaml.org,2002:timestamp", _no_timestamps)


def read_text(path: pathlib.Path) -> str:
    """Reads UTF-8 text with normalized line endings."""
    return path.read_bytes().decode("utf-8").replace("\r\n", "\n")


def load_yaml(path: pathlib.Path, diags: Diagnostics, display: str) -> Any | None:
    try:
        return yaml.load(read_text(path), Loader=_StrictLoader)  # noqa: S506 (strict SafeLoader subclass)
    except yaml.YAMLError as exc:
        line = 0
        mark = getattr(exc, "problem_mark", None) or getattr(exc, "context_mark", None)
        if mark is not None:
            line = mark.line + 1
        diags.error("GEN-001", display, str(getattr(exc, "problem", exc)), line)
        return None
