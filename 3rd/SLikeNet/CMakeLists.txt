# Define target name
set (TARGET_NAME SLikeNet)
set (LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)
list (APPEND TARGET_PROPERTIES FOLDER "3rd")

setup_lint ()

# define_source_files这个宏定义在UrhoCommon.cmake，参数什么意思去那个文件里看
define_source_files (RECURSE GROUP)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/include)

# Define dependency libs
set (INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

add_definitions (-DRAKNET_ENABLE_STATIC -D_RAKNET_SUPPORT_NatTypeDetectionServer=0 -D_RAKNET_SUPPORT_UDPForwarder=0 -D_RAKNET_SUPPORT_TwoWayAuthentication=0)
add_definitions (-D_RAKNET_SUPPORT_CloudClient=0 -D_RAKNET_SUPPORT_CloudServer=0 -D_RAKNET_SUPPORT_ConnectionGraph2=0 -D_RAKNET_SUPPORT_NatPunchthroughServer=0)
add_definitions (-D_RAKNET_SUPPORT_RelayPlugin=0 -D_RAKNET_SUPPORT_LibVoice=0 -D_RAKNET_SUPPORT_DynDNS=0 -D_RAKNET_SUPPORT_HTTPConnection2=0 -D_RAKNET_SUPPORT_HTTPConnection=0)
add_definitions (-D_RAKNET_SUPPORT_EmailSender=0 -D_RAKNET_SUPPORT_UDPProxyClient=0 -D_RAKNET_SUPPORT_UDPProxyCoordinator=0 -D_RAKNET_SUPPORT_TeamManager=0 -D_RAKNET_SUPPORT_TeamBalancer=0)
add_definitions (-D_RAKNET_SUPPORT_NatTypeDetectionClient=0 -D_RAKNET_SUPPORT_FullyConnectedMesh2=0 -D_RAKNET_SUPPORT_ConnectionGraph2=0 -D_RAKNET_SUPPORT_FullyConnectedMesh=0 -D_RAKNET_SUPPORT_TelnetTransport=0)
add_definitions (-D_RAKNET_SUPPORT_PacketLogger=0 -D_RAKNET_SUPPORT_DirectoryDeltaTransfer=0 -D_RAKNET_SUPPORT_FileOperations=0 -D_RAKNET_SUPPORT_UDPProxyServer=0)
add_definitions (-D_RAKNET_SUPPORT_ConsoleServer=0 -D_RAKNET_SUPPORT_RakNetTransport=0 -D_RAKNET_SUPPORT_TCPInterface=0 -D_RAKNET_SUPPORT_LogCommandParser=0 -D_RAKNET_SUPPORT_RakNetCommandParser=0)
add_definitions (-D_RAKNET_SUPPORT_Rackspace=0 -D_RAKNET_SUPPORT_PacketizedTCP=0 -D_RAKNET_SUPPORT_RPC4Plugin=0 -D_RAKNET_SUPPORT_Router2=0 -D_RAKNET_SUPPORT_ReplicaManager3=0)
add_definitions (-D_RAKNET_SUPPORT_ReadyEvent=0 -D_RAKNET_SUPPORT_MessageFilter=0 -D_RAKNET_SUPPORT_FileListTransfer=0)

# 也是个宏，见UrhoCommon.cmake
if (FLAGGG_LIB_TYPE STREQUAL SHARED)
    setup_library (SHARED)
    add_definitions (-D_RAKNET_DLL)
else()
    setup_library (STATIC)
    add_definitions (-D_RAKNET_LIB)
endif()

if (WIN32 AND NOT UNIX)
	target_link_libraries (${TARGET_NAME} ws2_32.lib)
else (WIN32 AND NOT UNIX)
	target_link_libraries (${TARGET_NAME} pthread)
endif (WIN32 AND NOT UNIX)

# Setup test cases
setup_test ()
