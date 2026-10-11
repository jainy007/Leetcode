# Hand-written SIL support (virtual time, fault injection, temp files). Test-only; never linked
# into production targets.
add_library(ea_sil STATIC
    src/fault_injecting_backend.cpp
    src/temp_dir.cpp)
target_include_directories(ea_sil PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(ea_sil PUBLIC ea::runtime PRIVATE ea::profile_qm)
add_library(ea::sil ALIAS ea_sil)

# Closed-loop plant for ACC SIL (vehicle dynamics, radar, driver, CAN timing, metrics).
add_library(ea_plant STATIC plant/src/acc_plant.cpp)
target_include_directories(ea_plant PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/plant/include)
target_link_libraries(ea_plant PUBLIC ea::runtime PRIVATE ea::profile_qm)
add_library(ea::plant ALIAS ea_plant)
