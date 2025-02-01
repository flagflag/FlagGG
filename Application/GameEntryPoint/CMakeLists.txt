set (TARGET_NAME GameEntryPoint)
set (EXECUTABLE_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)
set (FLAGGG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../FlagGG)
set (APP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../)
list (APPEND TARGET_PROPERTIES FOLDER "Application/Game")

setup_lint ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR})
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../)

include_directories (${FLAGGG_DIR})
include_directories (${APP_DIR})

define_source_files (RECURSE GROUP)

link_directories (${CMAKE_CURRENT_SOURCE_DIR}/../UltralightSDK/lib)

setup_main_executable ()

target_link_libraries (${TARGET_NAME} Game)

setup_test ()

