#!/usr/bin/env bash
# Usage: build.sh <name> <repository url> <commit or branch>
# Builds benchmarks/base64/benchmark_base64 from the given revision and
# copies it to bin/<name>. Uses $CC and $CXX.
set -euo pipefail
name=$1
url=$2
ref=$3
src=src/$name
rm -rf "$src"
git init -q "$src"
git -C "$src" fetch -q --depth 1 "$url" "$ref"
git -C "$src" checkout -q FETCH_HEAD
echo "$name: $url $ref = $(git -C "$src" rev-parse HEAD)" | tee -a bin/revisions.txt
cmake -S "$src" -B "build/$name" -DCMAKE_BUILD_TYPE=Release \
  -DSIMDUTF_TESTS=ON -DSIMDUTF_BENCHMARKS=ON >/dev/null
cmake --build "build/$name" --target benchmark_base64 -j "${JOBS:-$(nproc)}" >/dev/null
cp "build/$name/benchmarks/base64/benchmark_base64" "bin/$name"
