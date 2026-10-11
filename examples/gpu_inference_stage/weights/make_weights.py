"""Deterministically (re)creates the example weight files.

The weights are synthetic (a fixed integer formula) so the example is reproducible
without a training pipeline. In a real project these files are exported by the training
toolchain and their SHA-256 is recorded in model.yaml by the model owner.
"""
import hashlib
import pathlib
import struct

IN, OUT = 64, 4
HERE = pathlib.Path(__file__).resolve().parent


def weights():
    return [((i * (j + 1)) % 7 - 3) / 20.0 for i in range(IN) for j in range(OUT)]


def bias():
    return [0.1, -0.2, 0.05, 0.0]


def write(name, values):
    data = struct.pack("<%df" % len(values), *values)
    (HERE / name).write_bytes(data)
    print(name, hashlib.sha256(data).hexdigest())


if __name__ == "__main__":
    write("fc1_w.bin", weights())
    write("fc1_b.bin", bias())
