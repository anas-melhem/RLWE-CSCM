cmake_minimum_required(VERSION 3.16)
project(RLWE_CSCM VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(rlwe_cscm
    src/polynomial.cpp
    src/rlwe_cscm.cpp
)

target_include_directories(rlwe_cscm PUBLIC include)

target_compile_options(rlwe_cscm PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -O2>
)

add_executable(example_basic examples/basic_demo.cpp)
target_link_libraries(example_basic PRIVATE rlwe_cscm)

add_executable(test_correctness tests/test_correctness.cpp)
target_link_libraries(test_correctness PRIVATE rlwe_cscm)

add_executable(benchmark_ops benchmarks/benchmark_ops.cpp)
target_link_libraries(benchmark_ops PRIVATE rlwe_cscm)

enable_testing()
add_test(NAME correctness COMMAND test_correctness)
