#!/usr/bin/env python3
import csv
import re
import statistics
import subprocess
from pathlib import Path


SCRIPT_DIR = Path(__file__).resolve().parent
PROJECT_DIR = SCRIPT_DIR.parent
DISPLAY_DIR = SCRIPT_DIR / "display"
OUTPUT_CSV = DISPLAY_DIR / "openmp_thread_scaling.csv"
BUILD_DIR = "build"

MATRIX_N = 1000
RUNS = 5
THREADS = [1, 2, 4, 8, 16, 32]


def run_command(cmd):
    print("$", " ".join(str(x) for x in cmd))
    result = subprocess.run(
        cmd,
        cwd=PROJECT_DIR,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        check=False,
    )
    print(result.stdout, end="")
    if result.returncode != 0:
        raise SystemExit(result.returncode)
    return result.stdout


def parse_output(output):
    times = [float(x) for x in re.findall(r"run \d+: ([0-9.]+) ms", output)]
    avg_match = re.search(r"avg: ([0-9.]+) ms", output)
    sum_match = re.search(r"sum: ([0-9.]+)", output)
    validation = "PASS" if "validation:" in output and "PASS" in output else "FAIL"

    if len(times) != RUNS:
        raise RuntimeError(f"expected {RUNS} runs, got {len(times)}")

    return {
        "times": times,
        "avg_ms": float(avg_match.group(1)) if avg_match else statistics.mean(times),
        "median_ms": statistics.median(times),
        "sum": sum_match.group(1) if sum_match else "",
        "validation": validation,
    }


def main():
    DISPLAY_DIR.mkdir(parents=True, exist_ok=True)

    run_command(["cmake", "-S", ".", "-B", BUILD_DIR])
    run_command(["cmake", "--build", BUILD_DIR])

    executable = f"./{BUILD_DIR}/matmul"

    baseline_output = run_command([executable, "-m", "naive", "-n", str(MATRIX_N), "-r", str(RUNS)])
    baseline = parse_output(baseline_output)
    baseline_median = baseline["median_ms"]

    rows = []

    for threads in THREADS:
        output = run_command([executable, "-m", "openmp", "-n", str(MATRIX_N), "-r", str(RUNS), "-t", str(threads)])
        parsed = parse_output(output)
        rows.append({
            "N": MATRIX_N,
            "baseline_module": "naive",
            "baseline_median_ms": baseline_median,
            "threads": threads,
            "run1_ms": parsed["times"][0],
            "run2_ms": parsed["times"][1],
            "run3_ms": parsed["times"][2],
            "run4_ms": parsed["times"][3],
            "run5_ms": parsed["times"][4],
            "avg_ms": parsed["avg_ms"],
            "median_ms": parsed["median_ms"],
            "speedup_vs_serial_baseline": baseline_median / parsed["median_ms"],
            "sum": parsed["sum"],
            "validation": parsed["validation"],
        })

    fieldnames = [
        "N",
        "baseline_module",
        "baseline_median_ms",
        "threads",
        "run1_ms",
        "run2_ms",
        "run3_ms",
        "run4_ms",
        "run5_ms",
        "avg_ms",
        "median_ms",
        "speedup_vs_serial_baseline",
        "sum",
        "validation",
    ]

    with OUTPUT_CSV.open("w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        for row in rows:
            writer.writerow(row)

    print(f"saved: {OUTPUT_CSV}")


if __name__ == "__main__":
    main()
