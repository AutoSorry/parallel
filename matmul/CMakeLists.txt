cmake_minimum_required(VERSION 3.16)

project(matrix-mul C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(DEFAULT_MATRIX_N "1024" CACHE STRING "default matrix size")

file(GLOB SRC_FILES CONFIGURE_DEPENDS src/*.c algorithms/*.c)

add_executable(matmul ${SRC_FILES})

target_include_directories(matmul PRIVATE include src)
target_compile_definitions(matmul PRIVATE DEFAULT_MATRIX_N=${DEFAULT_MATRIX_N})

target_compile_options(matmul PRIVATE -O2 -Wall -Wextra)
target_link_libraries(matmul PRIVATE m)

if(APPLE AND CMAKE_C_COMPILER_ID MATCHES "Clang")
    find_path(LIBOMP_INCLUDE_DIR omp.h
        HINTS /opt/homebrew/opt/libomp/include /usr/local/opt/libomp/include)
    find_library(LIBOMP_LIBRARY omp
        HINTS /opt/homebrew/opt/libomp/lib /usr/local/opt/libomp/lib)

    if(LIBOMP_INCLUDE_DIR AND LIBOMP_LIBRARY)
        target_compile_options(matmul PRIVATE -Xpreprocessor -fopenmp)
        target_include_directories(matmul PRIVATE ${LIBOMP_INCLUDE_DIR})
        target_link_libraries(matmul PRIVATE ${LIBOMP_LIBRARY})
        message(STATUS "OpenMP enabled via libomp: ${LIBOMP_LIBRARY}")
    else()
        message(WARNING "libomp was not found; OpenMP pragmas will be ignored by the compiler")
    endif()
else()
    find_package(OpenMP QUIET)
    if(OpenMP_C_FOUND)
        target_link_libraries(matmul PRIVATE OpenMP::OpenMP_C)
    else()
        message(WARNING "OpenMP was not found; OpenMP pragmas will be ignored by the compiler")
    endif()
endif()
