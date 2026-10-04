# 并行程序设计实验——矩阵乘法

本项目实现一份矩阵乘法命令行程序，使用朴素串行程序作为 basline ，支持对并行算法进行正确性检验以及性能测量。

## 项目结构

```text
.
├── CMakeLists.txt              # 项目构建入口
├── framework/                  # 通用接口
│   ├── CMakeLists.txt
│   ├── include/
│   │   ├── kernel.h
│   │   ├── registry.h
│   │   └── timer.h
│   └── src/
│       ├── registry.c
│       └── timer.c
└── matmul/                     # 矩阵乘法应用
    ├── CMakeLists.txt
    ├── algorithms/
    ├── include/
    └── src/
```

## 环境与构建

| 依赖 | 要求 |
| --- | --- |
| CMake | 3.16 或更高版本 |
| GCC | 支持 C99 和 OpenMP 的 GNU C 编译器 |

在仓库根目录执行，首次配置时显式指定 GCC 编译器：

```bash
cmake -S . -B build -DCMAKE_C_COMPILER=/path/to/gcc
cmake --build build
```

构建完成后，可执行文件位于 `build/bin/matmul`。

## 运行

```bash
./build/bin/matmul --list
./build/bin/matmul -m naive -n 500 -r 5
./build/bin/matmul -m openmp -n 500 -t 4 -r 5
./build/bin/matmul --all -n 500 -r 5
```

常用参数：

| 参数 | 作用 |
| --- | --- |
| `-l`, `--list` | 列出已注册的算法 |
| `-a`, `--all` | 运行所有可用算法 |
| `-m`, `--module NAME` | 运行指定算法，如 `naive`、`openmp` |
| `-n`, `--size N` | 指定方阵的规模 |
| `-r`, `--runs N` | 指定运行次数，默认为 5 |
| `-t`, `--threads N` | 指定 OpenMP 线程数 |
| `-A`, `--input-a SOURCE` | 指定输入矩阵 A 的来源 |
| `-B`, `--input-b SOURCE` | 指定输入矩阵 B 的来源 |
| `--seed SEED` | 为随机输入指定 8 个大写字母组成的种子 |

### 输入矩阵

A、B 可分别选择 `random` 或文本文件路径；未指定时默认为 `random`。

```bash
./build/bin/matmul -A random -B random -n 500 --seed ABCDEFGH -m naive
./build/bin/matmul -A matrix_a.txt -B matrix_b.txt -m naive
./build/bin/matmul -A matrix_a.txt -B random -m openmp -t 4
```

如果所有输入均随机，则必须指定 `-n`；否则由文件描述决定规模。文件描述矩阵的规模与指定的 `-n` 应当一致。

每个文本文件描述**一个方阵**：第一项是正整数边长 `N`，随后是按行排列的 `N × N` 个数字，数字可用空格或换行分隔。例如 `2 × 2` 的矩阵：

```text
2
1 2
3 4
```
