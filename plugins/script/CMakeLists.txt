add_library(script MODULE
			interfaces/BrushInterface.cpp
			interfaces/CameraInterface.cpp
			interfaces/CommandSystemInterface.cpp
			interfaces/DeclarationManagerInterface.cpp
			interfaces/DialogInterface.cpp
			interfaces/EClassInterface.cpp
			interfaces/EntityInterface.cpp
			interfaces/FileSystemInterface.cpp
			interfaces/FxManagerInterface.cpp
			interfaces/GameInterface.cpp
			interfaces/GridInterface.cpp
			interfaces/LayerInterface.cpp
			interfaces/MapInterface.cpp
			interfaces/MathInterface.cpp
			interfaces/ModelInterface.cpp
			interfaces/PatchInterface.cpp
			interfaces/RadiantInterface.cpp
			interfaces/RegistryInterface.cpp
			interfaces/SceneGraphInterface.cpp
			interfaces/SelectionGroupInterface.cpp
			interfaces/SelectionInterface.cpp
			interfaces/SelectionSetInterface.cpp
			interfaces/ShaderSystemInterface.cpp
			interfaces/SkinInterface.cpp
			interfaces/SoundInterface.cpp
			LuaModule.cpp
			SceneNodeBuffer.cpp
			ScriptCommand.cpp
			ScriptingSystem.cpp
			ScriptModule.cpp)
if(CMAKE_MAJOR_VERSION LESS 3 OR (CMAKE_MAJOR_VERSION EQUAL 3 AND CMAKE_MINOR_VERSION LESS 11))
	if(MSVC)
		set_source_files_properties(lua_impl.cpp PROPERTIES COMPILE_FLAGS "/Ox")
	else()
		set_source_files_properties(lua_impl.cpp PROPERTIES COMPILE_FLAGS "-O2")
	endif()
else()
	if(MSVC)
		set_source_files_properties(lua_impl.cpp PROPERTIES COMPILE_OPTIONS "/Ox")
	else()
		set_source_files_properties(lua_impl.cpp PROPERTIES COMPILE_OPTIONS "-O2")
	endif()
endif()
set_target_properties(script PROPERTIES CXX_VISIBILITY_PRESET hidden)
target_link_libraries(script PUBLIC math scene)
set_target_properties(
	script PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${DR_STAGING_MODULESDIR}"
)

# Enable precompiled header for script plugin
if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16.0")
	target_precompile_headers(script PRIVATE "precompiled.h")
endif()
