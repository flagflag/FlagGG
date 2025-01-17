cmake_minimum_required(VERSION 2.8.12)

project(stripper)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_executable(stripper
    ${CMAKE_CURRENT_SOURCE_DIR}/stripper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

if(WIN32)
    set_target_properties(stripper PROPERTIES FOLDER "3rd/spirv-reflect/util")
endif()
