set(MATRIX_N "1024" CACHE STRING "default matrix size")

file(GLOB MATMUL_SOURCES CONFIGURE_DEPENDS
    src/*.c
    algorithms/*.c
)

add_executable(matmul ${MATMUL_SOURCES})

target_include_directories(matmul PRIVATE include)
target_compile_definitions(matmul PRIVATE MATRIX_N=${MATRIX_N})
target_compile_options(matmul PRIVATE -O2 -Wall -Wextra)
target_link_libraries(matmul PRIVATE kernel_framework m)

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
