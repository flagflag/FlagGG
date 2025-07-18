# Define target name
set (TARGET_NAME mtlpp)
set (LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)
list (APPEND TARGET_PROPERTIES FOLDER "3rd")

setup_lint ()

# Define source files
list (APPEND EXCLUDED_SOURCE_DIRS "examples")
string (REPLACE ";" "/[^;]+;" EXCLUDE_PATTERNS "${EXCLUDED_SOURCE_DIRS};")
define_source_files (EXCLUDE_PATTERNS ${EXCLUDE_PATTERNS} RECURSE GROUP GLOB_CPP_PATTERNS *.mm GLOB_H_PATTERNS *.hpp)

# Define dependency libs
set (INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

setup_library (STATIC)

target_link_libraries(${TARGET_NAME} PRIVATE "-framework CoreFoundation")
target_link_libraries(${TARGET_NAME} PRIVATE "-framework Foundation")
target_link_libraries(${TARGET_NAME} PRIVATE "-framework Metal")

if (IOS)
	xcode_setup(${TARGET_NAME})
endif ()

# Setup test cases
setup_test ()
