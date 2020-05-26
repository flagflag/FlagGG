set (TARGET_NAME GameReport)
set (EXECUTABLE_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)
set (FLAGGG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../FlagGG)
list (APPEND TARGET_PROPERTIES FOLDER "Tools")

setup_lint ()

add_definitions (-DGameReport_EXPORTS)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

# find_package (FlagGG REQUIRED)
include_directories (${FLAGGG_DIR})

define_source_files (RECURSE GROUP)

setup_library (SHARED)

target_link_libraries(${TARGET_NAME} FlagGG)

setup_test ()

