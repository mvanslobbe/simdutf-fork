#!/usr/bin/env bash
# Usage: run.sh <rounds> <data dir> <name>...
# Runs bin/<name> for every round, mode, data set and implementation,
# interleaving the variants so that they share the same machine state.
# Prints one "RESULT variant mode set implementation GB/s" line per result.
set -euo pipefail
rounds=$1
data=$2
shift 2
best=$("bin/$1" -l | sed -n 's/^# current system detected as \(.*\)\.$/\1/p')
impls="fallback"
if [ "$best" != fallback ]; then
  impls="fallback $best"
fi
for round in $(seq "$rounds"); do
  for set in tiny images email large; do
    for mode in encode decode; do
      for impl in $impls; do
        if [ "$mode" = encode ]; then
          filter=(-f "simdutf::${impl}_standard")
        else
          filter=(-f "simdutf::${impl}")
        fi
        for name in "$@"; do
          "bin/$name" "--$mode" "$data/$set"/*.txt "${filter[@]}" |
            sed -n 's/^simdutf::\([a-z0-9]*\)\(_standard\)\? *: *\([0-9.]*\) GB\/s.*/\1 \3/p' |
            while read -r found gbs; do
              [ "$found" = "$impl" ] && echo "RESULT $name $mode $set $impl $gbs"
            done || true
        done
      done
    done
  done
  echo "# round $round done" >&2
done
