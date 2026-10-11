# GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
# Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
# Project:   aeb_monitor (pattern: can_function)
# Inputs:    sha256:4edac0782e43a3effd02948398696bdcee44fdc1992b83dad1769ef283390f0c
# Regenerate: python3 -m tools.gen

add_library(aeb_monitor STATIC
    src/aeb_monitor.cpp
    src/aeb_monitor_logic.cpp
    src/aeb_monitor_codec.cpp)
target_include_directories(aeb_monitor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(aeb_monitor PUBLIC ea::runtime PRIVATE ea::profile_asil)

if(EA_BUILD_TESTS)
    ea_add_gtest(aeb_monitor_sil
        SOURCES sil/aeb_monitor_sil_test.cpp
        LIBS aeb_monitor ea::plant)
    target_include_directories(aeb_monitor_sil PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif()
