include_external_msproject(
    EditableGameEntryPoint ${CMAKE_CURRENT_SOURCE_DIR}/EditableGameEntryPoint.csproj
    TYPE FAE04EC0-301F-11D3-BF4B-00C04F79EFBC)
set_target_properties (EditableGameEntryPoint PROPERTIES FOLDER "Application/Game")
add_dependencies(EditableGameEntryPoint Game)
