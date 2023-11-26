# Define target name
set (TARGET_NAME PugiXml)
set (LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)
list (APPEND TARGET_PROPERTIES FOLDER "3rd")

setup_lint ()

# Define source files
define_source_files (GLOB_CPP_PATTERNS *.cpp GLOB_H_PATTERNS *.hpp)

# Define dependency libs
set (INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

setup_library (STATIC)

if (IOS)
	xcode_setup(${TARGET_NAME})
endif ()

# Setup test cases
setup_test ()
