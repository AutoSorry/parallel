set(LU_MATRIX_N "1024" CACHE STRING "default LU matrix size")

file(GLOB LU_SOURCES CONFIGURE_DEPENDS
    src/*.c
    algorithms/*.c
)

add_executable(lu ${LU_SOURCES})

target_include_directories(lu PRIVATE include)
target_compile_definitions(lu PRIVATE MATRIX_N=${LU_MATRIX_N})
target_compile_options(lu PRIVATE -O2 -Wall -Wextra)
target_link_libraries(lu PRIVATE kernel_framework m)
