# GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
# Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
# Project:   aeb_system (pattern: can_system)
# Inputs:    sha256:3c38374823eec23e9813f37d7d20dee2e10abbfc8a620b8f99e7ed929b104323
# Regenerate: python3 -m tools.gen

# System SIL only: the product code is in the node projects aeb and aeb_monitor.
if(EA_BUILD_TESTS)
    ea_add_gtest(aeb_system_sil
        SOURCES sil/aeb_system_sil_test.cpp
        LIBS aeb aeb_monitor ea::plant)
    target_include_directories(aeb_system_sil PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif()
