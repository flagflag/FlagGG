set (TARGET_NAME Network_TCP)
set (EXECUTABLE_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)
set (FLAGGG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../FlagGG)
list (APPEND TARGET_PROPERTIES FOLDER "Samples")

setup_lint ()

add_definitions (-D_DEBUG)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

# find_package (FlagGG REQUIRED)
include_directories (${FLAGGG_DIR})

define_source_files (RECURSE GROUP)

setup_main_executable ()

target_link_libraries(${TARGET_NAME} FlagGG)

setup_test ()

