# Matrix Multiplication

This branch contains the matrix multiplication command-line tool, built on the
shared `framework/` library. It includes serial (`naive`) and OpenMP (`openmp`)
implementations. The `stub` module is a placeholder: it is excluded from
`--all` and fails validation when run explicitly.

## Build

Requirements: CMake and a C compiler. OpenMP support is needed for parallel
execution; on macOS, use Homebrew GCC explicitly (Apple Clang needs `libomp`).

```bash
cmake -S . -B build
cmake --build build
```

On macOS with Homebrew GCC, for example:

```bash
CC=/opt/homebrew/bin/gcc-16 cmake -S . -B build
cmake --build build
```

To change the default matrix size, configure with `-DMATRIX_N=512`.

## Run

```bash
./build/matmul/matmul --list
./build/matmul/matmul -m naive -n 500 -r 5
./build/matmul/matmul -m openmp -n 500 -t 4 -r 5
./build/matmul/matmul --all -r 5
```

Use `-n` for the runtime matrix size, `-r` for the number of runs, and `-t`
for the OpenMP thread count. Results are validated against matrix
multiplication. The `stats/` directory contains the thread-scaling experiment
and plotting scripts; see `stats/README.md`.

The `main` branch hosts the shared framework; the `lu` branch hosts the LU
decomposition tool.
