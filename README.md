# Matrix Kernels

This project contains two independent command-line tools built on a shared C
matrix-kernel framework:

- `matmul`: serial, OpenMP, and placeholder matrix multiplication modules;
- `lu`: serial LU decomposition without pivoting.

The `stub` matmul module is intentionally retained as an example module. It is
excluded from `matmul --all`, but can be run explicitly and is expected to fail
validation.

The project is built directly in the host environment. Docker and VS Code
Dev Containers are not required.

## Requirements

- GCC (on macOS, use Homebrew GCC explicitly; `/usr/bin/gcc` is an alias for Apple Clang)
- CMake
- Ninja (optional; used by the commands below when available)
- Python 3 and matplotlib for generating experiment plots
- OpenMP support for the OpenMP implementation

On macOS with Homebrew, install GCC and OpenMP with:

```bash
brew install gcc libomp
```

## Build

On macOS, select the Homebrew GCC compiler explicitly. For example, Homebrew
GCC 16 is normally installed as `/opt/homebrew/bin/gcc-16`:

```bash
rm -rf build
CC=/opt/homebrew/bin/gcc-16 cmake -S . -B build
cmake --build build
```

If your Homebrew GCC has a different version, replace `gcc-16` with the
installed executable name.

Using Ninja with GCC:

```bash
rm -rf build
CC=/opt/homebrew/bin/gcc-16 cmake -S . -B build -G Ninja
cmake --build build
```

If Ninja is not installed, use CMake's default generator instead:

```bash
CC=/opt/homebrew/bin/gcc-16 cmake -S . -B build
cmake --build build
```

To use a different compile-time matrix size for both tools:

```bash
CC=/opt/homebrew/bin/gcc-16 cmake -S . -B build -G Ninja -DMATRIX_N=512
cmake --build build
```

The LU target has its own size option if it needs to differ from matmul:

```bash
cmake -S . -B build -DMATRIX_N=512 -DLU_MATRIX_N=256
cmake --build build
```

## Run

List the registered algorithms:

```bash
./build/matmul/matmul --list
```

Run the serial implementation:

```bash
./build/matmul/matmul -m naive -n 500 -r 5
```

Run the OpenMP implementation with four threads:

```bash
./build/matmul/matmul -m openmp -n 500 -t 4 -r 5
```

Run all available modules:

```bash
./build/matmul/matmul --all -r 5
```

`--all` runs `naive` and `openmp`; it skips `stub` by design. To run the
placeholder explicitly:

```bash
./build/matmul/matmul -m stub -n 4 -r 1
```

## LU

List and run the serial LU module:

```bash
./build/lu/lu --list
./build/lu/lu -m serial_lu -n 500 -r 5
```

The first LU implementation uses deterministic, strongly diagonally dominant
input and no pivoting. It stores the result in packed LU form and validates
`L * U ≈ A`. Partial pivoting is reserved for a later extension.

## Experiments

Run the OpenMP thread-scaling experiment:

```bash
python3 stats/run_openmp_threads.py
```

The generated CSV files and figures are stored under `stats/display/`.

## Record the environment

For reproducible performance reports, record the local toolchain and system
information:

```bash
uname -a
cmake --version
```
