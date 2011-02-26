find_package(SDL)

if(SDL_FOUND)

include_directories(
${SDL_INCLUDE_DIR}
)

if(BUILD_PLUGINS)

add_library(ommplugin-videosink-sdl
SHARED
SdlVideoSink.cpp
)

target_link_libraries(ommplugin-videosink-sdl
${SDL_LIBRARY}
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
)

set_target_properties(ommplugin-videosink-sdl
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

add_library(ommplugin-audiosink-sdl
SHARED
SdlAudioSink.cpp
)

target_link_libraries(ommplugin-audiosink-sdl
${SDL_LIBRARY}
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
)

set_target_properties(ommplugin-audiosink-sdl
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-videosink-sdl ommplugin-audiosink-sdl
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)

endif(BUILD_PLUGINS)
endif(SDL_FOUND)
