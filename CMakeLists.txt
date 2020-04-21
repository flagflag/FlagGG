cmake_minimum_required(VERSION 3.11)
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
if ((NOT WIN32) AND (NOT WIN64))
    set(CMAKE_CXX_STANDARD 11)
    message ("unix set std=c++11")

    # 编译参数
    set(CXX_FLAGS
        -g
        -Wall
        )
    string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
    message ("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
endif ()

if (FLAGGG_LIB_TYPE STREQUAL STATIC)
	add_definitions (-DFlagGG_STATIC_DEFINE)
endif()

project(FlagGG)

include (${CMAKE_CURRENT_SOURCE_DIR}/CMake/Common.cmake)

add_subdirectory (3rd)
add_subdirectory (FlagGG)
add_subdirectory (Samples)
add_subdirectory (Application)
add_subdirectory (Tools)
