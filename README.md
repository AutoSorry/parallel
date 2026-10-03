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
for the OpenMP thread count. By default, both input matrices are generated
deterministically. You can choose each input independently with `-A`/`--input-a`
and `-B`/`--input-b`:

```bash
./build/matmul/matmul -A random -B random -n 500 -m naive
./build/matmul/matmul -A matrix_a.txt -B matrix_b.txt -m naive
./build/matmul/matmul -A matrix_a.txt -B random -m openmp -t 4
```

The source `random` selects deterministic generated data. Any other source is
treated as a text file. A matrix file starts with its dimension and then has
`N*N` numeric values, for example:

```text
2
1 2
3 4
```

When a file is used, its dimension determines `N`; an explicit `-n` must match
it. Both input files must have the same dimension. Results are validated
against matrix multiplication. The `stats/` directory contains the
thread-scaling experiment and plotting scripts; see `stats/README.md`.

The `main` branch hosts the shared framework; the `lu` branch hosts the LU
decomposition tool.
