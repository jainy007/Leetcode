# GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
# Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
# Project:   acc (pattern: can_function)
# Inputs:    sha256:7daf72a7ff67fdde3af69c03ce9ff58b4ecdb3b7244d701c28de0793bc835631
# Regenerate: python3 -m tools.gen

add_library(acc STATIC
    src/acc_controller.cpp
    src/acc_controller_logic.cpp
    src/acc_controller_codec.cpp)
target_include_directories(acc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(acc PUBLIC ea::runtime PRIVATE ea::profile_asil)

if(EA_BUILD_TESTS)
    ea_add_gtest(acc_sil
        SOURCES sil/acc_controller_sil_test.cpp
        LIBS acc ea::plant)
    target_include_directories(acc_sil PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif()
