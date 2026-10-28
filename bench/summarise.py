#!/usr/bin/env python3
"""Turn RESULT lines from run.sh into a Markdown table.

For each (implementation, mode, data set, variant) the best speed over all
rounds is kept. Each variant after the first is also shown relative to the
first variant (usually master): >1.00 means faster.
"""
import sys
from collections import defaultdict


def main(path, title):
    best = defaultdict(float)
    variants = []
    for line in open(path):
        f = line.split()
        if len(f) != 6 or f[0] != "RESULT":
            continue
        _, name, mode, dataset, impl, gbs = f
        if name not in variants:
            variants.append(name)
        key = (impl, mode, dataset, name)
        best[key] = max(best[key], float(gbs))

    impls = sorted({k[0] for k in best}, key=lambda i: (i != "fallback", i))
    sets = ["tiny", "images", "email", "large"]
    base = variants[0]
    print(f"### {title}\n")
    head = "| implementation | mode | data | " + " | ".join(
        f"{v} GB/s" for v in variants)
    head += "".join(f" | {v}/{base}" for v in variants[1:]) + " |"
    print(head)
    print("|" + "---|" * (3 + 2 * len(variants) - 1))
    for impl in impls:
        for mode in ("encode", "decode"):
            for dataset in sets:
                vals = [best.get((impl, mode, dataset, v)) for v in variants]
                if not all(vals):
                    continue
                row = f"| {impl} | {mode} | {dataset} | "
                row += " | ".join(f"{x:.2f}" for x in vals)
                row += "".join(f" | {x / vals[0]:.2f}" for x in vals[1:])
                print(row + " |")
    print()


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])
