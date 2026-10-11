# CUDA backend (NVIDIA DRIVE OS). Requires the CUDA toolkit (>= 11.4).
enable_language(CUDA)
find_package(CUDAToolkit 11.4 REQUIRED)
add_library(ea_gpu_cuda STATIC cuda_backend.cu)
target_include_directories(ea_gpu_cuda PUBLIC ${PROJECT_SOURCE_DIR}/runtime/include)
target_link_libraries(ea_gpu_cuda PUBLIC ea::runtime CUDA::cudart_static)
set_target_properties(ea_gpu_cuda PROPERTIES CUDA_STANDARD 17 CUDA_STANDARD_REQUIRED ON)
target_compile_options(ea_gpu_cuda PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:--fmad=false>)
add_library(ea::gpu_cuda ALIAS ea_gpu_cuda)
