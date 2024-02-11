set (TARGET_NAME Game)
set (LIBRARY_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)
set (FLAGGG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../FlagGG)
set (APP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../)
list (APPEND TARGET_PROPERTIES FOLDER "Application/Game")

setup_lint ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

# find_package (FlagGG REQUIRED)
include_directories (${FLAGGG_DIR})
include_directories (${APP_DIR})

if (NOT FLAGGG_D3D11)
    list (APPEND EXCLUDED_SOURCE_DIRS Unit)
endif ()
# 输出排除的目录
message ("exclude folder: ${EXCLUDED_SOURCE_DIRS}")
string (REPLACE ";" "/[^;]+;" EXCLUDE_PATTERNS "${EXCLUDED_SOURCE_DIRS};")
define_source_files (EXCLUDE_PATTERNS ${EXCLUDE_PATTERNS} RECURSE GROUP)

if (FLAGGG_LIB_TYPE STREQUAL SHARED)
    setup_library (SHARED)
else()
    setup_library (STATIC)
endif()

if (FLAGGG_PROTO)
	add_definitions (-DFLAGGG_PROTO)
	target_link_libraries (${TARGET_NAME} libprotobuf FlagGG)
else ()
	target_link_libraries (${TARGET_NAME} FlagGG)
endif ()

target_link_libraries (${TARGET_NAME} LuaGameEngine)

setup_test ()

