SET(CMAKE_LIBRARY_PATH
${CMAKE_SYSTEM_LIBRARY_PATH}
)

FIND_LIBRARY(LIB_ASOUND
NAME asound
PATHS ${CMAKE_LIBRARY_PATH}
)

if(LIB_ASOUND)

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

SET( OMM_AVSTREAM_ALSASINK_SRCS
AlsaAudioSink.cpp
)

ADD_LIBRARY( ommaudiosink-alsa
SHARED
${OMM_AVSTREAM_ALSASINK_SRCS}
)

TARGET_LINK_LIBRARIES( ommaudiosink-alsa
${LIB_ASOUND}
)

INSTALL( TARGETS ommaudiosink-alsa
LIBRARY DESTINATION lib/omm
)

endif(LIB_ASOUND)
