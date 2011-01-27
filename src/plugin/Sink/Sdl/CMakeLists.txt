find_package(SDL)

if(SDL_FOUND)
message(STATUS "SDL found in " ${SDL_INCLUDE_DIR} ", " ${SDL_LIBRARY})
INCLUDE_DIRECTORIES(
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

else(SDL_FOUND)
message(STATUS "SDL not found")
endif(SDL_FOUND)
