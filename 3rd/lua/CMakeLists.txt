# Define target name
set (TARGET_NAME lua53)
set (LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)
list (APPEND TARGET_PROPERTIES FOLDER "3rd")

setup_lint ()

# Define source files
define_source_files (RECURSE GROUP GLOB_CPP_PATTERNS *.c EXCLUDE_PATTERNS luac.c)

# Define dependency libs
set (INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

if (FLAGGG_LIB_TYPE STREQUAL SHARED)
    setup_library (SHARED)
else()
    setup_library (STATIC)
endif()

set_target_properties(lua53 PROPERTIES LINKER_LANGUAGE C)

if (IOS)
	xcode_setup(${TARGET_NAME})
endif ()

# Setup test cases
setup_test ()