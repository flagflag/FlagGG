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
set (BDFX_LIB bdfx)
set (PUGIXML_LIB PugiXml)

if (WIN32 OR WIN64)
    # Direct3D
    include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../3rd/D3D11/include)
    link_directories (${CMAKE_CURRENT_SOURCE_DIR}/../3rd/D3D11/lib/x64)
    set (D3D11_LIB
    d3dx9
    d3dx9d
    d3d11
    d3dx11.lib
    d3dx11d
    dxerr
    dxgi
    dxguid)
    # CoreCLR
    link_directories (${CMAKE_CURRENT_SOURCE_DIR}/../Application/CoreCLRHelper/Runtime)
    # Vulkan
    link_directories (${CMAKE_CURRENT_SOURCE_DIR}/../3rd/vulkan/lib/x64)
    set (VULKAN_LIB
        vulkan-1
    )
endif ()
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../3rd/Khronos)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../3rd/HLSLcc/include)
