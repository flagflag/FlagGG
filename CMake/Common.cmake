# Macro for adjusting target output name by dropping _suffix from the target name
macro (adjust_target_name)
    if (TARGET_NAME MATCHES _.*$)
        string (REGEX REPLACE _.*$ "" OUTPUT_NAME ${TARGET_NAME})
        set_target_properties (${TARGET_NAME} PROPERTIES OUTPUT_NAME ${OUTPUT_NAME})
    endif ()
endmacro ()

# Macro for checking the SOURCE_FILES variable is properly initialized
macro (check_source_files)
    if (NOT SOURCE_FILES)
        if (NOT ${ARGN} STREQUAL "")
            message (FATAL_ERROR ${ARGN})
        else ()
            message (FATAL_ERROR "Could not configure and generate the project file because no source files have been defined yet. "
                "You can define the source files explicitly by setting the SOURCE_FILES variable in your CMakeLists.txt; or "
                "by calling the define_source_files() macro which would by default glob all the C++ source files found in the same scope of "
                "CMakeLists.txt where the macro is being called and the macro would set the SOURCE_FILES variable automatically. "
                "If your source files are not located in the same directory as the CMakeLists.txt or your source files are "
                "more than just C++ language then you probably have to pass in extra arguments when calling the macro in order to make it works. "
                "See the define_source_files() macro definition in the CMake/Modules/UrhoCommon.cmake for more detail.")
        endif ()
    endif ()
endmacro ()

# Macro for setting symbolic link on platform that supports it
macro (create_symlink SOURCE DESTINATION)
    cmake_parse_arguments (ARG "FALLBACK_TO_COPY" "BASE" "" ${ARGN})
    # Make absolute paths so they work more reliably on cmake-gui
    if (IS_ABSOLUTE ${SOURCE})
        set (ABS_SOURCE ${SOURCE})
    else ()
        set (ABS_SOURCE ${CMAKE_SOURCE_DIR}/${SOURCE})
    endif ()
    if (IS_ABSOLUTE ${DESTINATION})
        set (ABS_DESTINATION ${DESTINATION})
    else ()
        if (ARG_BASE)
            set (ABS_DESTINATION ${ARG_BASE}/${DESTINATION})
        else ()
            set (ABS_DESTINATION ${CMAKE_BINARY_DIR}/${DESTINATION})
        endif ()
    endif ()
    if (CMAKE_HOST_WIN32)
        if (IS_DIRECTORY ${ABS_SOURCE})
            set (SLASH_D /D)
        else ()
            unset (SLASH_D)
        endif ()
        if (HAS_MKLINK)
            if (NOT EXISTS ${ABS_DESTINATION})
                # Have to use string-REPLACE as file-TO_NATIVE_PATH does not work as expected with MinGW on "backward slash" host system
                string (REPLACE / \\ BACKWARD_ABS_DESTINATION ${ABS_DESTINATION})
                string (REPLACE / \\ BACKWARD_ABS_SOURCE ${ABS_SOURCE})
                execute_process (COMMAND cmd /C mklink ${SLASH_D} ${BACKWARD_ABS_DESTINATION} ${BACKWARD_ABS_SOURCE} OUTPUT_QUIET ERROR_QUIET)
            endif ()
        elseif (ARG_FALLBACK_TO_COPY)
            if (SLASH_D)
                set (COMMAND COMMAND ${CMAKE_COMMAND} -E copy_directory ${ABS_SOURCE} ${ABS_DESTINATION})
            else ()
                set (COMMAND COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ABS_SOURCE} ${ABS_DESTINATION})
            endif ()
            # Fallback to copy only one time
            execute_process (${COMMAND})
            if (TARGET ${TARGET_NAME})
                # Fallback to copy every time the target is built
                add_custom_command (TARGET ${TARGET_NAME} POST_BUILD ${COMMAND})
            endif ()
        else ()
            message (WARNING "Unable to create symbolic link on this host system, you may need to manually copy file/dir from \"${SOURCE}\" to \"${DESTINATION}\"")
        endif ()
    else ()
        execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${ABS_SOURCE} ${ABS_DESTINATION})
    endif ()
endmacro ()

