# Matrix Kernel Framework 重建与拆分计划

## 1. 目标

将当前以矩阵乘法为中心的项目重建为一个可复用的矩阵计算框架，并在同一个 Git 仓库中维护两个独立的命令行工具：

```text
matmul：矩阵乘法工具
lu：    LU 分解工具
```

两个工具共享框架代码，但分别拥有自己的输入生成、算法模块、验证逻辑和命令行入口。

本次重建的优先级是：

1. 先保留现有矩阵乘法工具的可用功能；
2. 再新增独立的 LU 分解工具；
3. 最后逐步抽象公共运行逻辑，避免把矩阵乘法语义泄漏到通用框架中；
4. 通过清晰的 Git 提交历史记录每一个可验证的重构阶段。

---

## 2. 目标目录结构

最终目录结构计划如下：

```text
matrix-kernels/
├── CMakeLists.txt
├── README.md
├── REBUILD_PLAN.md
├── .gitignore
├── framework/
│   ├── CMakeLists.txt
│   ├── include/
│   │   ├── kernel.h
│   │   ├── registry.h
│   │   ├── timer.h
│   │   └── matrix.h
│   └── src/
│       ├── registry.c
│       ├── timer.c
│       └── matrix.c
├── matmul/
│   ├── CMakeLists.txt
│   ├── include/
│   │   ├── matmul_config.h
│   │   └── matmul_validate.h
│   ├── src/
│   │   ├── main.c
│   │   ├── matrix_init.c
│   │   └── validate.c
│   └── algorithms/
│       ├── algo_naive.c
│       └── algo_openmp.c
├── lu/
│   ├── CMakeLists.txt
│   ├── include/
│   │   ├── lu_config.h
│   │   └── lu_validate.h
│   ├── src/
│   │   ├── main.c
│   │   ├── matrix_init.c
│   │   └── validate.c
│   └── algorithms/
│       └── algo_serial_lu.c
└── stats/
    ├── README.md
    └── ...
```

说明：目录名可以根据后续仓库命名调整，但“共享 framework + 独立 matmul + 独立 lu”这一层次保持不变。

---

## 3. 共享 framework 的职责

`framework/` 只放与具体算法语义无关的代码。

### 3.1 模块接口

把当前的 `matmul_fn` 和 `matmul_module` 改成通用命名，例如：

```c
typedef struct {
    const double *a;
    const double *b;
    double *c;
    int n;
    void *extra;
} kernel_data;

typedef struct {
    int runs;
    int threads;
} kernel_options;

typedef void (*kernel_fn)(kernel_data *data,
                          const kernel_options *options);
```

模块描述至少包含：

```c
typedef struct {
    const char *name;
    const char *description;
    kernel_fn run;
} kernel_module;
```

约定：

- `a`、`b` 是算法输入，某些算法可以不使用 `b`；
- `c` 是主要输出缓冲区；
- `extra` 为需要额外输出的算法预留，例如 LU 的 pivot vector；
- `n` 是编译期或运行期矩阵尺寸，具体采用哪一种由工具配置决定；
- framework 不解释 `c` 的数学含义，也不负责验证算法正确性。

### 3.2 注册表

保留当前注册表的基本能力：

- 注册模块；
- 按名称查找模块；
- 获取模块数量；
- 按索引访问模块；
- 列出全部模块。

第一版重建不再依赖 `__attribute__((constructor))` 自动注册。每个工具显式调用自己的注册函数，例如：

```c
void matmul_register_algorithms(void);
void lu_register_algorithms(void);
```

这样可以避免静态库链接时目标文件被丢弃，也能明确控制每个可执行文件包含哪些算法。

### 3.3 计时

framework 提供：

```c
double now_seconds(void);
```

使用单调时钟。公共 runner 只测量模块 `run` 调用本身，初始化和验证放在计时区间之外。

### 3.4 通用矩阵辅助函数

只保留不依赖具体问题语义的功能，例如：

- 矩阵清零；
- 矩阵拷贝；
- 矩阵分配/释放辅助；
- 可选的矩阵摘要。

矩阵乘法输入生成和 LU 输入生成不放进 framework，分别由各自工具负责。

---

## 4. matmul 工具职责

