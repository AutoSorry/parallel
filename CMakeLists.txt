cmake_minimum_required(VERSION 3.16)

project(matrix_kernels C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_subdirectory(framework)
add_subdirectory(matmul)
add_subdirectory(lu)
