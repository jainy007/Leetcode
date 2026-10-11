"""Consistency lint for requirements, Gherkin and models: runs every generator rule, writes nothing.

    python3 -m tools.lint [project ...]

This is what an agent (or a human) runs while drafting inputs. It is the same code path as the
generator, so "lint clean" means "the generator will accept it".
"""
import sys

from tools.gen.__main__ import main

if __name__ == "__main__":
    sys.exit(main(["--dry-run", *sys.argv[1:]]))