`matmul/` 保持当前矩阵乘法工具的用户体验，并逐步迁移到通用接口。

### 4.1 算法模块

保留并改造：

- `algo_naive.c`：串行三重循环矩阵乘法；
- `algo_openmp.c`：OpenMP 外层循环矩阵乘法。

它们统一实现：

```text
C = A * B
```

### 4.2 输入生成

由 `matmul/src/matrix_init.c` 生成 `A` 和 `B`。

输入生成必须是确定性的，保证：

- 不同算法使用相同数据；
- 多次运行之间输入一致；
- 结果验证可复现。

### 4.3 验证

矩阵乘法工具继续使用：

```text
C ≈ A * B
```

参考结果由 `naive` 或独立参考实现生成。

### 4.4 命令行

第一阶段保持现有参数：

```text
--list|-l
--all|-a
--module|-m NAME
--runs|-r N
--size|-n N
--threads|-t N
```

目标可执行文件：

```bash
./build/matmul
```

---

## 5. lu 工具职责

`lu/` 是独立的 LU 分解命令行工具，不再把 LU 强行解释为矩阵乘法算法。

### 5.1 算法模块

第一版实现：

```text
serial_lu：串行 LU 分解
```

输入和输出约定：

```text
A：待分解矩阵，只读
B：不使用，设为 NULL 或忽略
C：packed LU 输出
```

packed LU 约定：

```text
C[i][j] = U[i][j]，当 i <= j
C[i][j] = L[i][j]，当 i >  j
```

其中 `L` 的对角线隐含为 1，不单独存储。

### 5.2 选主元策略

第一版必须在实现前明确以下策略之一：

#### 基础版本：无选主元

- 输入生成器保证矩阵非奇异且适合无选主元 LU；
- 验证 `A ≈ L * U`；
- 不需要 `pivot vector`。

#### 完整版本：部分选主元

- 实现 `P * A = L * U`；
- `C` 保存 packed LU；
- `extra` 或独立输出保存 pivot vector；
- 验证 `P * A ≈ L * U`。

默认先完成无选主元基础版本，再根据课程要求扩展部分选主元。无论采用哪种版本，都不能继续使用当前矩阵乘法工具中全 1 的 `B` 作为 LU 输入。

### 5.3 输入生成

由 `lu/src/matrix_init.c` 生成适合 LU 的矩阵。第一版建议使用确定性、强对角占优矩阵，以降低零主元风险。

### 5.4 验证

LU 验证不比较 `A * B`，而是：

```text
从 packed LU 还原 L 和 U
计算 L * U
比较 L * U 与原始 A
```

验证过程必须在计时区间外。

### 5.5 命令行

复用通用参数中的：

```text
--list|-l
--module|-m NAME
--runs|-r N
--size|-n N
```

`--threads|-t` 可以保留为通用参数，但串行 LU 第一版应明确忽略或拒绝该参数。

目标可执行文件：

```bash
./build/lu
```

---

## 6. CMake 重建方案

顶层 `CMakeLists.txt` 负责：

```cmake
add_subdirectory(framework)
add_subdirectory(matmul)
add_subdirectory(lu)
```

framework 编译为静态库，例如：

```text
kernel_framework
```

两个工具分别链接该库：

```text
matmul -> kernel_framework
lu     -> kernel_framework
```

OpenMP 只在 `matmul` 目标中按需启用，不应让 LU 工具被迫依赖 OpenMP。

矩阵尺寸配置也要按工具划分，避免把矩阵乘法专用宏名泄漏到 framework。可以保留编译期固定尺寸，也可以在工具层逐步迁移到命令行尺寸；迁移过程中必须明确两者的优先级。

---

## 7. Git 重建计划

本次重建需要重新组织 Git 历史，但在执行破坏性操作前必须先保留当前状态。

### 7.1 保护当前工作区

首先记录当前状态：

```bash
git status --short
git diff --stat
git diff
git log --oneline --decorate -10
```

当前工作区存在未提交的 LU 文件、README 和 CMake 修改，同时存在已删除的开发容器文件。重建前必须确认这些改动是否全部保留，不能直接丢弃。

### 7.2 创建保护分支或标签

在清理历史前创建一个本地保护引用，例如：

