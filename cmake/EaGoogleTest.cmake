# GoogleTest: use an installed package when available (offline / customer builds),
# otherwise fetch a pinned, immutable commit (v1.15.2).
find_package(GTest 1.14 QUIET CONFIG)
if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG b514bdc898e2951020cbdca1304b75f5950d1f59 # v1.15.2
        GIT_SHALLOW FALSE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    if(NOT TARGET GTest::gtest_main)
        add_library(GTest::gtest_main ALIAS gtest_main)
    endif()
endif()
include(GoogleTest)

# ea_add_gtest(<name> SOURCES ... LIBS ...)
function(ea_add_gtest name)
    cmake_parse_arguments(ARG "" "" "SOURCES;LIBS" ${ARGN})
    add_executable(${name} ${ARG_SOURCES})
    target_link_libraries(${name} PRIVATE ${ARG_LIBS} ea::profile_common GTest::gtest_main)
    gtest_discover_tests(${name}
        DISCOVERY_MODE PRE_TEST
        XML_OUTPUT_DIR ${CMAKE_BINARY_DIR}/test-results)
endfunction()
