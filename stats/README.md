# Matrix Multiplication Statistics

This directory stores experiment statistics and plotting scripts.

## Directory layout

- `display/`: generated tables and figures for reports.
- `plot_line.py`: generic line-chart plotting script for CSV data.
- `run_openmp_threads.py`: runs OpenMP thread-scaling experiments and writes a CSV table.

## Files

- `display/serial_scaling.csv`: timing table for serial matrix multiplication at different matrix sizes.
- `display/serial_scaling_median_time.png`: median-time scaling figure generated from the CSV file.
- `display/openmp_thread_scaling.csv`: OpenMP timing table with different thread counts, using serial `naive` median time as the speedup baseline.
- `display/openmp_thread_scaling_median_time.png`: OpenMP median-time curve with different thread counts.
- `display/openmp_thread_scaling_speedup.png`: OpenMP speedup curve with different thread counts.

Generate the serial scaling plot with:

```bash
python3 stats/plot_line.py \
  --csv stats/display/serial_scaling.csv \
  --x N \
  --y median_ms \
  --xlabel "Matrix size N" \
  --ylabel "Median time" \
  --unit "ms" \
  --title "Serial Matrix Multiplication Scaling" \
  --output stats/display/serial_scaling_median_time.png
```

Generate OpenMP thread-scaling plots with:

```bash
python3 stats/plot_line.py \
  --csv stats/display/openmp_thread_scaling.csv \
  --x threads \
  --y median_ms \
  --xlabel "OpenMP threads" \
  --ylabel "Median time" \
  --unit "ms" \
  --title "OpenMP Thread Scaling: Median Time" \
  --output stats/display/openmp_thread_scaling_median_time.png

python3 stats/plot_line.py \
  --csv stats/display/openmp_thread_scaling.csv \
  --x threads \
  --y speedup_vs_serial_baseline \
  --xlabel "OpenMP threads" \
  --ylabel "Speedup vs serial baseline" \
  --unit "x" \
  --title "OpenMP Thread Scaling: Speedup vs Serial Baseline" \
  --output stats/display/openmp_thread_scaling_speedup.png \
  --ideal
```
