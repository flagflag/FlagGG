include (${CMAKE_CURRENT_SOURCE_DIR}/../CMake/ThirdParty.cmake)

add_subdirectory (Network_TCP)
if (WIN32 OR WIN64)
    add_subdirectory (Direct3D_Cube)
endif ()
