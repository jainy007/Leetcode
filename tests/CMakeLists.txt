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
