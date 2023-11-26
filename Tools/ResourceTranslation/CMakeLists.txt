set (TARGET_NAME ResourceTranslation)
set (EXECUTABLE_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)
list (APPEND TARGET_PROPERTIES FOLDER "Tools")

setup_lint ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR})
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../FlagGG)

define_source_files (RECURSE GROUP)

setup_main_executable ()

target_link_libraries(${TARGET_NAME} FlagGG)

setup_test ()

