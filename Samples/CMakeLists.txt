include (${CMAKE_CURRENT_SOURCE_DIR}/../CMake/ThirdParty.cmake)

add_subdirectory (NetworkTest)
if (WIN32 OR WIN64)
    add_subdirectory (Direct3D_Cube)
endif ()
add_subdirectory (Test)
add_subdirectory (LuaTest)
add_subdirectory (ThreadTest)
