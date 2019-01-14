cmake_minimum_required(VERSION 3.13)

include (${CMAKE_CURRENT_SOURCE_DIR}/../CMake/Common.cmake)
include (${CMAKE_CURRENT_SOURCE_DIR}/../CMake/ThirdParty.cmake)

# Define target name
set (TARGET_NAME FlagGG)

setup_lint ()

add_definitions (-D_DEBUG)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

# define_source_files这个宏定义在Common.cmake
define_source_files (RECURSE GROUP)

# setup_library (STATIC) 表示静态库(win上*.lib，linux上*.a)
# setup_library (SHARED) 表示动态库(win上*.dll，linux上*.os)
# setup_main_executable() 表示可执行文件
setup_main_executable ()

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

