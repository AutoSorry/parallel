file(GLOB MATMUL_SOURCES CONFIGURE_DEPENDS
    src/*.c
    algorithms/*.c
)

add_executable(matmul ${MATMUL_SOURCES})
set_target_properties(matmul PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

target_include_directories(matmul PRIVATE include)
target_compile_options(matmul PRIVATE -O2 -Wall -Wextra)
target_link_libraries(matmul PRIVATE kernel_framework OpenMP::OpenMP_C m)
