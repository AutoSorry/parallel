# LU Decomposition

This branch contains the LU decomposition command-line tool built on the
shared `framework/` library. `serial_lu` performs LU without pivoting, stores
the result in packed form, and validates `L * U ≈ A` against a deterministic,
strongly diagonally dominant input matrix.

## Build

需要 GCC、CMake 和 OpenMP 支持。

```bash
cmake -S . -B build
cmake --build build
```

矩阵规模通过运行时参数 `-n` 指定，输入矩阵由程序生成。

## Run

```bash
./build/bin/lu --list
./build/bin/lu -m serial_lu -n 500 -r 5 --seed ABCDEFGH
./build/bin/lu -m openmp_lu -n 500 -r 5 -t 4 --seed ABCDEFGH
```

使用 `-n` 指定方阵边长，使用 `-r` 指定运行次数，使用 `-t` 指定 OpenMP 线程数，使用 `--seed` 指定 8 个大写字母组成的种子。

程序根据种子生成随机的强对角占优矩阵。对每一行，非对角元素取自 `[-0.5, 0.5]`，对角元素设置为该行非对角元素绝对值之和加 `1.0`，因此满足无主元 LU 所需的严格对角占优条件。未指定种子时，程序自动生成并打印种子。

The `main` branch hosts the shared framework; the `matmul` branch hosts the
matrix multiplication tool and its experiments.
