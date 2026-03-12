add_library(sound MODULE
			stbvorbis_impl.cpp
			sound.cpp
			SoundManager.cpp
			SoundPlayer.cpp
			SoundShader.cpp)
if(CMAKE_MAJOR_VERSION LESS 3 OR (CMAKE_MAJOR_VERSION EQUAL 3 AND CMAKE_MINOR_VERSION LESS 11))
	if(MSVC)
		set_source_files_properties(stbvorbis_impl.cpp PROPERTIES COMPILE_FLAGS "/Ox")
	else()
		set_source_files_properties(stbvorbis_impl.cpp PROPERTIES COMPILE_FLAGS "-O2")
	endif()
else()
	if(MSVC)
		set_source_files_properties(stbvorbis_impl.cpp PROPERTIES COMPILE_OPTIONS "/Ox")
	else()
		set_source_files_properties(stbvorbis_impl.cpp PROPERTIES COMPILE_OPTIONS "-O2")
	endif()
endif()
target_link_libraries(sound PUBLIC wxutil ${AL_LIBRARIES})
set_target_properties(
	sound PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${DR_STAGING_MODULESDIR}"
)
