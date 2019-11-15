set (TARGET_NAME Game)
set (EXECUTABLE_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)
set (FLAGGG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../FlagGG)
list (APPEND TARGET_PROPERTIES FOLDER "Application")

setup_lint ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

# find_package (FlagGG REQUIRED)
include_directories (${FLAGGG_DIR})

if (NOT FLAGGG_D3D11)
    list (APPEND EXCLUDED_SOURCE_DIRS Unit)
endif ()
# 输出排除的目录
message ("exclude folder: ${EXCLUDED_SOURCE_DIRS}")
string (REPLACE ";" "/[^;]+;" EXCLUDE_PATTERNS "${EXCLUDED_SOURCE_DIRS};")
define_source_files (EXCLUDE_PATTERNS ${EXCLUDE_PATTERNS} RECURSE GROUP)

setup_main_executable ()

target_link_libraries(${TARGET_NAME} FlagGG)

setup_test ()

