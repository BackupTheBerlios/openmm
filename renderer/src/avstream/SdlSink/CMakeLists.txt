FIND_PACKAGE(SDL)

if(SDL_FOUND)
message(STATUS "SDL found in " ${SDL_INCLUDE_DIR} ", " ${SDL_LIBRARY})
INCLUDE_DIRECTORIES(
${SDL_INCLUDE_DIR}
)

SET(CMAKE_LIBRARY_PATH
${CMAKE_SYSTEM_LIBRARY_PATH}
)

if(TARGET omm)
INCLUDE_DIRECTORIES(
${omm_SOURCE_DIR}/lib
)
endif(TARGET omm)


IF (AVSTREAM_ALIGN_DOUBLE)
ADD_DEFINITIONS(
# FIXME: on some (32-bit intel?) systems, ffmpeg is compiled with -malign-double
# we need to set this flag as it set in the installed ffmpeg libs, otherwise
# we get a crash in ffmpeg::av_metadata_get()
-malign-double
)
ENDIF (AVSTREAM_ALIGN_DOUBLE)

ADD_LIBRARY( ommvideosink-sdl
SHARED
SdlVideoSink.cpp
)

TARGET_LINK_LIBRARIES( ommvideosink-sdl
${SDL_LIBRARY}
ommavstream
)

ADD_LIBRARY( ommaudiosink-sdl
SHARED
SdlAudioSink.cpp
)

TARGET_LINK_LIBRARIES( ommaudiosink-sdl
${SDL_LIBRARY}
ommavstream
)

INSTALL( TARGETS ommvideosink-sdl ommaudiosink-sdl
LIBRARY DESTINATION lib/omm
)

else(SDL_FOUND)
message(STATUS "SDL not found")
endif(SDL_FOUND)
