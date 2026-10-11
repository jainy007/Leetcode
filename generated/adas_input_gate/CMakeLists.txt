# GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
# Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
# Project:   adas_input_gate (pattern: input_gate)
# Inputs:    sha256:c2ae8363c4d06ee0cb2ee9ec885959083cf8629b1d677189a6ad2542626ab1f5
# Regenerate: python3 -m tools.gen

add_library(adas_input_gate STATIC
    src/adas_input_gate.cpp
    src/adas_input_gate_codec.cpp
    src/adas_input_gate_task.cpp)
target_include_directories(adas_input_gate PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(adas_input_gate PUBLIC ea::runtime PRIVATE ea::profile_asil)

if(EA_BUILD_TESTS)
    ea_add_gtest(adas_input_gate_sil
        SOURCES sil/adas_input_gate_sil_test.cpp
        LIBS adas_input_gate ea::sil)
    target_include_directories(adas_input_gate_sil PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif()