# Macro for adding additional make clean files
macro (add_make_clean_files)
    get_directory_property (ADDITIONAL_MAKE_CLEAN_FILES ADDITIONAL_MAKE_CLEAN_FILES)
    set_directory_properties (PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${ADDITIONAL_MAKE_CLEAN_FILES};${ARGN}")
endmacro ()

# *** THIS IS A DEPRECATED MACRO ***
# Macro for defining external library dependencies
# The purpose of this macro is emulate CMake to set the external library dependencies transitively
# It works for both targets setup within FlagGG project and downstream projects that uses FlagGG as external static/shared library
# *** THIS IS A DEPRECATED MACRO ***
macro (define_dependency_libs TARGET)
    # ThirdParty/SLikeNet external dependency
    if (${TARGET} MATCHES SLikeNet|FlagGG)
        if (WIN32)
            list (APPEND LIBS iphlpapi)
        endif ()
    endif ()
endmacro ()

# Macro for defining source files with optional arguments as follows:
#  GLOB_CPP_PATTERNS <list> - Use the provided globbing patterns for CPP_FILES instead of the default *.cpp
#  GLOB_H_PATTERNS <list> - Use the provided globbing patterns for H_FILES instead of the default *.h
#  EXCLUDE_PATTERNS <list> - Use the provided regex patterns for excluding the unwanted matched source files
#  EXTRA_CPP_FILES <list> - Include the provided list of files into CPP_FILES result
#  EXTRA_H_FILES <list> - Include the provided list of files into H_FILES result
#  PCH <list> - Enable precompiled header support on the defined source files using the specified header file, the list is "<path/to/header> [C++|C]"
#  RECURSE - Option to glob recursively
#  GROUP - Option to group source files based on its relative path to the corresponding parent directory
macro (define_source_files)
    # Source files are defined by globbing source files in current source directory and also by including the extra source files if provided
    cmake_parse_arguments (ARG "RECURSE;GROUP" "" "PCH;EXTRA_CPP_FILES;EXTRA_H_FILES;GLOB_CPP_PATTERNS;GLOB_H_PATTERNS;EXCLUDE_PATTERNS" ${ARGN})
    if (NOT ARG_GLOB_CPP_PATTERNS)
        set (ARG_GLOB_CPP_PATTERNS *.cpp *.cc)    # Default glob pattern
    endif ()
    if (NOT ARG_GLOB_H_PATTERNS)
        set (ARG_GLOB_H_PATTERNS *.h *.hpp)
    endif ()
    if (ARG_RECURSE)
        set (ARG_RECURSE _RECURSE)
    else ()
        unset (ARG_RECURSE)
    endif ()
    file (GLOB${ARG_RECURSE} CPP_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${ARG_GLOB_CPP_PATTERNS})
    file (GLOB${ARG_RECURSE} H_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${ARG_GLOB_H_PATTERNS})
    if (ARG_EXCLUDE_PATTERNS)
        set (CPP_FILES_WITH_SENTINEL ";${CPP_FILES};")  # Stringify the lists
        set (H_FILES_WITH_SENTINEL ";${H_FILES};")
        foreach (PATTERN ${ARG_EXCLUDE_PATTERNS})
            foreach (LOOP RANGE 1)
                string (REGEX REPLACE ";${PATTERN};" ";;" CPP_FILES_WITH_SENTINEL "${CPP_FILES_WITH_SENTINEL}")
                string (REGEX REPLACE ";${PATTERN};" ";;" H_FILES_WITH_SENTINEL "${H_FILES_WITH_SENTINEL}")
            endforeach ()
        endforeach ()
        set (CPP_FILES ${CPP_FILES_WITH_SENTINEL})      # Convert strings back to lists, extra sentinels are harmless
        set (H_FILES ${H_FILES_WITH_SENTINEL})
    endif ()
    list (APPEND CPP_FILES ${ARG_EXTRA_CPP_FILES})
    list (APPEND H_FILES ${ARG_EXTRA_H_FILES})
    set (SOURCE_FILES ${CPP_FILES} ${H_FILES})
    # Optionally enable PCH
    if (ARG_PCH)
        enable_pch (${ARG_PCH})
    endif ()
    # Optionally group the sources based on their physical subdirectories
    if (ARG_GROUP)
        foreach (CPP_FILE ${CPP_FILES})
            get_filename_component (PATH ${CPP_FILE} PATH)
            if (PATH)
                string (REPLACE / \\ PATH ${PATH})
                source_group ("Source Files\\${PATH}" FILES ${CPP_FILE})
            endif ()
        endforeach ()
        foreach (H_FILE ${H_FILES})
            get_filename_component (PATH ${H_FILE} PATH)
            if (PATH)
                string (REPLACE / \\ PATH ${PATH})
                source_group ("Header Files\\${PATH}" FILES ${H_FILE})
            endif ()
        endforeach ()
    endif ()
endmacro ()

# Macro for defining resource directories with optional arguments as follows:
#  GLOB_PATTERNS <list> - Use the provided globbing patterns for resource directories, default to "${CMAKE_SOURCE_DIR}/bin/*Data"
#  EXCLUDE_PATTERNS <list> - Use the provided regex patterns for excluding the unwanted matched directories
#  EXTRA_DIRS <list> - Include the provided list of directories into globbing result
#  HTML_SHELL <value> - An absolute path to the HTML shell file (only applicable for Web platform)
macro (define_resource_dirs)
    check_source_files ("Could not call define_resource_dirs() macro before define_source_files() macro.")
    cmake_parse_arguments (ARG "" "HTML_SHELL" "GLOB_PATTERNS;EXCLUDE_PATTERNS;EXTRA_DIRS" ${ARGN})
    if (WEB AND ARG_HTML_SHELL)
        add_html_shell (${ARG_HTML_SHELL})
    endif ()
    # If not explicitly specified then use the FlagGG project structure convention
    if (NOT ARG_GLOB_PATTERNS)
        set (ARG_GLOB_PATTERNS ${CMAKE_SOURCE_DIR}/bin/*Data)
    endif ()
    file (GLOB GLOB_RESULTS ${ARG_GLOB_PATTERNS})
    unset (GLOB_DIRS)
    foreach (DIR ${GLOB_RESULTS})
        if (IS_DIRECTORY ${DIR})
            list (APPEND GLOB_DIRS ${DIR})
        endif ()
    endforeach ()
    if (ARG_EXCLUDE_PATTERNS)
        set (GLOB_DIRS_WITH_SENTINEL ";${GLOB_DIRS};")  # Stringify the lists
        foreach (PATTERN ${ARG_EXCLUDE_PATTERNS})
            foreach (LOOP RANGE 1)
                string (REGEX REPLACE ";${PATTERN};" ";;" GLOB_DIRS_WITH_SENTINEL "${GLOB_DIRS_WITH_SENTINEL}")
            endforeach ()
        endforeach ()
        set (GLOB_DIRS ${GLOB_DIRS_WITH_SENTINEL})      # Convert strings back to lists, extra sentinels are harmless
    endif ()
    list (APPEND RESOURCE_DIRS ${GLOB_DIRS})
    foreach (DIR ${ARG_EXTRA_DIRS})
        if (EXISTS ${DIR})
            list (APPEND RESOURCE_DIRS ${DIR})
        endif ()
    endforeach ()
    source_group ("Resource Dirs" FILES ${RESOURCE_DIRS})
    # Populate all the variables required by resource packaging, if the build option is enabled
    if (FLAGGG_PACKAGING AND RESOURCE_DIRS)
        foreach (DIR ${RESOURCE_DIRS})
            get_filename_component (NAME ${DIR} NAME)
            if (ANDROID)
                set (RESOURCE_${DIR}_PATHNAME ${CMAKE_BINARY_DIR}/assets/${NAME}.pak)
            else ()
                set (RESOURCE_${DIR}_PATHNAME ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${NAME}.pak)
            endif ()
            list (APPEND RESOURCE_PAKS ${RESOURCE_${DIR}_PATHNAME})
            if (EMSCRIPTEN AND NOT EMSCRIPTEN_SHARE_DATA)
                # Set the custom EMCC_OPTION property to preload the *.pak individually
                if (EMSCRIPTEN AND EMSCRIPTEN_PRELOAD)
                    set_source_files_properties (${RESOURCE_${DIR}_PATHNAME} PROPERTIES EMCC_OPTION preload-file EMCC_FILE_ALIAS "${NAME}.pak")
                endif ()
            endif ()
        endforeach ()
        set_property (SOURCE ${RESOURCE_PAKS} PROPERTY GENERATED TRUE)
        if (WEB)
            if (EMSCRIPTEN)
                # Set the custom EMCC_OPTION property to peload the generated shared data file
                if (EMSCRIPTEN_SHARE_DATA)
                    set (SHARED_RESOURCE_JS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.js)
                    list (APPEND SOURCE_FILES ${SHARED_RESOURCE_JS} ${SHARED_RESOURCE_JS}.data)
                    # DEST_BUNDLE_DIR may be empty when macro caller does not wish to install anything
                    if (DEST_BUNDLE_DIR)
                        install (FILES ${SHARED_RESOURCE_JS} ${SHARED_RESOURCE_JS}.data DESTINATION ${DEST_BUNDLE_DIR})
                    endif ()
                    # Define a custom command for generating a shared data file
                    if (RESOURCE_PAKS)
                        # When sharing a single data file, all main targets are assumed to use a same set of resource paks
                        foreach (FILE ${RESOURCE_PAKS})
                            get_filename_component (NAME ${FILE} NAME)
                            list (APPEND PAK_NAMES ${NAME})
                        endforeach ()
                        if (CMAKE_BUILD_TYPE STREQUAL Debug AND EMSCRIPTEN_EMCC_VERSION VERSION_GREATER 1.32.2)
                            set (SEPARATE_METADATA --separate-metadata)
                        endif ()
                        add_custom_command (OUTPUT ${SHARED_RESOURCE_JS} ${SHARED_RESOURCE_JS}.data
                            COMMAND ${EMPACKAGER} ${SHARED_RESOURCE_JS}.data --preload ${PAK_NAMES} --js-output=${SHARED_RESOURCE_JS} --use-preload-cache ${SEPARATE_METADATA}
                            DEPENDS RESOURCE_CHECK ${RESOURCE_PAKS}
                            WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
                            COMMENT "Generating shared data file")
                    endif ()
                endif ()
            endif ()
        endif ()
    endif ()
    if (XCODE)
        if (NOT RESOURCE_FILES)
            # Default app bundle icon
            set (RESOURCE_FILES ${CMAKE_SOURCE_DIR}/bin/Data/Textures/UrhoIcon.icns)
            if (ARM)
                # Default app icon on the iOS/tvOS home screen
                list (APPEND RESOURCE_FILES ${CMAKE_SOURCE_DIR}/bin/Data/Textures/UrhoIcon.png)
            endif ()
        endif ()
        # Group them together under 'Resources' in Xcode IDE
        source_group (Resources FILES ${RESOURCE_PAKS} ${RESOURCE_FILES})     # RESOURCE_PAKS could be empty if packaging is not requested
        # But only use either paks or dirs
        if (RESOURCE_PAKS)
            set_source_files_properties (${RESOURCE_PAKS} ${RESOURCE_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
        else ()
            set_source_files_properties (${RESOURCE_DIRS} ${RESOURCE_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
        endif ()
    endif ()
    list (APPEND SOURCE_FILES ${RESOURCE_DIRS} ${RESOURCE_PAKS} ${RESOURCE_FILES})
endmacro()

# Macro for adding a HTML shell-file when targeting Web platform
macro (add_html_shell)
    check_source_files ("Could not call add_html_shell() macro before define_source_files() macro.")
    if (EMSCRIPTEN)
        if (NOT ${ARGN} STREQUAL "")
            set (HTML_SHELL ${ARGN})
        else ()
            # Create FlagGG custom HTML shell that also embeds our own project logo
            if (NOT EXISTS ${CMAKE_BINARY_DIR}/Source/shell.html)
                file (READ ${EMSCRIPTEN_ROOT_PATH}/src/shell.html HTML_SHELL)
                string (REPLACE "<!doctype html>" "#!/usr/bin/env ${EMSCRIPTEN_EMRUN_BROWSER}\n<!-- This is a generated file. DO NOT EDIT!-->\n\n<!doctype html>" HTML_SHELL "${HTML_SHELL}")     # Stringify to preserve semicolons
                string (REPLACE "<body>" "<body>\n<script>document.body.innerHTML=document.body.innerHTML.replace(/^#!.*\\n/, '');</script>\n<a href=\"https://github.com/flagflag/FlagGG\" title=\"FlagGG Homepage\"><img src=\"https://github.com/flagflag/FlagGG/images/logo.png\" alt=\"link to https://github.com/flagflag/FlagGG\" height=\"80\" width=\"160\" /></a>\n" HTML_SHELL "${HTML_SHELL}")
                file (WRITE ${CMAKE_BINARY_DIR}/Source/shell.html "${HTML_SHELL}")
            endif ()
            set (HTML_SHELL ${CMAKE_BINARY_DIR}/Source/shell.html)
        endif ()
        list (APPEND SOURCE_FILES ${HTML_SHELL})
        set_source_files_properties (${HTML_SHELL} PROPERTIES EMCC_OPTION shell-file)
    endif ()
endmacro ()

include (GenerateExportHeader)

# Macro for precompiling header (On MSVC, the dummy C++ or C implementation file for precompiling the header file would be generated if not already exists)
# This macro should be called before the CMake target has been added
# Typically, user should indirectly call this macro by using the 'PCH' option when calling define_source_files() macro
macro (enable_pch HEADER_PATHNAME)
    # No op when PCH support is not enabled
    if (FLAGGG_PCH)
        # Get the optional LANG parameter to indicate whether the header should be treated as C or C++ header, default to C++
        if ("${ARGN}" STREQUAL C) # Stringify as the LANG parameter could be empty
            set (EXT c)
            set (LANG C)
            set (LANG_H c-header)
        else ()
            # This is the default
            set (EXT cpp)
            set (LANG CXX)
            set (LANG_H c++-header)
        endif ()
        # Relative path is resolved using CMAKE_CURRENT_SOURCE_DIR
        if (IS_ABSOLUTE ${HEADER_PATHNAME})
            set (ABS_HEADER_PATHNAME ${HEADER_PATHNAME})
        else ()
            set (ABS_HEADER_PATHNAME ${CMAKE_CURRENT_SOURCE_DIR}/${HEADER_PATHNAME})
        endif ()
        # Determine the precompiled header output filename
        get_filename_component (HEADER_FILENAME ${HEADER_PATHNAME} NAME)
        if (CMAKE_COMPILER_IS_GNUCXX)
            # GNU g++
            set (PCH_FILENAME ${HEADER_FILENAME}.gch)
        else ()
            # Clang or MSVC
            set (PCH_FILENAME ${HEADER_FILENAME}.pch)
        endif ()

        if (MSVC)
            get_filename_component (NAME_WE ${HEADER_FILENAME} NAME_WE)
            if (TARGET ${TARGET_NAME})
                if (VS)
                    # VS is multi-config, the exact path is only known during actual build time based on effective build config
                    set (PCH_PATHNAME "$(IntDir)${PCH_FILENAME}")
                else ()
                    set (PCH_PATHNAME ${CMAKE_CURRENT_BINARY_DIR}/${PCH_FILENAME})
                endif ()
                foreach (FILE ${SOURCE_FILES})
                    if (FILE MATCHES \\.${EXT}$)
                        if (FILE MATCHES ${NAME_WE}\\.${EXT}$)
                            # Precompiling header file
                            set_property (SOURCE ${FILE} APPEND_STRING PROPERTY COMPILE_FLAGS " /Fp${PCH_PATHNAME} /Yc${HEADER_FILENAME}")     # Need a leading space for appending
                        else ()
                            # Using precompiled header file
                            set_property (SOURCE ${FILE} APPEND_STRING PROPERTY COMPILE_FLAGS " /Fp${PCH_PATHNAME} /Yu${HEADER_FILENAME} /FI${HEADER_FILENAME}")
                        endif ()
                    endif ()
                endforeach ()
                unset (${TARGET_NAME}_HEADER_PATHNAME)
            else ()
                # The target has not been created yet, so set an internal variable to come back here again later
                set (${TARGET_NAME}_HEADER_PATHNAME ${ARGV})
                # But proceed to add the dummy C++ or C implementation file if necessary
                set (${LANG}_FILENAME ${NAME_WE}.${EXT})
                get_filename_component (PATH ${HEADER_PATHNAME} PATH)
                if (PATH)
                    set (PATH ${PATH}/)
                endif ()
                list (FIND SOURCE_FILES ${PATH}${${LANG}_FILENAME} ${LANG}_FILENAME_FOUND)
                if (${LANG}_FILENAME_FOUND STREQUAL -1)
                    if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${${LANG}_FILENAME})
                        # Only generate it once so that its timestamp is not touched unnecessarily
                        file (WRITE ${CMAKE_CURRENT_BINARY_DIR}/${${LANG}_FILENAME} "// This is a generated file. DO NOT EDIT!\n\n#include \"${HEADER_FILENAME}\"")
                    endif ()
                    list (INSERT SOURCE_FILES 0 ${${LANG}_FILENAME})
                endif ()
            endif ()
        elseif (XCODE)
            if (TARGET ${TARGET_NAME})
                # Precompiling and using precompiled header file
                set_target_properties (${TARGET_NAME} PROPERTIES XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER YES XCODE_ATTRIBUTE_GCC_PREFIX_HEADER ${ABS_HEADER_PATHNAME})
                unset (${TARGET_NAME}_HEADER_PATHNAME)
            else ()
                # The target has not been created yet, so set an internal variable to come back here again later
                set (${TARGET_NAME}_HEADER_PATHNAME ${ARGV})
            endif ()
        else ()
            # GCC or Clang
            if (TARGET ${TARGET_NAME})
                # Precompiling header file
                get_directory_property (COMPILE_DEFINITIONS COMPILE_DEFINITIONS)
                get_directory_property (INCLUDE_DIRECTORIES INCLUDE_DIRECTORIES)
                get_target_property (TYPE ${TARGET_NAME} TYPE)
                if (TYPE MATCHES SHARED|MODULE)
                    list (APPEND COMPILE_DEFINITIONS ${TARGET_NAME}_EXPORTS)
                    if (LANG STREQUAL CXX)
                        _test_compiler_hidden_visibility ()
                    endif ()
                endif ()
                # Use PIC flags as necessary, except when compiling using MinGW which already uses PIC flags for all codes
                if (NOT MINGW)
                    get_target_property (PIC ${TARGET_NAME} POSITION_INDEPENDENT_CODE)
                    if (PIC)
                        set (PIC_FLAGS -fPIC)
                    endif ()
                endif ()
                string (REPLACE ";" " -D" COMPILE_DEFINITIONS "-D${COMPILE_DEFINITIONS}")
                string (REPLACE "\"" "\\\"" COMPILE_DEFINITIONS ${COMPILE_DEFINITIONS})
                string (REPLACE ";" "\" -I\"" INCLUDE_DIRECTORIES "-I\"${INCLUDE_DIRECTORIES}\"")
                if (CMAKE_SYSROOT)
                    set (SYSROOT_FLAGS "--sysroot=\"${CMAKE_SYSROOT}\"")
                endif ()
                # Make sure the precompiled headers are not stale by creating custom rules to re-compile the header as necessary
                file (MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${PCH_FILENAME})
                set (ABS_PATH_PCH ${CMAKE_CURRENT_BINARY_DIR}/${HEADER_FILENAME})
                foreach (CONFIG ${CMAKE_CONFIGURATION_TYPES} ${CMAKE_BUILD_TYPE})   # These two vars are mutually exclusive
                    # Generate *.rsp containing configuration specific compiler flags
                    string (TOUPPER ${CONFIG} UPPERCASE_CONFIG)
                    file (WRITE ${ABS_PATH_PCH}.${CONFIG}.pch.rsp.new "${COMPILE_DEFINITIONS} ${SYSROOT_FLAGS} ${CLANG_${LANG}_FLAGS} ${CMAKE_${LANG}_FLAGS} ${CMAKE_${LANG}_FLAGS_${UPPERCASE_CONFIG}} ${COMPILER_HIDDEN_VISIBILITY_FLAGS} ${COMPILER_HIDDEN_INLINE_VISIBILITY_FLAGS} ${PIC_FLAGS} ${INCLUDE_DIRECTORIES} -c -x ${LANG_H}")
                    execute_process (COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ABS_PATH_PCH}.${CONFIG}.pch.rsp.new ${ABS_PATH_PCH}.${CONFIG}.pch.rsp)
                    file (REMOVE ${ABS_PATH_PCH}.${CONFIG}.pch.rsp.new)
                    if (NOT ${TARGET_NAME}_PCH_DEPS)
                        if (NOT CMAKE_CURRENT_SOURCE_DIR EQUAL CMAKE_CURRENT_BINARY_DIR)
                            # Create a dummy initial PCH file in the Out-of-source build tree to keep CLion happy
                            execute_process (COMMAND ${CMAKE_COMMAND} -E touch ${ABS_PATH_PCH})
                        endif ()
                        # Determine the dependency list
                        execute_process (COMMAND ${CMAKE_${LANG}_COMPILER} @${ABS_PATH_PCH}.${CONFIG}.pch.rsp -MTdeps -MM -MF ${ABS_PATH_PCH}.d ${ABS_HEADER_PATHNAME} RESULT_VARIABLE ${LANG}_COMPILER_EXIT_CODE)
                        if (NOT ${LANG}_COMPILER_EXIT_CODE EQUAL 0)
                            message (FATAL_ERROR "Could not generate dependency list for PCH. There is something wrong with your compiler toolchain. "
                                "Ensure its bin path is in the PATH environment variable or ensure CMake can find CC/CXX in your build environment.")
                        endif ()
                        file (STRINGS ${ABS_PATH_PCH}.d ${TARGET_NAME}_PCH_DEPS)
                        string (REGEX REPLACE "^deps: *| *\\; *" ";" ${TARGET_NAME}_PCH_DEPS ${${TARGET_NAME}_PCH_DEPS})
                        string (REGEX REPLACE "\\\\ " "\ " ${TARGET_NAME}_PCH_DEPS "${${TARGET_NAME}_PCH_DEPS}")    # Need to stringify the second time to preserve the semicolons
                    endif ()
                    # Create the rule that depends on the included headers
                    add_custom_command (OUTPUT ${HEADER_FILENAME}.${CONFIG}.pch.trigger
                        COMMAND ${CMAKE_${LANG}_COMPILER} @${ABS_PATH_PCH}.${CONFIG}.pch.rsp -o ${PCH_FILENAME}/${PCH_FILENAME}.${CONFIG} ${ABS_HEADER_PATHNAME}
                        COMMAND ${CMAKE_COMMAND} -E touch ${HEADER_FILENAME}.${CONFIG}.pch.trigger
                        DEPENDS ${ABS_PATH_PCH}.${CONFIG}.pch.rsp ${${TARGET_NAME}_PCH_DEPS}
                        COMMENT "Precompiling header file '${HEADER_FILENAME}' for ${CONFIG} configuration")
                    add_make_clean_files (${PCH_FILENAME}/${PCH_FILENAME}.${CONFIG})
                endforeach ()
                # Using precompiled header file
                set (CMAKE_${LANG}_FLAGS "${CMAKE_${LANG}_FLAGS} -include \"${ABS_PATH_PCH}\"")
                unset (${TARGET_NAME}_HEADER_PATHNAME)
            else ()
                # The target has not been created yet, so set an internal variable to come back here again later
                set (${TARGET_NAME}_HEADER_PATHNAME ${ARGV})
                # But proceed to add the dummy source file(s) to trigger the custom command output rule
                if (CMAKE_CONFIGURATION_TYPES)
                    # Multi-config, trigger all rules and let the compiler to choose which precompiled header is suitable to use
                    foreach (CONFIG ${CMAKE_CONFIGURATION_TYPES})
                        list (APPEND TRIGGERS ${HEADER_FILENAME}.${CONFIG}.pch.trigger)
                    endforeach ()
                else ()
                    # Single-config, just trigger the corresponding rule matching the current build configuration
                    set (TRIGGERS ${HEADER_FILENAME}.${CMAKE_BUILD_TYPE}.pch.trigger)
                endif ()
                list (APPEND SOURCE_FILES ${TRIGGERS})
            endif ()
        endif ()
    endif ()
endmacro ()

# Macro for finding file in FlagGG build tree or FlagGG SDK
macro (find_FlagGG_file VAR NAME)
    # Pass the arguments to the actual find command
    cmake_parse_arguments (ARG "" "DOC;MSG_MODE" "HINTS;PATHS;PATH_SUFFIXES" ${ARGN})
    find_file (${VAR} ${NAME} HINTS ${ARG_HINTS} PATHS ${ARG_PATHS} PATH_SUFFIXES ${ARG_PATH_SUFFIXES} DOC ${ARG_DOC} NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
    mark_as_advanced (${VAR})  # Hide it from cmake-gui in non-advanced mode
    if (NOT ${VAR} AND ARG_MSG_MODE)
        message (${ARG_MSG_MODE}
            "Could not find ${VAR} file in the FlagGG build tree or FlagGG SDK. "
            "Please reconfigure and rebuild your FlagGG build tree or reinstall the SDK for the correct target platform.")
    endif ()
endmacro ()

# Macro for finding tool in FlagGG build tree or FlagGG SDK
macro (find_FlagGG_tool VAR NAME)
    # Pass the arguments to the actual find command
    cmake_parse_arguments (ARG "" "DOC;MSG_MODE" "HINTS;PATHS;PATH_SUFFIXES" ${ARGN})
    find_program (${VAR} ${NAME} HINTS ${ARG_HINTS} PATHS ${ARG_PATHS} PATH_SUFFIXES ${ARG_PATH_SUFFIXES} DOC ${ARG_DOC} NO_DEFAULT_PATH)
    mark_as_advanced (${VAR})  # Hide it from cmake-gui in non-advanced mode
    if (NOT ${VAR})
        set (${VAR} ${CMAKE_BINARY_DIR}/bin/tool/${NAME})
        if (ARG_MSG_MODE AND NOT CMAKE_PROJECT_NAME STREQUAL FlagGG)
            message (${ARG_MSG_MODE}
                "Could not find ${VAR} tool in the FlagGG build tree or FlagGG SDK. Your project may not build successfully without this tool. "
                "You may have to first rebuild the FlagGG in its build tree or reinstall FlagGG SDK to get this tool built or installed properly. "
                "Alternatively, copy the ${VAR} executable manually into bin/tool subdirectory in your own project build tree.")
        endif ()
    endif ()
endmacro ()

# Macro for setting up header files installation for the SDK and the build tree (only support subset of install command arguments)
#  FILES <list> - File list to be installed
#  DIRECTORY <list> - Directory list to be installed
#  FILES_MATCHING - Option to perform file pattern matching on DIRECTORY list
#  USE_FILE_SYMLINK - Option to use file symlinks on the matched files found in the DIRECTORY list
#  BUILD_TREE_ONLY - Option to install the header files into the build tree only
#  PATTERN <list> - Pattern list to be used in file pattern matching option
#  BASE <value> - An absolute base path to be prepended to the destination path when installing to build tree, default to build tree
#  DESTINATION <value> - A relative destination path to be installed to
#  ACCUMULATE <value> - Accumulate the header files into the specified CMake variable, implies USE_FILE_SYMLINK when input list is a directory
macro (install_header_files)
    # Need to check if the destination variable is defined first because this macro could be called by downstream project that does not wish to install anything
    if (DEST_INCLUDE_DIR)
        # Parse the arguments for the underlying install command for the SDK
        cmake_parse_arguments (ARG "FILES_MATCHING;USE_FILE_SYMLINK;BUILD_TREE_ONLY" "BASE;DESTINATION;ACCUMULATE" "FILES;DIRECTORY;PATTERN" ${ARGN})
        unset (INSTALL_MATCHING)
        if (ARG_FILES)
            set (INSTALL_TYPE FILES)
            set (INSTALL_SOURCES ${ARG_FILES})
        elseif (ARG_DIRECTORY)
            set (INSTALL_TYPE DIRECTORY)
            set (INSTALL_SOURCES ${ARG_DIRECTORY})
            if (ARG_FILES_MATCHING)
                set (INSTALL_MATCHING FILES_MATCHING)
                # Our macro supports PATTERN <list> but CMake's install command does not, so convert the list to: PATTERN <value1> PATTERN <value2> ...
                foreach (PATTERN ${ARG_PATTERN})
                    list (APPEND INSTALL_MATCHING PATTERN ${PATTERN})
                endforeach ()
            endif ()
        else ()
            message (FATAL_ERROR "Couldn't setup install command because the install type is not specified.")
        endif ()
        if (NOT ARG_DESTINATION)
            message (FATAL_ERROR "Couldn't setup install command because the install destination is not specified.")
        endif ()
        if (NOT ARG_BUILD_TREE_ONLY AND NOT CMAKE_PROJECT_NAME MATCHES ^FlagGG-ExternalProject-)
            install (${INSTALL_TYPE} ${INSTALL_SOURCES} DESTINATION ${ARG_DESTINATION} ${INSTALL_MATCHING})
        endif ()

        # Reparse the arguments for the create_symlink macro to "install" the header files in the build tree
        if (NOT ARG_BASE)
            # Use build tree as base path
            if (ANDROID AND GRADLE_BUILD_DIR)
                set (ARG_BASE ${GRADLE_BUILD_DIR}/tree/${CMAKE_BUILD_TYPE}/${ANDROID_ABI})
            else ()
                set (ARG_BASE ${CMAKE_BINARY_DIR})
            endif ()
        endif ()
        foreach (INSTALL_SOURCE ${INSTALL_SOURCES})
            if (NOT IS_ABSOLUTE ${INSTALL_SOURCE})
                set (INSTALL_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/${INSTALL_SOURCE})
            endif ()
            if (INSTALL_SOURCE MATCHES /$)
                # Source is a directory
                if (ARG_USE_FILE_SYMLINK OR ARG_ACCUMULATE OR BASH_ON_WINDOWS)
                    # Use file symlink for each individual files in the source directory
                    if (IS_SYMLINK ${ARG_DESTINATION} AND NOT CMAKE_HOST_WIN32)
                        execute_process (COMMAND ${CMAKE_COMMAND} -E remove ${ARG_DESTINATION})
                    endif ()
                    set (GLOBBING_EXPRESSION RELATIVE ${INSTALL_SOURCE})
                    if (ARG_FILES_MATCHING)
                        foreach (PATTERN ${ARG_PATTERN})
                            list (APPEND GLOBBING_EXPRESSION ${INSTALL_SOURCE}${PATTERN})
                        endforeach ()
                    else ()
                        list (APPEND GLOBBING_EXPRESSION ${INSTALL_SOURCE}*)
                    endif ()
                    file (GLOB_RECURSE NAMES ${GLOBBING_EXPRESSION})
                    foreach (NAME ${NAMES})
                        get_filename_component (PATH ${ARG_DESTINATION}/${NAME} PATH)
                        # Recreate the source directory structure in the destination path
                        if (NOT EXISTS ${ARG_BASE}/${PATH})
                            file (MAKE_DIRECTORY ${ARG_BASE}/${PATH})
                        endif ()
                        create_symlink (${INSTALL_SOURCE}${NAME} ${ARG_DESTINATION}/${NAME} BASE ${ARG_BASE} FALLBACK_TO_COPY)
                        if (ARG_ACCUMULATE)
                            list (APPEND ${ARG_ACCUMULATE} ${ARG_DESTINATION}/${NAME})
                        endif ()
                    endforeach ()
                else ()
                    # Use a single symlink pointing to the source directory
                    if (NOT IS_SYMLINK ${ARG_DESTINATION} AND NOT CMAKE_HOST_WIN32)
                        execute_process (COMMAND ${CMAKE_COMMAND} -E remove_directory ${ARG_DESTINATION})
                    endif ()
                    create_symlink (${INSTALL_SOURCE} ${ARG_DESTINATION} BASE ${ARG_BASE} FALLBACK_TO_COPY)
                endif ()
            else ()
                # Source is a file (it could also be actually a directory to be treated as a "file", i.e. for creating symlink pointing to the directory)
                get_filename_component (NAME ${INSTALL_SOURCE} NAME)
                create_symlink (${INSTALL_SOURCE} ${ARG_DESTINATION}/${NAME} BASE ${ARG_BASE} FALLBACK_TO_COPY)
                if (ARG_ACCUMULATE)
                    list (APPEND ${ARG_ACCUMULATE} ${ARG_DESTINATION}/${NAME})
                endif ()
            endif ()
        endforeach ()
    endif ()
endmacro ()

# Macro for setting common output directories
macro (set_output_directories OUTPUT_PATH)
    cmake_parse_arguments (ARG LOCAL "" "" ${ARGN})
    if (ARG_LOCAL)
        unset (SCOPE)
        unset (OUTPUT_DIRECTORY_PROPERTIES)
    else ()
        set (SCOPE CMAKE_)
    endif ()
    foreach (TYPE ${ARG_UNPARSED_ARGUMENTS})
        set (${SCOPE}${TYPE}_OUTPUT_DIRECTORY ${OUTPUT_PATH})
        list (APPEND OUTPUT_DIRECTORY_PROPERTIES ${TYPE}_OUTPUT_DIRECTORY ${${TYPE}_OUTPUT_DIRECTORY})
        foreach (CONFIG ${CMAKE_CONFIGURATION_TYPES})
            string (TOUPPER ${CONFIG} CONFIG)
            set (${SCOPE}${TYPE}_OUTPUT_DIRECTORY_${CONFIG} ${OUTPUT_PATH})
            list (APPEND OUTPUT_DIRECTORY_PROPERTIES ${TYPE}_OUTPUT_DIRECTORY_${CONFIG} ${${TYPE}_OUTPUT_DIRECTORY_${CONFIG}})
        endforeach ()
        if (TYPE STREQUAL RUNTIME AND NOT ${OUTPUT_PATH} STREQUAL .)
            file (RELATIVE_PATH REL_OUTPUT_PATH ${CMAKE_BINARY_DIR} ${OUTPUT_PATH})
            set (DEST_RUNTIME_DIR ${REL_OUTPUT_PATH})
        endif ()
    endforeach ()
    if (ARG_LOCAL)
        list (APPEND TARGET_PROPERTIES ${OUTPUT_DIRECTORY_PROPERTIES})
    endif ()
endmacro ()

# Macro for setting up an executable target
# Macro arguments:
#  PRIVATE - setup executable target without installing it
#  TOOL - setup a tool executable target
#  NODEPS - setup executable target without defining FlagGG dependency libraries
#  WIN32/MACOSX_BUNDLE/EXCLUDE_FROM_ALL - see CMake help on add_executable() command
# CMake variables:
#  SOURCE_FILES - list of source files
#  INCLUDE_DIRS - list of directories for include search path
#  LIBS - list of dependent libraries that are built internally in the project
#  ABSOLUTE_PATH_LIBS - list of dependent libraries that are external to the project
#  LINK_DEPENDS - list of additional files on which a target binary depends for linking (Makefile-based generator only)
#  LINK_FLAGS - list of additional link flags
#  TARGET_PROPERTIES - list of target properties
macro (setup_executable)
    cmake_parse_arguments (ARG "PRIVATE;TOOL;TOOL2;NODEPS" "" "" ${ARGN})
    check_source_files ()
    add_executable (${TARGET_NAME} ${ARG_UNPARSED_ARGUMENTS} ${SOURCE_FILES})
    set (RUNTIME_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    if (ARG_PRIVATE)
        set_output_directories (. LOCAL RUNTIME PDB)
        set (RUNTIME_DIR .)
    endif ()
    if (ARG_TOOL)
        list (APPEND TARGET_PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH YES)
        if (NOT ARG_PRIVATE AND NOT DEST_RUNTIME_DIR MATCHES tool)
            set_output_directories (${CMAKE_BINARY_DIR}/bin/tool LOCAL RUNTIME PDB)
            set (RUNTIME_DIR ${CMAKE_BINARY_DIR}/bin/tool)
        endif ()
    endif ()
    if (ARG_TOOL2)
        message('Tool2')       
        set_output_directories (${CMAKE_BINARY_DIR}/bin LOCAL RUNTIME PDB)
        set (RUNTIME_DIR ${CMAKE_BINARY_DIR}/bin)
    endif()
    if (NOT ARG_NODEPS)
        define_dependency_libs (FlagGG)
    endif ()
    if (XCODE AND LUAJIT_EXE_LINKER_FLAGS_APPLE)
        # Xcode universal build linker flags when targeting 64-bit OSX with LuaJIT enabled
        list (APPEND TARGET_PROPERTIES XCODE_ATTRIBUTE_OTHER_LDFLAGS[arch=x86_64] "${LUAJIT_EXE_LINKER_FLAGS_APPLE} $(OTHER_LDFLAGS)")
    endif ()
    _setup_target ()

    if (FLAGGG_SCP_TO_TARGET)
        add_custom_command (TARGET ${TARGET_NAME} POST_BUILD COMMAND scp $<TARGET_FILE:${TARGET_NAME}> ${FLAGGG_SCP_TO_TARGET} || exit 0
            COMMENT "Scp-ing ${TARGET_NAME} executable to target system")
    endif ()
    if (WIN32 AND NOT ARG_NODEPS AND FLAGGG_LIB_TYPE STREQUAL SHARED)
        # Make a copy of the FlagGG DLL to the runtime directory in the build tree
        if (TARGET FlagGG)
            # add_custom_command (TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:FlagGG> ${RUNTIME_DIR})
            add_make_clean_files (${RUNTIME_DIR}/$<TARGET_FILE_NAME:FlagGG>)
        else ()
            foreach (DLL ${FLAGGG_DLL})
                add_custom_command (TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DLL} ${RUNTIME_DIR})
                add_make_clean_files (${RUNTIME_DIR}/${DLL})
            endforeach ()
        endif ()
    endif ()
    if (DIRECT3D_DLL AND NOT ARG_NODEPS)
        # Make a copy of the D3D DLL to the runtime directory in the build tree
        add_custom_command (TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DIRECT3D_DLL} ${RUNTIME_DIR})
        add_make_clean_files (${RUNTIME_DIR}/${DIRECT3D_DLL})
    endif ()
    # Need to check if the destination variable is defined first because this macro could be called by downstream project that does not wish to install anything
    if (NOT ARG_PRIVATE)
        if (WEB AND DEST_BUNDLE_DIR)
            set (EXTS data html.map js wasm)
            if (SELF_EXECUTABLE_SHELL)
                # Install it as program so it gets the correct file permission
                install (PROGRAMS $<TARGET_FILE:${TARGET_NAME}> DESTINATION ${DEST_BUNDLE_DIR})
            else ()
                list (APPEND EXTS html)
            endif ()
            set (LOCATION $<TARGET_FILE_DIR:${TARGET_NAME}>)
            unset (FILES)
            foreach (EXT ${EXTS})
                list (APPEND FILES ${LOCATION}/${TARGET_NAME}.${EXT})
            endforeach ()
            install (FILES ${FILES} DESTINATION ${DEST_BUNDLE_DIR} OPTIONAL)
        elseif (DEST_RUNTIME_DIR AND (DEST_BUNDLE_DIR OR NOT (IOS OR TVOS)))
            install (TARGETS ${TARGET_NAME} RUNTIME DESTINATION ${DEST_RUNTIME_DIR} BUNDLE DESTINATION ${DEST_BUNDLE_DIR})
            if (WIN32 AND NOT ARG_NODEPS AND FLAGGG_LIB_TYPE STREQUAL SHARED AND NOT FLAGGG_DLL_INSTALLED)
                if (TARGET FlagGG)
                    install (FILES $<TARGET_FILE:FlagGG> DESTINATION ${DEST_RUNTIME_DIR})
                else ()
                    install (FILES ${FLAGGG_DLL} DESTINATION ${DEST_RUNTIME_DIR})
                endif ()
                set (FLAGGG_DLL_INSTALLED TRUE)
            endif ()
            if (DIRECT3D_DLL AND NOT DIRECT3D_DLL_INSTALLED)
                # Make a copy of the D3D DLL to the runtime directory in the installed location
                install (FILES ${DIRECT3D_DLL} DESTINATION ${DEST_RUNTIME_DIR})
                set (DIRECT3D_DLL_INSTALLED TRUE)
            endif ()
        endif ()
    endif ()
endmacro ()

# Macro for setting up a library target
# Macro arguments:
#  NODEPS - setup library target without defining FlagGG dependency libraries (applicable for downstream projects)
#  STATIC/SHARED/MODULE/EXCLUDE_FROM_ALL - see CMake help on add_library() command
# CMake variables:
#  SOURCE_FILES - list of source files
#  INCLUDE_DIRS - list of directories for include search path
#  LIBS - list of dependent libraries that are built internally in the project
#  ABSOLUTE_PATH_LIBS - list of dependent libraries that are external to the project
#  LINK_DEPENDS - list of additional files on which a target binary depends for linking (Makefile-based generator only)
#  LINK_FLAGS - list of additional link flags
#  TARGET_PROPERTIES - list of target properties
macro (setup_library)
    cmake_parse_arguments (ARG NODEPS "" "" ${ARGN})
    check_source_files ()
    add_library (${TARGET_NAME} ${ARG_UNPARSED_ARGUMENTS} ${SOURCE_FILES})
    get_target_property (LIB_TYPE ${TARGET_NAME} TYPE)
    if (NOT ARG_NODEPS AND NOT PROJECT_NAME STREQUAL FlagGG)
        define_dependency_libs (FlagGG)
    endif ()
    if (XCODE AND LUAJIT_SHARED_LINKER_FLAGS_APPLE AND LIB_TYPE STREQUAL SHARED_LIBRARY)
        list (APPEND TARGET_PROPERTIES XCODE_ATTRIBUTE_OTHER_LDFLAGS[arch=x86_64] "${LUAJIT_SHARED_LINKER_FLAGS_APPLE} $(OTHER_LDFLAGS)")    # Xcode universal build linker flags when targeting 64-bit OSX with LuaJIT enabled
    endif ()
    _setup_target ()

    if (PROJECT_NAME STREQUAL FlagGG)
        # Accumulate all the dependent static libraries that are used in building the FlagGG library itself
        if (NOT ${TARGET_NAME} STREQUAL FlagGG AND LIB_TYPE STREQUAL STATIC_LIBRARY)
            set (STATIC_LIBRARY_TARGETS ${STATIC_LIBRARY_TARGETS} ${TARGET_NAME} PARENT_SCOPE)
            # When performing Xcode CI build suppress all the warnings for 3rd party libraries because there are just too many of them
            if (XCODE AND DEFINED ENV{CI})
                set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
                set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
            endif ()
        endif ()
    elseif (FLAGGG_SCP_TO_TARGET)
        add_custom_command (TARGET ${TARGET_NAME} POST_BUILD COMMAND scp $<TARGET_FILE:${TARGET_NAME}> ${FLAGGG_SCP_TO_TARGET} || exit 0
            COMMENT "Scp-ing ${TARGET_NAME} library to target system")
    endif ()
endmacro ()

# Macro for setting up an executable target with resources to copy/package/bundle/preload
# Macro arguments:
#  NODEPS - setup executable target without defining FlagGG dependency libraries
#  NOBUNDLE - do not use MACOSX_BUNDLE even when FLAGGG_MACOSX_BUNDLE build option is enabled
#  WIN32/MACOSX_BUNDLE/EXCLUDE_FROM_ALL - see CMake help on add_executable() command
# CMake variables:
#  RESOURCE_DIRS - list of resource directories (will be packaged into *.pak when FLAGGG_PACKAGING build option is set)
#  RESOURCE_FILES - list of additional resource files (will not be packaged into *.pak in any case)
#  SOURCE_FILES - list of source files
#  INCLUDE_DIRS - list of directories for include search path
#  LIBS - list of dependent libraries that are built internally in the project
#  ABSOLUTE_PATH_LIBS - list of dependent libraries that are external to the project
#  LINK_DEPENDS - list of additional files on which a target binary depends for linking (Makefile-based generator only)
#  LINK_FLAGS - list of additional link flags
#  TARGET_PROPERTIES - list of target properties
macro (setup_main_executable)
    cmake_parse_arguments (ARG "NOBUNDLE;MACOSX_BUNDLE;WIN32" "" "" ${ARGN})
    if (NOT RESOURCE_DIRS)
        define_resource_dirs ()
    endif ()
    if (ANDROID)
        # Setup target as main shared library
        setup_library (SHARED)
        if (DEST_LIBRARY_DIR)
            install (TARGETS ${TARGET_NAME} LIBRARY DESTINATION ${DEST_LIBRARY_DIR} ARCHIVE DESTINATION ${DEST_LIBRARY_DIR})
        endif ()
    else ()
        # Setup target as executable
        if (WIN32)
            if (NOT FLAGGG_WIN32_CONSOLE OR ARG_WIN32)
                set (EXE_TYPE WIN32)
            endif ()
            list (APPEND TARGET_PROPERTIES DEBUG_POSTFIX _d)
        elseif (IOS)
            set (EXE_TYPE MACOSX_BUNDLE)
            list (APPEND TARGET_PROPERTIES XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY 1,2 MACOSX_BUNDLE_INFO_PLIST iOSBundleInfo.plist.template)
        elseif (TVOS)
            set (EXE_TYPE MACOSX_BUNDLE)
            list (APPEND TARGET_PROPERTIES XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY 3 MACOSX_BUNDLE_INFO_PLIST tvOSBundleInfo.plist.template)
        elseif (APPLE)
            if ((FLAGGG_MACOSX_BUNDLE OR ARG_MACOSX_BUNDLE) AND NOT ARG_NOBUNDLE)
                set (EXE_TYPE MACOSX_BUNDLE)
                list (APPEND TARGET_PROPERTIES MACOSX_BUNDLE_INFO_PLIST MacOSXBundleInfo.plist.template)
            endif ()
        elseif (WEB)
            if (EMSCRIPTEN)
                # Output to HTML when a HTML shell-file is being added in source files list
                foreach (FILE ${SOURCE_FILES})
                    get_property (EMCC_OPTION SOURCE ${FILE} PROPERTY EMCC_OPTION)
                    if (EMCC_OPTION STREQUAL shell-file)
                        list (APPEND TARGET_PROPERTIES SUFFIX .html)
                        # Check if the shell-file is self-executable
                        file (READ ${FILE} SHEBANG LIMIT 3)     # Workaround CMake's funny way of file I/O operation
                        string (COMPARE EQUAL ${SHEBANG} "#!\n" SELF_EXECUTABLE_SHELL)
                        set (HAS_SHELL_FILE 1)
                        break ()
                    endif ()
                endforeach ()
                # Auto adding the HTML shell-file if necessary
                if (NOT EMCC_OPTION STREQUAL shell-file)
                    if (FLAGGG_TESTING OR EMSCRIPTEN_AUTO_SHELL)
                        add_html_shell ()
                        list (APPEND TARGET_PROPERTIES SUFFIX .html)
                        set (SELF_EXECUTABLE_SHELL 1)
                        set (HAS_SHELL_FILE 1)
                    endif ()
                endif ()
            endif ()
        endif ()
        setup_executable (${EXE_TYPE} ${ARG_UNPARSED_ARGUMENTS})
        if (HAS_SHELL_FILE)
            get_target_property (LOCATION ${TARGET_NAME} LOCATION)
            get_filename_component (NAME_WE ${LOCATION} NAME_WE)
            add_make_clean_files ($<TARGET_FILE_DIR:${TARGET_NAME}>/${NAME_WE}.js $<TARGET_FILE_DIR:${TARGET_NAME}>/${NAME_WE}.wasm)
        endif ()
    endif ()
    # Setup custom resource checker target
    if ((EXE_TYPE STREQUAL MACOSX_BUNDLE OR FLAGGG_PACKAGING) AND RESOURCE_DIRS)
        if (FLAGGG_PACKAGING)
            # FlagGG project builds the PackageTool as required; downstream project uses PackageTool found in the FlagGG build tree or FlagGG SDK
            find_FlagGG_tool (PACKAGE_TOOL PackageTool
                HINTS ${CMAKE_BINARY_DIR}/bin/tool ${FLAGGG_HOME}/bin/tool
                DOC "Path to PackageTool" MSG_MODE WARNING)
            if (CMAKE_PROJECT_NAME STREQUAL FlagGG)
                set (PACKAGING_DEP DEPENDS PackageTool)
            endif ()
            set (PACKAGING_COMMENT " and packaging")
        endif ()
        # Share a same custom target that checks for a same resource dirs list
        foreach (DIR ${RESOURCE_DIRS})
            string (MD5 MD5 ${DIR})
            set (MD5ALL ${MD5ALL}${MD5})
            if (CMAKE_HOST_WIN32)
                # On Windows host, always assumes there are changes so resource dirs would be repackaged in each build, however, still make sure the *.pak timestamp is not altered unnecessarily
                if (FLAGGG_PACKAGING)
                    set (PACKAGING_COMMAND && echo Packaging ${DIR}... && ${PACKAGE_TOOL} ${DIR} ${RESOURCE_${DIR}_PATHNAME}.new -c -q && ${CMAKE_COMMAND} -E copy_if_different ${RESOURCE_${DIR}_PATHNAME}.new ${RESOURCE_${DIR}_PATHNAME} && ${CMAKE_COMMAND} -E remove ${RESOURCE_${DIR}_PATHNAME}.new)
                endif ()
                list (APPEND COMMANDS COMMAND ${CMAKE_COMMAND} -E touch ${DIR} ${PACKAGING_COMMAND})
            else ()
                # On Unix-like hosts, detect the changes in the resource directory recursively so they are only repackaged and/or rebundled (Xcode only) as necessary
                if (FLAGGG_PACKAGING)
                    set (PACKAGING_COMMAND && echo Packaging ${DIR}... && ${PACKAGE_TOOL} ${DIR} ${RESOURCE_${DIR}_PATHNAME} -c -q)
                    set (OUTPUT_COMMAND test -e ${RESOURCE_${DIR}_PATHNAME} || \( true ${PACKAGING_COMMAND} \))
                else ()
                    set (OUTPUT_COMMAND true)   # Nothing to output
                endif ()
                list (APPEND COMMANDS COMMAND echo Checking ${DIR}... && bash -c \"\(\( `find ${DIR} -newer ${DIR} |wc -l` \)\)\" && touch -cm ${DIR} ${PACKAGING_COMMAND} || ${OUTPUT_COMMAND})
            endif ()
            add_make_clean_files (${RESOURCE_${DIR}_PATHNAME})
        endforeach ()
        string (MD5 MD5ALL ${MD5ALL})
        # Ensure the resource check is done before building the main executable target
        if (NOT RESOURCE_CHECK_${MD5ALL})
            set (RESOURCE_CHECK RESOURCE_CHECK)
            while (TARGET ${RESOURCE_CHECK})
                string (RANDOM RANDOM)
                set (RESOURCE_CHECK RESOURCE_CHECK_${RANDOM})
            endwhile ()
            set (RESOURCE_CHECK_${MD5ALL} ${RESOURCE_CHECK} CACHE INTERNAL "Resource check hash map")
        endif ()
        if (NOT TARGET ${RESOURCE_CHECK_${MD5ALL}})
            add_custom_target (${RESOURCE_CHECK_${MD5ALL}} ALL ${COMMANDS} ${PACKAGING_DEP} COMMENT "Checking${PACKAGING_COMMENT} resource directories")
        endif ()
        add_dependencies (${TARGET_NAME} ${RESOURCE_CHECK_${MD5ALL}})
    endif ()
    # Only need to install the resource directories once in case they are referenced by multiple targets
    if (RESOURCE_DIRS AND DEST_SHARE_DIR)
        foreach (DIR ${RESOURCE_DIRS})
            list (FIND INSTALLED_RESOURCE_DIRS ${DIR} FOUND_INDEX)
            if (FOUND_INDEX EQUAL -1)
                install (DIRECTORY ${DIR} DESTINATION ${DEST_SHARE_DIR}/Resources)
                list (APPEND INSTALLED_RESOURCE_DIRS ${DIR})
            endif ()
            # This cache variable is used to keep track of whether a resource directory has been instructed to be installed by CMake or not
            set (INSTALLED_RESOURCE_DIRS ${INSTALLED_RESOURCE_DIRS} CACHE INTERNAL "Installed resource dirs")
        endforeach ()
    endif ()
    # Define a custom command for stripping the main target executable for Release build configuration,
    # but only for platforms that support it and require it (for Android, let Android plugin handle it)
    if (CMAKE_BUILD_TYPE STREQUAL Release AND NOT ANDROID AND NOT WEB AND NOT MSVC)
        add_custom_command (TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${TARGET_NAME}>)
    endif ()
endmacro ()

# This cache variable is used to keep track of whether a resource directory has been instructed to be installed by CMake or not
unset (INSTALLED_RESOURCE_DIRS CACHE)

# Macro for setting up dependency lib for compilation and linking of a target (to be used internally)
macro (_setup_target)
    # Include directories
    include_directories (${INCLUDE_DIRS})
    # Link libraries
    define_dependency_libs (${TARGET_NAME})
    target_link_libraries (${TARGET_NAME} ${ABSOLUTE_PATH_LIBS} ${LIBS})
    # Enable PCH if requested
    if (${TARGET_NAME}_HEADER_PATHNAME)
        enable_pch (${${TARGET_NAME}_HEADER_PATHNAME})
    endif ()
    # Extra compiler flags for Xcode which are dynamically changed based on active arch in order to support Mach-O universal binary targets
    # We don't add the ABI flag for Xcode because it automatically passes '-arch i386' compiler flag when targeting 32 bit which does the same thing as '-m32'
    if (XCODE)
        # Speed up build when in Debug configuration by building active arch only
        list (FIND TARGET_PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH ATTRIBUTE_ALREADY_SET)
        if (ATTRIBUTE_ALREADY_SET EQUAL -1)
            list (APPEND TARGET_PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH $<$<CONFIG:Debug>:YES>)
        endif ()
        if (NOT FLAGGG_SSE)
            # Nullify the Clang default so that it is consistent with GCC
            list (APPEND TARGET_PROPERTIES XCODE_ATTRIBUTE_OTHER_CFLAGS[arch=i386] "-mno-sse $(OTHER_CFLAGS)")
            list (APPEND TARGET_PROPERTIES XCODE_ATTRIBUTE_OTHER_CPLUSPLUSFLAGS[arch=i386] "-mno-sse $(OTHER_CPLUSPLUSFLAGS)")
        endif ()
    endif ()
    # Extra linker flags for Emscripten
    if (EMSCRIPTEN)
        # These flags are set only once either in the main module or main executable
        if ((FLAGGG_LIB_TYPE STREQUAL MODULE AND ${TARGET_NAME} STREQUAL FlagGG) OR (NOT FLAGGG_LIB_TYPE STREQUAL MODULE AND NOT LIB_TYPE))
            list (APPEND LINK_FLAGS "-s TOTAL_MEMORY=${EMSCRIPTEN_TOTAL_MEMORY}")
            if (EMSCRIPTEN_ALLOW_MEMORY_GROWTH)
                list (APPEND LINK_FLAGS "-s ALLOW_MEMORY_GROWTH=1")
            endif ()
            if (EMSCRIPTEN_SHARE_DATA)      # MODULE lib type always have this variable enabled
                list (APPEND LINK_FLAGS "--pre-js \"${CMAKE_BINARY_DIR}/Source/pak-loader.js\"")
            endif ()
            if (FLAGGG_TESTING)
                list (APPEND LINK_FLAGS --emrun)
            else ()
                # If not using EMRUN then we need to include the emrun_prejs.js manually in order to process the request parameters as app's arguments correctly

                # message(TARGET_NAME: ${TARGET_NAME})
                # 不是所有项目都需要导出哪些方法的。在新版本Emscripten里面，导出不存在的方法会报错
                if (TARGET_NAME STREQUAL FEEntry)
                    SET(EXP_FUNC "'_main', '_OnConnected', '_OnDisconnected', '_OnErrorCatch', '_OnReciveMessage'")
                    if (EMSCRIPTEN_WX)
                        SET(EXP_FUNC "'_DownloadFinish', ${EXP_FUNC}")
                    endif()
                    SET(EXP_FUNC_ARG "-s EXPORTED_FUNCTIONS=\"[${EXP_FUNC}]\"")
                else()
                    SET(EXP_FUNC_ARG "")
                endif()
                if (EMSCRIPTEN_WX)
                    list (APPEND LINK_FLAGS "--pre-js \"${CMAKE_BINARY_DIR}/prejs.js\" ${EXP_FUNC_ARG}")
                else ()
                    list (APPEND LINK_FLAGS "--pre-js \"${EMSCRIPTEN_ROOT_PATH}/src/emrun_prejs.js\" ${EXP_FUNC_ARG}")
                endif ()
            endif ()
        endif ()
        # These flags are here instead of in the CMAKE_(EXE|MODULE)_LINKER_FLAGS so that they do not interfere with the auto-detection logic during initial configuration
        if (NOT LIB_TYPE OR LIB_TYPE STREQUAL MODULE)
            list (APPEND LINK_FLAGS "-s NO_EXIT_RUNTIME=1 -s ERROR_ON_UNDEFINED_SYMBOLS=1")
            if (EMSCRIPTEN_WASM)
                list (APPEND LINK_FLAGS "-s WASM=1")
            elseif (NOT EMSCRIPTEN_EMCC_VERSION VERSION_LESS 1.38.1)
                # Since version 1.38.1 emcc emits WASM by default, so we need to explicitily turn it off to emits asm.js
                # (See https://github.com/kripken/emscripten/commit/6e5818017d1b2e09e9f7ad22a32e9a191f6f9a3b for more detail)
                list (APPEND LINK_FLAGS "-s WASM=0")
            endif ()
        endif ()
        # Pass EMCC-specific setting to differentiate between main and side modules
        if (FLAGGG_LIB_TYPE STREQUAL MODULE)
            if (${TARGET_NAME} STREQUAL FlagGG)
                # Main module has standard libs statically linked
                list (APPEND LINK_FLAGS "-s MAIN_MODULE=1")
            elseif ((NOT ARG_NODEPS AND NOT LIB_TYPE) OR LIB_TYPE STREQUAL MODULE)
                if (LIB_TYPE)
                    set (SIDE_MODULES ${SIDE_MODULES} ${TARGET_NAME} PARENT_SCOPE)
                endif ()
                # Also consider the executable target as another side module but only this scope
                list (APPEND LINK_FLAGS "-s SIDE_MODULE=1")
                list (APPEND SIDE_MODULES ${TARGET_NAME})
                # Define custom commands for post processing the output file to first load the main module before the side module(s)
                add_custom_command (TARGET ${TARGET_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<STREQUAL:${FLAGGG_LIBRARIES},FlagGG>:$<TARGET_FILE:FlagGG>>$<$<NOT:$<STREQUAL:${FLAGGG_LIBRARIES},FlagGG>>:${FLAGGG_LIBRARIES}> $<TARGET_FILE_DIR:${TARGET_NAME}>
                    COMMAND ${CMAKE_COMMAND} -E $<$<NOT:$<CONFIG:Debug>>:echo> copy_if_different $<$<STREQUAL:${FLAGGG_LIBRARIES},FlagGG>:$<TARGET_FILE:FlagGG>.map>$<$<NOT:$<STREQUAL:${FLAGGG_LIBRARIES},FlagGG>>:${FLAGGG_LIBRARIES}.map> $<TARGET_FILE_DIR:${TARGET_NAME}> $<$<NOT:$<CONFIG:Debug>>:$<ANGLE-R>${NULL_DEVICE}>
                    COMMAND ${CMAKE_COMMAND} -DTARGET_NAME=${TARGET_NAME} -DTARGET_FILE=$<TARGET_FILE:${TARGET_NAME}> -DTARGET_DIR=$<TARGET_FILE_DIR:${TARGET_NAME}> -DHAS_SHELL_FILE=${HAS_SHELL_FILE} -DSIDE_MODULES="${SIDE_MODULES}" -P ${CMAKE_SOURCE_DIR}/CMake/Modules/PostProcessForWebModule.cmake)
                add_make_clean_files ($<TARGET_FILE_DIR:${TARGET_NAME}>/libFlagGG.js $<TARGET_FILE_DIR:${TARGET_NAME}>/libFlagGG.js.map)
            endif ()
        endif ()
        # Pass additional source files to linker with the supported flags, such as: js-library, pre-js, post-js, embed-file, preload-file, shell-file
        foreach (FILE ${SOURCE_FILES})
            get_property (EMCC_OPTION SOURCE ${FILE} PROPERTY EMCC_OPTION)
            if (EMCC_OPTION)
                unset (EMCC_FILE_ALIAS)
                unset (EMCC_EXCLUDE_FILE)
                unset (USE_PRELOAD_CACHE)
                if (EMCC_OPTION STREQUAL embed-file OR EMCC_OPTION STREQUAL preload-file)
                    get_property (EMCC_FILE_ALIAS SOURCE ${FILE} PROPERTY EMCC_FILE_ALIAS)
                    if (EMCC_FILE_ALIAS)
                        set (EMCC_FILE_ALIAS "@\"${EMCC_FILE_ALIAS}\"")
                    endif ()
                    get_property (EMCC_EXCLUDE_FILE SOURCE ${FILE} PROPERTY EMCC_EXCLUDE_FILE)
                    if (EMCC_EXCLUDE_FILE)
                        set (EMCC_EXCLUDE_FILE " --exclude-file \"${EMCC_EXCLUDE_FILE}\"")
                    else ()
                        list (APPEND LINK_DEPENDS ${FILE})
                    endif ()
                    if (EMCC_OPTION STREQUAL preload-file)
                        set (USE_PRELOAD_CACHE " --use-preload-cache")
                    endif ()
                endif ()
                list (APPEND LINK_FLAGS "--${EMCC_OPTION} \"${FILE}\"${EMCC_FILE_ALIAS}${EMCC_EXCLUDE_FILE}${USE_PRELOAD_CACHE}")
            endif ()
        endforeach ()
        # If it is a self-executable shell-file then change the file permission of the output file accordingly
        if (SELF_EXECUTABLE_SHELL AND NOT CMAKE_HOST_WIN32)
            add_custom_command (TARGET ${TARGET_NAME} POST_BUILD COMMAND chmod +x $<TARGET_FILE:${TARGET_NAME}>)
        endif ()
    endif ()
    # Set additional linker dependencies (only work for Makefile-based generator according to CMake documentation)
    if (LINK_DEPENDS)
        string (REPLACE ";" "\;" LINK_DEPENDS "${LINK_DEPENDS}")        # Stringify for string replacement
        list (APPEND TARGET_PROPERTIES LINK_DEPENDS "${LINK_DEPENDS}")  # Stringify with semicolons already escaped
        unset (LINK_DEPENDS)
    endif ()
    # Set additional linker flags
    if (LINK_FLAGS)
        string (REPLACE ";" " " LINK_FLAGS "${LINK_FLAGS}")
        list (APPEND TARGET_PROPERTIES LINK_FLAGS ${LINK_FLAGS})
        unset (LINK_FLAGS)
    endif ()
    if (TARGET_PROPERTIES)
        set_target_properties (${TARGET_NAME} PROPERTIES ${TARGET_PROPERTIES})
        unset (TARGET_PROPERTIES)
    endif ()
    # Create symbolic links in the build tree
    if (NOT ANDROID AND NOT FLAGGG_PACKAGING)
        # Ensure the asset root directory exist before creating the symlinks
        file (MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
        foreach (I ${RESOURCE_DIRS})
            get_filename_component (NAME ${I} NAME)
            if (NOT EXISTS ${CMAKE_BINARY_DIR}/bin/${NAME} AND EXISTS ${I})
                create_symlink (${I} ${CMAKE_BINARY_DIR}/bin/${NAME} FALLBACK_TO_COPY)
            endif ()
        endforeach ()
    endif ()
    # Workaround CMake/Xcode generator bug where it always appends '/build' path element to SYMROOT attribute and as such the items in Products are always rendered as red in the Xcode as if they are not yet built
    if (NOT DEFINED ENV{TRAVIS})
        if (XCODE AND NOT CMAKE_PROJECT_NAME MATCHES ^FlagGG-ExternalProject-)
            file (MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/build)
            get_target_property (LOCATION ${TARGET_NAME} LOCATION)
            string (REGEX REPLACE "^.*\\$\\(CONFIGURATION\\)" $(CONFIGURATION) SYMLINK ${LOCATION})
            get_filename_component (DIRECTORY ${SYMLINK} PATH)
            add_custom_command (TARGET ${TARGET_NAME} POST_BUILD
                COMMAND mkdir -p ${DIRECTORY} && ln -sf $<TARGET_FILE:${TARGET_NAME}> ${DIRECTORY}/$<TARGET_FILE_NAME:${TARGET_NAME}>
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/build)
        endif ()
    endif ()
endmacro()

# Macro for setting up a test case
macro (setup_test)
    if (FLAGGG_TESTING)
        cmake_parse_arguments (ARG "" NAME OPTIONS ${ARGN})
        if (NOT ARG_NAME)
            set (ARG_NAME ${TARGET_NAME})
        endif ()
        list (APPEND ARG_OPTIONS -timeout ${FLAGGG_TEST_TIMEOUT})
        if (WEB)
            if (EMSCRIPTEN)
                math (EXPR EMRUN_TIMEOUT "2 * ${FLAGGG_TEST_TIMEOUT}")
                add_test (NAME ${ARG_NAME} COMMAND ${EMRUN} --browser ${EMSCRIPTEN_EMRUN_BROWSER} --timeout ${EMRUN_TIMEOUT} --kill_exit ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}.html ${ARG_OPTIONS})
            endif ()
        else ()
            add_test (NAME ${ARG_NAME} COMMAND ${TARGET_NAME} ${ARG_OPTIONS})
        endif ()
    endif ()
endmacro ()

# Macro for setting up linter
macro (setup_lint)
    if (FLAGGG_LINT)
        find_program (CLANG_TIDY clang-tidy NO_CMAKE_FIND_ROOT_PATH)
        if (CLANG_TIDY)
            set (FLAGGG_PCH 0)
            set (CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY} -config=)
            set (CMAKE_EXPORT_COMPILE_COMMANDS 1)
        else ()
            message (FATAL_ERROR "Ensure clang-tidy host tool is installed and can be found in the PATH environment variable.")
        endif ()
    endif ()
endmacro ()

# Macro for resetting the linter (intended to be called in a child scope where its parent scope has the linter setup)
macro (reset_lint)
    unset (CMAKE_CXX_CLANG_TIDY)
    unset (CMAKE_EXPORT_COMPILE_COMMANDS)
endmacro ()
