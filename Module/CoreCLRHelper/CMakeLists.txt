set (TARGET_NAME CoreCLRHelper)
set (LIBRARY_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)
set (FLAGGG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../FlagGG)
set (APP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../)
list (APPEND TARGET_PROPERTIES FOLDER "Module")

setup_lint ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR})
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../)

include_directories (${FLAGGG_DIR})
include_directories (${APP_DIR})

define_source_files (RECURSE GROUP)

if (FLAGGG_LIB_TYPE STREQUAL SHARED)
    setup_library (SHARED)
else()
    setup_library (STATIC)
endif()

# set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "/DELAYLOAD:nethost.dll")

target_link_libraries (${TARGET_NAME} FlagGG nethost)

add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy 
    ${CMAKE_CURRENT_SOURCE_DIR}/Runtime/NetCore.runtimeconfig.json ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/Debug)
add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy 
    ${CMAKE_CURRENT_SOURCE_DIR}/Runtime/NetCore.runtimeconfig.json ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/Release)

add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy 
    ${CMAKE_CURRENT_SOURCE_DIR}/Runtime/nethost.dll ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/Debug)
add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy 
    ${CMAKE_CURRENT_SOURCE_DIR}/Runtime/nethost.dll ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/Release)

setup_test ()

