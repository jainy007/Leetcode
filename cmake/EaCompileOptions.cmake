# Compile profiles.
#
#   ea::profile_asil  - strict profile for ASIL-rated runtime and generated code
#                       (no exceptions, no RTTI, all conversions explicit, warnings are errors)
#   ea::profile_qm    - relaxed profile for QM code (IVI, tooling glue); still warning-clean
#
# Rule references: docs/rules/cpu.md (CPU-xxx).

add_library(ea_profile_common INTERFACE)
add_library(ea::profile_common ALIAS ea_profile_common)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(ea_profile_common INTERFACE
        -Wall -Wextra -Wpedantic -Werror
        -Wshadow -Wconversion -Wsign-conversion -Wcast-align
        -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual
        -Wnull-dereference -Wdouble-promotion -Wformat=2
        -Wimplicit-fallthrough)
endif()

add_library(ea_profile_asil INTERFACE)
add_library(ea::profile_asil ALIAS ea_profile_asil)
target_link_libraries(ea_profile_asil INTERFACE ea_profile_common)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # CPU-001: no exceptions, CPU-002: no RTTI on ASIL code paths.
    target_compile_options(ea_profile_asil INTERFACE
        $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions -fno-rtti>)
endif()
target_compile_definitions(ea_profile_asil INTERFACE EA_PROFILE_ASIL=1)

add_library(ea_profile_qm INTERFACE)
add_library(ea::profile_qm ALIAS ea_profile_qm)
target_link_libraries(ea_profile_qm INTERFACE ea_profile_common)
target_compile_definitions(ea_profile_qm INTERFACE EA_PROFILE_QM=1)

# Sanitizers (host builds only).
if(EA_SANITIZER)
    string(REPLACE ";" "," _ea_san "${EA_SANITIZER}")
    add_compile_options(-fsanitize=${_ea_san} -fno-omit-frame-pointer -fno-sanitize-recover=all)
    add_link_options(-fsanitize=${_ea_san})
    if("undefined" IN_LIST EA_SANITIZER)
        # The vptr check needs RTTI, which the ASIL profile disables (CPU-002).
        add_compile_options(-fno-sanitize=vptr)
    endif()
endif()

# Coverage. MC/DC needs clang >= 18.
if(EA_COVERAGE)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR "EA_COVERAGE requires clang (MC/DC via -fcoverage-mcdc)")
    endif()
    add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 18)
        add_compile_options(-fcoverage-mcdc)
    endif()
    add_link_options(-fprofile-instr-generate)
endif()
