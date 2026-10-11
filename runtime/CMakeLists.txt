# Hand-written runtime. Every file here is reviewed by a human; generated code depends on it.

add_library(ea_runtime STATIC
    src/core/core.cpp
    src/core/sha256.cpp
    src/codec/bits.cpp
    src/codec/someip.cpp
    src/e2e/p4.cpp
    src/osal/posix/clock.cpp
    src/osal/posix/realtime.cpp
    src/osal/posix/periodic_task.cpp
    src/osal/posix/shared_memory.cpp
    src/supervision/supervision.cpp
    src/gpu/backend.cpp
    src/gpu/cpu_ref_backend.cpp)

if(EA_OSAL STREQUAL "qnx")
    target_sources(ea_runtime PRIVATE src/osal/qnx/affinity.cpp)
elseif(EA_OSAL STREQUAL "posix")
    target_sources(ea_runtime PRIVATE src/osal/linux/affinity.cpp)
else()
    message(FATAL_ERROR "Unknown EA_OSAL '${EA_OSAL}' (expected posix | qnx)")
endif()

target_include_directories(ea_runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(ea_runtime PRIVATE ea::profile_asil)
find_package(Threads REQUIRED)
target_link_libraries(ea_runtime PUBLIC Threads::Threads)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(ea_runtime PUBLIC rt)
endif()
# GPU-007: reference numerics must not depend on compiler FMA contraction.
set_source_files_properties(src/gpu/cpu_ref_backend.cpp PROPERTIES COMPILE_OPTIONS "-ffp-contract=off")
add_library(ea::runtime ALIAS ea_runtime)

if(EA_GPU_CUDA)
    add_subdirectory(src/gpu/cuda)
endif()
if(EA_GPU_QNN)
    add_subdirectory(src/gpu/qnn)
endif()
