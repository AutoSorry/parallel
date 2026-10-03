# Matrix Kernel Framework

This branch contains the shared C matrix-kernel library. Its public headers
and implementations live under `framework/` and provide a kernel interface,
algorithm registry, timing, and matrix utilities.

Build the static library with CMake and a C compiler:

```bash
cmake -S . -B build
cmake --build build
```

The `matmul` branch contains the matrix multiplication application and its
experiments. The `lu` branch contains the LU decomposition application. Each
branch includes its own copy of the framework and can be built independently.
