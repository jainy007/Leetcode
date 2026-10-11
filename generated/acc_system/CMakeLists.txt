# GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
# Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
# Project:   acc_system (pattern: can_system)
# Inputs:    sha256:bcf75ddb317884072eb7fb43b43c3450882edb36edd84b7a99e749e56ad81f26
# Regenerate: python3 -m tools.gen

# System SIL only: the product code is in the node projects acc and acc_monitor.
if(EA_BUILD_TESTS)
    ea_add_gtest(acc_system_sil
        SOURCES sil/acc_system_sil_test.cpp
        LIBS acc acc_monitor ea::plant)
    target_include_directories(acc_system_sil PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif()
