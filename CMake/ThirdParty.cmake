# boost
if (WIN32 OR WIN64)
    include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../3rd/boost_1_67_0)
    link_directories (${CMAKE_CURRENT_SOURCE_DIR}/../3rd/boost_1_67_0/vc14_x86/lib)
    set (BOOST_LIB libboost_regex-vc140-mt-gd-x32-1_67)
else ()
    include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../3rd/boost_1_69_0)
    link_directories (/usr/local/lib)
    set (BOOST_LIB libboost_regex.a)
endif ()
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../3rd/lua)
set (LUA_LIB lua53)
set (SLikeNet_LIB SLikeNet)
set (BGFX_LIB bgfx)
set (BIMG_LIB bimg)
set (BX bx)
set (SHADERC shaderc)

# Direct3D
if (WIN32 OR WIN64)
    include_directories ("C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Include")
    link_directories ("C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Samples/C++/Effects11/Debug")
    link_directories ("C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Lib/x86")
    set (D3D11_LIB
    d3d9
    d3dx9
    d3dx9d
    d3d11
    d3dx11d
    dxerr
    d3dcompiler
    dxgi
    dxguid)
endif ()
