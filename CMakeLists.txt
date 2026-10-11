cmake_minimum_required(VERSION 3.22)

project(embedded_agent
    VERSION 0.1.0
    DESCRIPTION "Rule-based Req -> Gherkin -> Model -> C++ -> SIL toolchain for POSIX performance cores"
    LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------
option(EA_BUILD_TESTS "Build unit and SIL tests" ON)
option(EA_BUILD_BENCHMARKS "Build micro-benchmarks of runtime and generated code" OFF)
option(EA_GPU_CUDA "Build the CUDA backend (needs CUDA toolkit, e.g. NVIDIA DRIVE OS)" OFF)
option(EA_GPU_QNN "Build the Qualcomm QNN backend (needs QNN SDK, e.g. Snapdragon Ride)" OFF)
option(EA_COVERAGE "Instrument for statement/branch/MC-DC coverage (clang only)" OFF)
set(EA_SANITIZER "" CACHE STRING "Sanitizer to enable: address;undefined | thread | empty")
set(EA_OSAL "posix" CACHE STRING "OS abstraction implementation: posix | qnx")

include(EaCompileOptions)

# ---------------------------------------------------------------------------
# Python (code generator, linters, trace tool)
# ---------------------------------------------------------------------------
find_package(Python3 3.10 REQUIRED COMPONENTS Interpreter)

# ---------------------------------------------------------------------------
# Hand-written runtime (reviewed once, reused by all generated code)
# ---------------------------------------------------------------------------
add_subdirectory(runtime)

if(EA_BUILD_TESTS)
    enable_testing()
    include(EaGoogleTest)
    add_subdirectory(sil)
endif()

# ---------------------------------------------------------------------------
# Generated code (committed; CI regenerates and fails on any diff)
# ---------------------------------------------------------------------------
add_subdirectory(generated)

if(EA_BUILD_TESTS)
    add_subdirectory(tests)
    add_subdirectory(examples/gpu_inference_stage/deploy)
endif()

if(EA_BUILD_BENCHMARKS)
    include(EaBenchmark)
    add_subdirectory(bench)
endif()
