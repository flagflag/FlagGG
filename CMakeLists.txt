cmake_minimum_required(VERSION 3.13)
if ((NOT WIN32) AND (NOT WIN64))
    set(CMAKE_CXX_STANDARD 11)
    message ("unix set std=c++11")
endif ()

add_subdirectory (FlagGG)
add_subdirectory (Samples)
