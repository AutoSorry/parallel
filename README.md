# 并行程序设计实验——LU 分解

本分支实现一个基于共享 `framework/` 的 LU 分解命令行程序，提供串行、OpenMP 并行和 SIMD 向量化实现，覆盖 `k-i-j` 与 `k-j-i` 两种循环顺序。

## 项目结构

```text
.
├── CMakeLists.txt              # 项目构建入口
├── framework/                  # 通用算法接口、注册表和计时器
│   ├── CMakeLists.txt
│   ├── include/
│   └── src/
├── lu/                         # LU 分解应用
│   ├── CMakeLists.txt
│   ├── algorithms/             # 串行、OpenMP 和 SIMD 算法
│   ├── include/                # LU 数据结构和验证接口
│   └── src/                    # 输入、验证、命令行和程序入口
└── README.md
```

## 环境与构建

| 依赖 | 要求 |
| --- | --- |
| CMake | 3.16 或更高版本 |
| GCC | 支持 C99 和 OpenMP 的 GNU C 编译器 |

OpenMP 是本项目的必需依赖。CMake 配置阶段会检查 GCC 和 OpenMP 支持；不满足条件时，配置会失败。

在仓库根目录执行，首次配置时显式指定 GCC：

```bash
cmake -S . -B build -DCMAKE_C_COMPILER=/path/to/gcc
cmake --build build
```


## 运行

列出已注册的算法：

```bash
./build/bin/lu --list
```

运行一个 OpenMP 版本：

```bash
./build/bin/lu -m openmp_lu_kij -n 2000 -r 5 -t 4 --seed ABCDEFGH
```

常用参数：

| 参数 | 作用 |
| --- | --- |
| `-l`, `--list` | 列出已注册的算法 |
| `-m`, `--module NAME` | 选择一个已注册的 LU 算法 |
| `-n`, `--size N` | 指定 `N × N` 方阵的边长 |
| `-r`, `--runs N` | 指定运行次数，默认为 5 |
| `-t`, `--threads N` | 指定 OpenMP 线程数 |
| `-c`, `--chunk N` | 指定动态调度的 chunk size，默认为 1 |
| `--seed SEED` | 指定 8 个大写字母组成的输入种子 |

如果没有指定 `-m`，程序会依次运行所有已注册算法。计时区间只覆盖 LU 消元过程，输入生成、矩阵复制、参考结果计算和正确性验证均在计时之外完成。

算法原理、实验设计、数据与分析见 [`docs/report.md`](docs/report.md)。
