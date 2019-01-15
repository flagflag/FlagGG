cmake_minimum_required(VERSION 3.13)

include (${CMAKE_CURRENT_SOURCE_DIR}/../CMake/Common.cmake)
include (${CMAKE_CURRENT_SOURCE_DIR}/../CMake/ThirdParty.cmake)

# Define target name
set (TARGET_NAME FlagGG)
set (LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin)

setup_lint ()

add_definitions (-D_DEBUG -DFlagGG_EXPORTS)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

# 在非windows上把Graphics排除，因为目前Graphics只写了windows的部分
if (NOT FLAGGG_D3D11)
    list (APPEND EXCLUDED_SOURCE_DIRS Graphics)
endif ()
# 输出排除的目录
message ("exclude folder: ${EXCLUDED_SOURCE_DIRS}")
string (REPLACE ";" "/[^;]+;" EXCLUDE_PATTERNS "${EXCLUDED_SOURCE_DIRS};")
# define_source_files这个宏定义在Common.cmake
define_source_files (EXCLUDE_PATTERNS ${EXCLUDE_PATTERNS} RECURSE GROUP)

# setup_library (STATIC) 表示静态库(win上*.lib，linux上*.a)
# setup_library (SHARED) 表示动态库(win上*.dll，linux上*.os)
# setup_main_executable() 表示可执行文件
# setup_main_executable ()
setup_library (SHARED)

target_link_libraries(${TARGET_NAME}
    libboost_regex-vc120-mt-gd-x32-1_67
    d3d9
    d3dx9
    d3dx9d
    d3d11
    d3dx11d
    dxerr
    d3dcompiler
    Effects11
    dxgi
    dxguid)

# Setup test cases
setup_test ()

