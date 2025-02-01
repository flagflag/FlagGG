set (TARGET_NAME UltralightCore)
set (LIBRARY_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)
set (FLAGGG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../FlagGG)
set (APP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../)
list (APPEND TARGET_PROPERTIES FOLDER "Application/Plugins")

setup_lint ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR})
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../)

include_directories (${FLAGGG_DIR})
include_directories (${APP_DIR})

add_definitions (-DULTRALIGHT_IMPLEMENTATION=1)

define_source_files (RECURSE GROUP)

if (FLAGGG_LIB_TYPE STREQUAL SHARED)
    setup_library (SHARED)
else()
    setup_library (STATIC)
endif()

target_link_libraries (${TARGET_NAME} FlagGG)

setup_test ()

