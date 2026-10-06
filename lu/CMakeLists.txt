file(GLOB LU_SOURCES CONFIGURE_DEPENDS
    src/*.c
    algorithms/*.c
)

add_executable(lu ${LU_SOURCES})

target_include_directories(lu PRIVATE include)
target_compile_options(lu PRIVATE -O2 -Wall -Wextra)
set_target_properties(lu PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
target_link_libraries(lu PRIVATE kernel_framework m OpenMP::OpenMP_C)
