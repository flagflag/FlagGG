set (TARGET_NAME Editor)
set (LIBRARY_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)
set (FLAGGG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../FlagGG)
list (APPEND TARGET_PROPERTIES FOLDER "Application")

setup_lint ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

# find_package (FlagGG REQUIRED)
include_directories (${FLAGGG_DIR})

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../3rd/Assimp/include)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../)

define_source_files (RECURSE GROUP)

if (FLAGGG_LIB_TYPE STREQUAL SHARED)
    setup_library (SHARED)
else()
    setup_library (STATIC)
endif()

target_link_libraries(${TARGET_NAME} FlagGG Assimp CoreCLRHelper)

setup_test ()

