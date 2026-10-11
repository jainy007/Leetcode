# GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
# Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
# Project:   aeb_system (pattern: can_system)
# Inputs:    sha256:178716b09b759745eca874eeb1b20fa3a3e080bfff379eaf556512fc8cfefc60
# Regenerate: python3 -m tools.gen

# System SIL only: the product code is in the node projects aeb and aeb_monitor.
if(EA_BUILD_TESTS)
    ea_add_gtest(aeb_system_sil
        SOURCES sil/aeb_system_sil_test.cpp
        LIBS aeb aeb_monitor ea::plant)
    target_include_directories(aeb_system_sil PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif()
