# Benchmarks for simdutf#1065

Builds `benchmarks/base64/benchmark_base64` from each revision in
`bench/variants.txt` and runs them interleaved on the same runner, keeping
the best speed over all rounds. Results are in each job's summary.
