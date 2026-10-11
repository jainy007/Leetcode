"""JSON-Schema validation of every input document (GEN-002)."""
from __future__ import annotations

import json
import pathlib
from functools import lru_cache
from typing import Any

import jsonschema
from referencing import Registry, Resource

from tools.gen.errors import Diagnostics

SCHEMA_DIR = pathlib.Path(__file__).resolve().parents[2] / "schemas"


@lru_cache(maxsize=None)
def _registry() -> Registry:
    resources = []
    for path in sorted(SCHEMA_DIR.glob("*.schema.json")):
        doc = json.loads(path.read_text(encoding="utf-8"))
        resources.append((path.name, Resource.from_contents(doc)))
        resources.append((doc["$id"], Resource.from_contents(doc)))
    return Registry().with_resources(resources)


@lru_cache(maxsize=None)
def _validator(schema_name: str) -> jsonschema.Draft202012Validator:
    doc = json.loads((SCHEMA_DIR / schema_name).read_text(encoding="utf-8"))
    jsonschema.Draft202012Validator.check_schema(doc)
    return jsonschema.Draft202012Validator(doc, registry=_registry())


def validate(doc: Any, schema_name: str, diags: Diagnostics, display: str) -> bool:
    errors = sorted(_validator(schema_name).iter_errors(doc), key=lambda e: (list(map(str, e.absolute_path)), e.message))
    for err in errors:
        where = "/".join(str(p) for p in err.absolute_path) or "<root>"
        diags.error("GEN-002", display, f"{where}: {err.message}")
    return not errors
