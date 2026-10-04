cmake_minimum_required(VERSION 3.16)

project(matmul C)

if(NOT CMAKE_C_COMPILER_ID STREQUAL "GNU")
    message(FATAL_ERROR "This project requires GCC (GNU C compiler); detected ${CMAKE_C_COMPILER_ID}: ${CMAKE_C_COMPILER}")
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(OpenMP REQUIRED COMPONENTS C)

add_subdirectory(framework)
add_subdirectory(matmul)
