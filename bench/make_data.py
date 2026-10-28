#!/usr/bin/env python3
"""Write the benchmark inputs as base64 text files, one input per file.

tiny/   2000 inputs of 1 to 64 random bytes (tail-dominated)
large/  one input of 1 MiB random bytes
images/ and email/ are copied from lemire/base64data by the workflow.

The same files are used for encoding (as raw bytes) and decoding.
"""
import base64
import os
import random
import sys


def write(path, raw):
    with open(path, "wb") as f:
        f.write(base64.b64encode(raw))


def main(out):
    rng = random.Random(1065)
    os.makedirs(os.path.join(out, "tiny"), exist_ok=True)
    for i in range(2000):
        n = rng.randint(1, 64)
        write(os.path.join(out, "tiny", f"{i:04d}.txt"), rng.randbytes(n))
    os.makedirs(os.path.join(out, "large"), exist_ok=True)
    write(os.path.join(out, "large", "1mib.txt"), rng.randbytes(1 << 20))


if __name__ == "__main__":
    main(sys.argv[1])
