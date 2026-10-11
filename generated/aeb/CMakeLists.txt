# GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
# Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
# Project:   aeb (pattern: can_function)
# Inputs:    sha256:7985079d35cf79be077da24af2c4f9d16c2c565e0ce9d01d499b7c22d7f7c8d2
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
