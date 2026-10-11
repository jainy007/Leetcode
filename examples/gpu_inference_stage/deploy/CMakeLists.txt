# Two-process deployment of the generated stage and monitor (hand-written glue, QM).
add_executable(obstacle_score_stage_main stage_main.cpp)
target_link_libraries(obstacle_score_stage_main PRIVATE gpu_inference_stage ea::profile_qm)

add_executable(obstacle_score_monitor_main monitor_main.cpp)
target_link_libraries(obstacle_score_monitor_main PRIVATE gpu_inference_stage_monitor ea::profile_qm)

add_test(NAME gpu_inference_stage_two_processes
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/run_two_processes.sh
            $<TARGET_FILE:obstacle_score_stage_main>
            $<TARGET_FILE:obstacle_score_monitor_main>
            ${CMAKE_CURRENT_SOURCE_DIR}/..)
set_tests_properties(gpu_inference_stage_two_processes PROPERTIES TIMEOUT 30)
