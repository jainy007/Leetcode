# Cross-compile for aarch64 Linux (PREEMPT_RT), e.g. generic automotive SoC or NVIDIA DRIVE OS Linux.
# Usage: cmake --preset linux-aarch64-rt -DEA_SYSROOT=/path/to/sysroot
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
if(DEFINED EA_SYSROOT)
    set(CMAKE_SYSROOT ${EA_SYSROOT})
endif()
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
