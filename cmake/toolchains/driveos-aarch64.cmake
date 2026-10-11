# NVIDIA DRIVE OS (Orin/Thor), Linux flavour. Requires the DRIVE OS SDK toolchain + CUDA.
# Usage: cmake --preset driveos -DEA_DRIVEOS_ROOT=/drive -DEA_GPU_CUDA=ON
include(${CMAKE_CURRENT_LIST_DIR}/linux-aarch64-rt.cmake)
if(DEFINED EA_DRIVEOS_ROOT)
    set(CMAKE_SYSROOT ${EA_DRIVEOS_ROOT}/drive-linux/filesystem/targetfs)
endif()
set(CMAKE_CUDA_ARCHITECTURES 87) # Orin (sm_87); Thor uses 101 with CUDA 12.8+
