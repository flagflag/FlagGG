set (TARGET_NAME ProfilerGenerator)
set (EXECUTABLE_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)
list (APPEND TARGET_PROPERTIES FOLDER "Tools")

setup_lint ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

define_source_files (RECURSE GROUP)

setup_main_executable ()

setup_test ()

