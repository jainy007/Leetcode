# GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
# Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
# Project:   gpu_inference_stage (pattern: gpu_stage)
# Inputs:    sha256:2be61671649d30f44ded1262d2e27e019e3c0a654d49620f43cd33449f31fbef
# Regenerate: python3 -m tools.gen

# Types shared by producer and monitor (the only coupling between the two processes).
add_library(gpu_inference_stage_types INTERFACE)
target_include_directories(gpu_inference_stage_types INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Producer: ObstacleScoreStage (ASIL QM)
add_library(gpu_inference_stage STATIC src/obstacle_score_stage.cpp)
target_link_libraries(gpu_inference_stage PUBLIC gpu_inference_stage_types ea::runtime PRIVATE ea::profile_asil)

# Monitor: ObstacleScoreMonitor (ASIL B); does not link the producer (MP-001).
add_library(gpu_inference_stage_monitor STATIC src/obstacle_score_monitor.cpp)
target_link_libraries(gpu_inference_stage_monitor PUBLIC gpu_inference_stage_types ea::runtime PRIVATE ea::profile_asil)

if(EA_BUILD_TESTS)
    ea_add_gtest(gpu_inference_stage_sil
        SOURCES sil/obstacle_score_stage_sil_test.cpp
        LIBS gpu_inference_stage gpu_inference_stage_monitor ea::sil)
    target_include_directories(gpu_inference_stage_sil PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_definitions(gpu_inference_stage_sil PRIVATE EA_SIL_MODEL_DIR="${PROJECT_SOURCE_DIR}/examples/gpu_inference_stage")
endif()
