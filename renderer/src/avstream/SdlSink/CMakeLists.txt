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

SET( OMM_AVSTREAM_SDL_VIDEOSINK_SRCS
SdlVideoSink.cpp
)

SET( OMM_AVSTREAM_SDL_AUDIOSINK_SRCS
SdlAudioSink.cpp
)

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
${OMM_AVSTREAM_SDL_VIDEOSINK_SRCS}
)

ADD_LIBRARY( ommaudiosink-sdl
SHARED
${OMM_AVSTREAM_SDL_AUDIOSINK_SRCS}
)

TARGET_LINK_LIBRARIES( ommvideosink-sdl
${SDL_LIBRARY}
)

TARGET_LINK_LIBRARIES( ommaudiosink-sdl
${SDL_LIBRARY}
)

INSTALL( TARGETS ommvideosink-sdl
LIBRARY DESTINATION lib/omm
)

else(SDL_FOUND)
message(STATUS "SDL not found")
endif(SDL_FOUND)
