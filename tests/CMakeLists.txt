ea_add_gtest(runtime_unit_tests
    SOURCES
        runtime/result_test.cpp
        runtime/crc_sha_test.cpp
        runtime/bits_test.cpp
        runtime/someip_e2e_test.cpp
        runtime/ipc_test.cpp
        runtime/supervision_test.cpp
        runtime/osal_test.cpp
        runtime/gpu_cpu_ref_test.cpp
    LIBS ea::runtime)

# Generator gates (also run by CI): generated/ is current, and the qualification suite passes.
add_test(NAME generator_check
    COMMAND ${Python3_EXECUTABLE} -m tools.gen --check
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
add_test(NAME generator_qualification
    COMMAND ${Python3_EXECUTABLE} -m unittest discover -s tools/gen/tests -t .
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
