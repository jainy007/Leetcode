# Qualcomm QNN backend (Snapdragon Ride). Placeholder until the QNN SDK integration lands.
set(EA_QNN_SDK_ROOT "$ENV{QNN_SDK_ROOT}" CACHE PATH "Qualcomm QNN SDK root")
add_library(ea_gpu_qnn STATIC qnn_backend.cpp)
target_include_directories(ea_gpu_qnn PUBLIC ${PROJECT_SOURCE_DIR}/runtime/include)
if(EA_QNN_SDK_ROOT)
    target_include_directories(ea_gpu_qnn PRIVATE ${EA_QNN_SDK_ROOT}/include/QNN)
endif()
target_link_libraries(ea_gpu_qnn PUBLIC ea::runtime PRIVATE ea::profile_asil)
add_library(ea::gpu_qnn ALIAS ea_gpu_qnn)
