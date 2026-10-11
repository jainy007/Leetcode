# GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
# Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
# Project:   acc (pattern: acc_controller)
# Inputs:    sha256:cce2b25a21d5b0053b9588be55a9c9be1a00be09e870c4dedb9dbbf5638be09e
# Regenerate: python3 -m tools.gen

add_library(acc STATIC
    src/acc_controller.cpp
    src/acc_controller_codec.cpp)
target_include_directories(acc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(acc PUBLIC ea::runtime PRIVATE ea::profile_asil)

if(EA_BUILD_TESTS)
    ea_add_gtest(acc_sil
        SOURCES sil/acc_controller_sil_test.cpp
        LIBS acc ea::plant)
    target_include_directories(acc_sil PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif()
