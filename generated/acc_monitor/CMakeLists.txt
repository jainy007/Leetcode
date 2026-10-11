# GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
# Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
# Project:   acc_monitor (pattern: can_function)
# Inputs:    sha256:aaed2b82deaf8ac2890ea08cf061dfa671e7be14789da61b08e9a29f91792b3a
# Regenerate: python3 -m tools.gen

add_library(acc_monitor STATIC
    src/acc_monitor.cpp
    src/acc_monitor_logic.cpp
    src/acc_monitor_codec.cpp)
target_include_directories(acc_monitor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(acc_monitor PUBLIC ea::runtime PRIVATE ea::profile_asil)

if(EA_BUILD_TESTS)
    ea_add_gtest(acc_monitor_sil
        SOURCES sil/acc_monitor_sil_test.cpp
        LIBS acc_monitor ea::plant)
    target_include_directories(acc_monitor_sil PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif()
