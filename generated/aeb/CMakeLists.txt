# GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
# Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
# Project:   aeb (pattern: can_function)
# Inputs:    sha256:a08ca97b0dae15cd9bd7ef48fa8166ab2ed0041549faf134197c8d1be4271543
# Regenerate: python3 -m tools.gen

add_library(aeb STATIC
    src/aeb_controller.cpp
    src/aeb_controller_logic.cpp
    src/aeb_controller_codec.cpp)
target_include_directories(aeb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(aeb PUBLIC ea::runtime PRIVATE ea::profile_asil)

if(EA_BUILD_TESTS)
    ea_add_gtest(aeb_sil
        SOURCES sil/aeb_controller_sil_test.cpp
        LIBS aeb ea::plant)
    target_include_directories(aeb_sil PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif()
