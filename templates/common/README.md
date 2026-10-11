Templates are part of the qualified generator (ISO 26262-8 clause 11). Humans only; every change
needs two reviews and a passing qualification suite (`python3 -m unittest discover -s tools/gen/tests -t .`).
Rendering: Jinja2 with StrictUndefined, trim_blocks, lstrip_blocks. No logic beyond loops and
conditionals over the validated model; every decision is made in the pattern module in Python.
