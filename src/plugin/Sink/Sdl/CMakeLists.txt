find_package(SDL)

if(SDL_FOUND)

include_directories(
${SDL_INCLUDE_DIR}
)

add_library(ommvideosink-sdl
SHARED
SdlVideoSink.cpp
)

target_link_libraries(ommvideosink-sdl
${SDL_LIBRARY}
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
)

add_library(ommaudiosink-sdl
SHARED
SdlAudioSink.cpp
)

target_link_libraries(ommaudiosink-sdl
${SDL_LIBRARY}
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
)

install(TARGETS ommvideosink-sdl ommaudiosink-sdl
LIBRARY DESTINATION lib/omm
)

endif(SDL_FOUND)
