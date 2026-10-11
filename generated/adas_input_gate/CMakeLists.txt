# GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
# Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
# Project:   adas_input_gate (pattern: input_gate)
# Inputs:    sha256:e0c4923740db7d928336d8abc9859ca05e37446222ba5a081ee7c47bc53cdec6
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
    ea_add_gtest(adas_input_gate_codec_test
        SOURCES sil/adas_input_gate_codec_test.cpp
        LIBS adas_input_gate)
endif()
