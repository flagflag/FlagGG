cmake_minimum_required(VERSION 3.11)

project(examples)

list(APPEND SPIRV_REFLECT_FILES
  ${SPIRV_ROOT}/spirv_reflect.h
  ${SPIRV_ROOT}/spirv_reflect.c
)

list(APPEND COMMON_FILES
  common.h
  common.cpp
  ../common/output_stream.h
  ../common/output_stream.cpp
  sample_spv.h
)

################################################################################
# descriptors
################################################################################
add_executable(descriptors ${CMAKE_CURRENT_SOURCE_DIR}/main_descriptors.cpp ${COMMON_FILES} ${SPIRV_REFLECT_FILES})
target_include_directories(descriptors PRIVATE ${SPIRV_ROOT})
if (${VULKAN_DIR_FOUND})
    target_compile_definitions(descriptors PRIVATE SPIRV_REFLECT_HAS_VULKAN_H)
    target_include_directories(descriptors PRIVATE ${VULKAN_DIR}/include)
    set_target_properties(descriptors PROPERTIES CXX_STANDARD 11)
endif()
if(WIN32)
    set_target_properties(descriptors PROPERTIES FOLDER "3rd/spirv-reflect/examples")
endif()

################################################################################
# io_variables
################################################################################
add_executable(io_variables ${CMAKE_CURRENT_SOURCE_DIR}/main_io_variables.cpp ${COMMON_FILES} ${SPIRV_REFLECT_FILES})
target_include_directories(io_variables PRIVATE ${SPIRV_ROOT})
if (${VULKAN_DIR_FOUND})
    target_compile_definitions(io_variables PRIVATE SPIRV_REFLECT_HAS_VULKAN_H)
    target_include_directories(io_variables PRIVATE ${VULKAN_DIR}/include)
    set_target_properties(io_variables PROPERTIES CXX_STANDARD 11)
endif()
if(WIN32)
    set_target_properties(io_variables PROPERTIES FOLDER "3rd/spirv-reflect/examples")
endif()

################################################################################
# hlsl_resource_types
################################################################################
add_executable(hlsl_resource_types ${CMAKE_CURRENT_SOURCE_DIR}/main_hlsl_resource_types.cpp ${COMMON_FILES} ${SPIRV_REFLECT_FILES})
target_include_directories(hlsl_resource_types PRIVATE ${SPIRV_ROOT})
if (${VULKAN_DIR_FOUND})
    target_compile_definitions(hlsl_resource_types PRIVATE SPIRV_REFLECT_HAS_VULKAN_H)
    target_include_directories(hlsl_resource_types PRIVATE ${VULKAN_DIR}/include)
    set_target_properties(hlsl_resource_types PROPERTIES CXX_STANDARD 11)
endif()
if(WIN32)
    set_target_properties(hlsl_resource_types PROPERTIES FOLDER "3rd/spirv-reflect/examples")
endif()