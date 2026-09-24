#!/bin/sh

set -eu

CC="${CC:-/opt/homebrew/bin/gcc-16}"
CFLAGS="${CFLAGS:--O3 -march=native -fopenmp}"
THREADS="${THREADS:-16}"
RUNS="${RUNS:-3}"
RESULT="${RESULT:-ppt_scale_results_gcc.txt}"

SOURCES="ppt_pi1 ppt_pi2 ppt_pi3 ppt_pi4 ppt_pi5"
SIZES="10000000 100000000 1000000000 5000000000 10000000000"

printf '%s\n' 'OpenMP PPT multi-scale benchmark' > "$RESULT"
printf '%s\n' '================================' >> "$RESULT"
printf '%s\n' 'Configuration:' >> "$RESULT"
printf '  compiler: %s\n' "$CC" >> "$RESULT"
printf '  compiler_version: ' >> "$RESULT"
"$CC" --version | while IFS= read -r line; do printf '%s\n' "$line"; break; done >> "$RESULT"
printf '  flags: %s\n' "$CFLAGS" >> "$RESULT"
printf '  threads: %s\n' "$THREADS" >> "$RESULT"
printf '  runs_per_size: %s\n' "$RUNS" >> "$RESULT"
printf '  sizes: %s\n' "$SIZES" >> "$RESULT"
printf '  timing: omp_get_wtime() inside each program\n' >> "$RESULT"
printf '%s\n\n' 'Each size is compiled separately because NUM_STEPS is a compile-time macro.' >> "$RESULT"

for size in $SIZES; do
    printf '%s\n' "Building NUM_STEPS=$size ..." >&2
    for source in $SOURCES; do
        "$CC" $CFLAGS -DNUM_STEPS="${size}L" -DNUM_THREADS="$THREADS" \
            "${source}.c" -o "${source}_scale"
    done

    printf '%s\n' "NUM_STEPS=$size" >> "$RESULT"
    printf '%s\n' "----------------" >> "$RESULT"
    for source in $SOURCES; do
        printf '[%s]\n' "$source" >> "$RESULT"
        for run in $(seq 1 "$RUNS"); do
            printf 'run %s: ' "$run" >> "$RESULT"
            printf '1000000000\n' | OMP_NUM_THREADS="$THREADS" \
                "./${source}_scale" >> "$RESULT"
        done
        printf '\n' >> "$RESULT"
    done
done

python3 - "$RESULT" "$SOURCES" <<'PY'
import re
import sys
from pathlib import Path

path = Path(sys.argv[1])
sources = sys.argv[2:]
text = path.read_text()

print("Summary (all recorded runs):", file=sys.stdout)
with path.open("a") as out:
    out.write("Summary (arithmetic mean of all recorded runs):\n")
    for size_block in re.finditer(r"NUM_STEPS=(\d+)\n-+\n(.*?)(?=\nNUM_STEPS=|\Z)", text, re.S):
        size, block = size_block.groups()
        out.write(f"  NUM_STEPS={size}\n")
        print(f"  NUM_STEPS={size}")
        for source in sources:
            match = re.search(rf"\[{source}\]\n(.*?)(?=\n\[|\Z)", block, re.S)
            values = [float(value) for value in re.findall(r"time=([0-9.]+)", match.group(1))] if match else []
            if values:
                mean = sum(values) / len(values)
                out.write(f"    {source}: mean={mean:.9f} s min={min(values):.9f} s\n")
                print(f"    {source}: mean={mean:.9f} s min={min(values):.9f} s")
        out.write("\n")
PY

printf 'Results written to %s\n' "$RESULT" >&2