```bash
git branch backup/pre-framework-rebuild
```

如果需要固定当前提交，也可以额外创建标签。保护引用创建后，先确认：

```bash
git show-ref backup/pre-framework-rebuild
```

### 7.3 推荐的新提交序列

重建后的 Git 历史建议保持为以下小步提交：

1. `docs: document framework rebuild plan`
   - 只提交本文件。

2. `chore: create framework and application layout`
   - 创建 `framework/`、`matmul/`、`lu/` 目录及各自 CMake 骨架；
   - 此时可以暂时没有完整算法实现，但目录和目标名称应明确。

3. `refactor: extract shared registry and timer framework`
   - 抽取注册表、计时器和通用矩阵辅助；
   - framework 能被独立链接。

4. `refactor: migrate matmul to shared framework`
   - 迁移 naive、OpenMP、矩阵乘法主程序和验证器；
   - `matmul` 行为与迁移前保持一致。

5. `refactor: use explicit algorithm registration`
   - 删除对 constructor 自动注册的依赖；
   - matmul 显式注册自己的算法。

6. `feat: add standalone serial lu application`
   - 添加 LU 工具、输入生成器、packed LU 算法和验证器；
   - `lu` 能独立构建和运行。

7. `docs: update build and usage instructions`
   - 更新 README、命令示例、实验说明和验证预期。

每个提交完成后都应能够构建，至少不要留下无法恢复的中间状态。

### 7.4 是否重写历史

“Git 要重建”需要在真正执行前选择具体方式：

- **推荐方式：保留旧历史，新建重构提交**
  - 风险低；
  - 可以通过保护分支回看旧实现；
  - 适合课程项目和协作仓库。

- **破坏性重写：建立全新的 orphan history**
  - 只保留重建后的文件；
  - 旧提交不再位于默认分支历史中；
  - 必须先创建保护分支；
  - 如果远程已有协作者，需要额外协调并避免未经确认强制推送。

在没有明确收到“重写为 orphan history”指令前，默认采用第一种方式，不执行 `git reset --hard`、`git checkout -- .`、`git clean -fd` 或强制推送。

---

## 8. 分阶段验收标准

### 阶段 A：框架骨架

- 顶层 CMake 能配置；
- `framework` 能编译为库；
- `matmul` 和 `lu` 目标存在；
- 没有引入未声明的隐式依赖。

### 阶段 B：matmul 迁移

```bash
cmake -S . -B build
cmake --build build
./build/matmul --list
./build/matmul -m naive -n 4 -r 1
```

验收：

- naive 正常列出并运行；
- OpenMP 在环境支持时正常列出并运行；
- naive 验证 PASS；
- 迁移前后的输出语义一致。

### 阶段 C：LU 工具

```bash
./build/lu --list
./build/lu -m serial_lu -n 4 -r 1
```

验收：

- `serial_lu` 能显式注册；
- 输入矩阵不会因为零主元导致未定义行为；
- 输出采用约定的 packed LU 格式；
- `L * U ≈ A` 验证 PASS；
- 计时不包含初始化和验证。

### 阶段 D：回归与文档

- 两个工具都能从干净 build 目录构建；
- README 包含安装、构建、运行和验证命令；
- Git 工作区只包含有意修改；
- 保护分支仍然存在，直到重建确认完成。

---

## 9. 当前暂不处理的内容

以下事项不放在第一轮重建中：

- 不立即实现 LU 的 OpenMP 版本；
- 不立即实现分块 LU；
- 不立即把所有命令行解析抽象成复杂的通用库；
- 不把统计脚本强行改成同时支持两种工具；
- 不在没有测试依据的情况下改变数值误差阈值；
- 不在未确认前删除当前旧目录或执行强制 Git 操作。

---

## 10. 开始实施前的确认项

1. Git 重建采用“保留旧历史并新增重构提交”，还是“新建 orphan history”？
2. LU 第一版是否要求部分选主元？
3. 矩阵尺寸继续使用编译期配置，还是优先迁移为命令行运行期配置？
4. 当前 `stats/` 实验脚本是否纳入本次重建，还是在两个工具稳定后再迁移？

在这些问题确认前，本文件只作为修改计划，不执行破坏性 Git 历史操作。
