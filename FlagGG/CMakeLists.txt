include (${CMAKE_CURRENT_SOURCE_DIR}/../CMake/ThirdParty.cmake)

# Define target name
set (TARGET_NAME FlagGG)
set (LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin)

setup_lint ()

add_definitions (-DFlagGG_EXPORTS)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../3rd)
message ("include dir: ${CMAKE_CURRENT_SOURCE_DIR}")

# 在非windows上把Graphics排除，因为目前Graphics只写了windows的部分
if (NOT FLAGGG_D3D11)
    list (APPEND EXCLUDED_SOURCE_DIRS Graphics)
    list (APPEND EXCLUDED_SOURCE_DIRS Scene)
endif ()
list (APPEND EXCLUDED_SOURCE_DIRS spdlog)
# 输出排除的目录
message ("exclude folder: ${EXCLUDED_SOURCE_DIRS}")
string (REPLACE ";" "/[^;]+;" EXCLUDE_PATTERNS "${EXCLUDED_SOURCE_DIRS};")
# define_source_files这个宏定义在Common.cmake
define_source_files (EXCLUDE_PATTERNS ${EXCLUDE_PATTERNS} RECURSE GROUP)

# setup_library (STATIC) 表示静态库(win上*.lib，linux上*.a)
# setup_library (SHARED) 表示动态库(win上*.dll，linux上*.os)
# setup_main_executable() 表示可执行文件
# setup_main_executable ()
if (FLAGGG_LIB_TYPE STREQUAL SHARED)
    setup_library (SHARED)
else()
    setup_library (STATIC)
endif()

message ("boost lib: ${BOOST_LIB}")
message ("d3d11 lib: ${D3D11_LIB}")
message ("lua lib: ${LUA_LIB}")
message ("SLikeNet lib: ${SLikeNet_LIB}")

target_link_libraries(${TARGET_NAME}
    ${BOOST_LIB}
    ${D3D11_LIB}
    ${LUA_LIB}
	${SLikeNet_LIB})

# Setup test cases
setup_test ()

