set (TARGET_NAME Editor)
set (EXECUTABLE_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)
set (FLAGGG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../FlagGG)
list (APPEND TARGET_PROPERTIES FOLDER "Application")

setup_lint ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

# find_package (FlagGG REQUIRED)
include_directories (${FLAGGG_DIR})

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../3rd/Assimp/include)

define_source_files (RECURSE GROUP)

setup_main_executable ()

target_link_libraries(${TARGET_NAME} FlagGG Assimp)

setup_test ()

