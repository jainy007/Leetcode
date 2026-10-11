# Google Benchmark: installed package if available, else a pinned commit (v1.9.1).
find_package(benchmark 1.8 QUIET CONFIG)
if(NOT benchmark_FOUND)
    include(FetchContent)
    FetchContent_Declare(googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG c58e6d0710581e3a08d65c349664128a8d9a2461 # v1.9.1
        GIT_SHALLOW FALSE)
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googlebenchmark)
endif()
