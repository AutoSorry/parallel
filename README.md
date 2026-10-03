# LU Decomposition

This branch contains the LU decomposition command-line tool built on the
shared `framework/` library. `serial_lu` performs LU without pivoting, stores
the result in packed form, and validates `L * U ≈ A` against a deterministic,
strongly diagonally dominant input matrix.

## Build

Requirements: CMake and a C compiler. OpenMP is not required.

```bash
cmake -S . -B build
cmake --build build
```

To change the default matrix size, configure with `-DLU_MATRIX_N=512`.

## Run

```bash
./build/lu/lu --list
./build/lu/lu -m serial_lu -n 500 -r 5
```

Use `-n` for the runtime matrix size and `-r` for the number of runs.

The `main` branch hosts the shared framework; the `matmul` branch hosts the
matrix multiplication tool and its experiments.
