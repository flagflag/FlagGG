set (TARGET_NAME UltralightDemo2)
set (EXECUTABLE_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)
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

setup_main_executable ()

target_link_libraries (${TARGET_NAME} UltralightCore AppCore Ultralight WebCore FlagGG)

if (WIN32)
	add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy 
        ${CMAKE_CURRENT_SOURCE_DIR}/../UltralightSDK/bin/AppCore.dll ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/Debug)
	add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy 
        ${CMAKE_CURRENT_SOURCE_DIR}/../UltralightSDK/bin/Ultralight.dll ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/Debug)
	add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy 
        ${CMAKE_CURRENT_SOURCE_DIR}/../UltralightSDK/bin/WebCore.dll ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/Debug)
endif()

# Set the assets path to "/assets" or "/../Resources/assets" on macOS
if (APPLE)
  set(ASSETS_PATH "$<TARGET_FILE_DIR:${TARGET_NAME}>/../Resources/assets") 
else ()
  set(ASSETS_PATH "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets") 
endif ()

# Copy assets to assets directory
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/../UltralightDemoAsset/assets/" "${ASSETS_PATH}")

# Copy resources to assets directory
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/../UltralightSDK/resources" "${ASSETS_PATH}/resources")

setup_test ()

