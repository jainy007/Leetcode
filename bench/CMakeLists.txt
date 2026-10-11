# Micro-benchmarks for hot paths. Not part of the safety argument; used to guide optimization.
add_executable(ea_bench
    runtime_bench.cpp
    generated_bench.cpp)
target_link_libraries(ea_bench PRIVATE
    ea::runtime adas_input_gate gpu_inference_stage gpu_inference_stage_monitor
    benchmark::benchmark_main ea::profile_common)
target_include_directories(ea_bench PRIVATE
    ${PROJECT_SOURCE_DIR}/generated/adas_input_gate
    ${PROJECT_SOURCE_DIR}/generated/gpu_inference_stage)
target_compile_definitions(ea_bench PRIVATE
    EA_BENCH_MODEL_DIR="${PROJECT_SOURCE_DIR}/examples/gpu_inference_stage")
