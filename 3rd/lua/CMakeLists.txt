# Define target name
set (TARGET_NAME lua53)
set (LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)
list (APPEND TARGET_PROPERTIES FOLDER "3rd")

setup_lint ()

# define_source_files这个宏定义在UrhoCommon.cmake，参数什么意思去那个文件里看
define_source_files (RECURSE GROUP GLOB_CPP_PATTERNS *.c EXCLUDE_PATTERNS luac.c)

# Define dependency libs
set (INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

# 也是个宏，见UrhoCommon.cmake
#if (FLAGGG_LIB_TYPE STREQUAL SHARED)
#    setup_library (SHARED)
#else()
#    setup_library (STATIC)
#endif()
setup_library (SHARED)

set_target_properties(lua53 PROPERTIES LINKER_LANGUAGE C)

# Setup test cases
setup_test ()