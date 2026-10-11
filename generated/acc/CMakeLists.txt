# GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
# Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
# Project:   acc (pattern: can_function)
# Inputs:    sha256:fbfce52a7d6bd218de02fbc84f6ae65f4d57f340161623b43290af8f8ea21a6d
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
