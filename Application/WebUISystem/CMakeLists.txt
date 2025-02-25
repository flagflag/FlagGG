set (TARGET_NAME WebUISystem)
set (LIBRARY_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)
set (FLAGGG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../FlagGG)
set (APP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../)
list (APPEND TARGET_PROPERTIES FOLDER "Application/Plugins")

setup_lint ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR})
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../)

include_directories (${FLAGGG_DIR})
include_directories (${APP_DIR})

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../UltralightSDK/include)

define_source_files (RECURSE GROUP)

link_directories (${CMAKE_CURRENT_SOURCE_DIR}/../UltralightSDK/lib)

if (FLAGGG_LIB_TYPE STREQUAL SHARED)
    setup_library (SHARED)
else()
    setup_library (STATIC)
endif()

target_link_libraries (${TARGET_NAME} UltralightCore Ultralight WebCore AppCore FlagGG)

if (WIN32)
	add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy 
        ${CMAKE_CURRENT_SOURCE_DIR}/../UltralightSDK/bin/Ultralight.dll ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/Debug)
	add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy 
        ${CMAKE_CURRENT_SOURCE_DIR}/../UltralightSDK/bin/WebCore.dll ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/Debug)
	add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy 
        ${CMAKE_CURRENT_SOURCE_DIR}/../UltralightSDK/bin/AppCore.dll ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/Debug)
endif()

setup_test ()

