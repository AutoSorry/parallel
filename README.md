# 并行程序设计实验——LU 分解

本分支实现一个基于共享 `framework/` 的 LU 分解命令行程序。程序提供串行无主元 LU 分解和 OpenMP 并行 LU 分解，用于比较具有循环依赖的数值计算任务在不同线程数下的性能表现。

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
│   ├── algorithms/             # serial_lu、openmp_lu
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

将 `/path/to/gcc` 替换为本机实际的 GCC 路径。构建完成后，可执行文件位于：

```text
build/bin/lu
```

更换编译器时请使用新的构建目录，或清理已有的 `build/` 后重新配置。`build/` 已被 Git 忽略。

## 运行

列出已注册的算法：

```bash
./build/bin/lu --list
```

运行串行 LU：

```bash
./build/bin/lu -m serial_lu -n 500 -r 5 --seed ABCDEFGH
```

运行 OpenMP LU：

```bash
./build/bin/lu -m openmp_lu -n 500 -r 5 -t 4 --seed ABCDEFGH
```

常用参数：

| 参数 | 作用 |
| --- | --- |
| `-l`, `--list` | 列出已注册的算法 |
| `-m`, `--module NAME` | 选择 `serial_lu` 或 `openmp_lu` |
| `-n`, `--size N` | 指定 `N × N` 方阵的边长 |
| `-r`, `--runs N` | 指定运行次数，默认为 5 |
| `-t`, `--threads N` | 指定 OpenMP 线程数 |
| `-c`, `--chunk N` | 指定动态调度的 chunk size，默认为 1 |
| `--seed SEED` | 指定 8 个大写字母组成的输入种子 |

如果没有指定 `-m`，程序会依次运行所有已注册算法。输入生成、矩阵复制、串行参考结果计算和结果验证不计入 LU 算法的计时区间；每一轮运行前，程序会将原始矩阵复制到工作矩阵，然后对工作矩阵原地执行 LU 分解。

## 输入矩阵

程序根据种子生成确定性的强对角占优方阵，不从文件读取输入。`--seed` 必须是恰好 8 个大写字母，例如：

```text
ABCDEFGH
```

如果未指定 `--seed`，程序会自动生成一个 8 位大写字母种子并打印出来，便于记录和复现实验。

矩阵生成规则如下：

1. 非对角元素从 `[-0.5, 0.5]` 范围内确定性生成；
2. 对每一行计算非对角元素绝对值之和；
3. 将对角元素设置为该绝对值之和加 `1.0`。

因此每一行满足：

```text
|A[i][i]| > Σ(j != i) |A[i][j]|
```

该严格对角占优性质降低了无主元 LU 分解出现零主元的风险。

## LU 分解格式

程序使用无主元 LU 分解，并将结果保存在同一个工作矩阵中。该矩阵采用 packed LU 格式：

```text
C[i][j] = U[i][j]，当 i <= j
C[i][j] = L[i][j]，当 i >  j
```

其中 `L` 的对角线隐含为 1，不单独存储。程序先使用 `serial_lu` 对相同输入生成 packed LU 参考结果，再将目标算法的 packed LU 与串行参考结果逐元素比较。这样可以直接检查并行调度是否改变了串行算法的计算结果。

## 算法说明

### `serial_lu`

串行 LU 使用三层循环执行原地高斯消元：

```text
for k = 0 ... N-1
    pivot = A[k][k]
    for i = k+1 ... N-1
        A[i][k] /= pivot
        for j = k+1 ... N-1
            A[i][j] -= A[i][k] × A[k][j]
```

### `openmp_lu`

LU 的 `k` 循环存在循环携带依赖：第 `k+1` 步必须读取第 `k` 步更新后的矩阵，因此不能直接并行化。当前 OpenMP 实现保持 `k` 串行，在固定 `k` 后并行处理 `i` 行，`j` 循环保持串行。这样每个线程负责一整行更新，避免不同线程写入同一行。

随着 `k` 增加，可并行的行数逐渐减少，因此 LU 的并行规模会收缩。这也是高线程数下并行效率可能下降的重要原因。
