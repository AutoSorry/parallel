add_library(kernel_framework STATIC
    src/registry.c
    src/timer.c
)

target_include_directories(kernel_framework PUBLIC include)
target_compile_options(kernel_framework PRIVATE -O2 -Wall -Wextra)
