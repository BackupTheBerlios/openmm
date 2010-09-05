if(TARGET ommavstream)
INCLUDE_DIRECTORIES(
${omm_SOURCE_DIR}/lib
)

# ADD_DEFINITIONS(
# -malign-double
# )

SET( OMM_AV_STREAM_PPM_VIDEOSINK_SRCS
PpmVideoSink.cpp
)

ADD_LIBRARY( ommvideosink-ppm
SHARED
${OMM_AV_STREAM_PPM_VIDEOSINK_SRCS}
)

SET( OMM_AV_STREAM_PPM_VIDEOSINK_SRCS
PpmVideoSinkOld.cpp
)

ADD_LIBRARY( ommvideosink-ppmold
SHARED
${OMM_AV_STREAM_PPM_VIDEOSINK_SRCS}
)

SET( OMM_AV_STREAM_PCM_AUDIOSINK_SRCS
PcmAudioSink.cpp
)

ADD_LIBRARY( ommaudiosink-pcm
SHARED
${OMM_AV_STREAM_PCM_AUDIOSINK_SRCS}
)

INSTALL( TARGETS ommvideosink-ppm ommaudiosink-pcm
LIBRARY DESTINATION lib/omm
)

else(TARGET ommavstream)
message(STATUS "libommavstream not found")
endif(TARGET ommavstream)
