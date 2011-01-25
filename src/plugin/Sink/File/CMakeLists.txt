if(TARGET ommavstream)
INCLUDE_DIRECTORIES(
${omm_SOURCE_DIR}/lib
)

ADD_LIBRARY( ommvideosink-ppm
SHARED
PpmVideoSink.cpp
)

TARGET_LINK_LIBRARIES( ommvideosink-ppm
ommavstream
)

ADD_LIBRARY( ommaudiosink-pcm
SHARED
PcmAudioSink.cpp
)

TARGET_LINK_LIBRARIES( ommaudiosink-pcm
ommavstream
)

INSTALL( TARGETS ommvideosink-ppm ommaudiosink-pcm
LIBRARY DESTINATION lib/omm
)

else(TARGET ommavstream)
message(STATUS "libommavstream not found")
endif(TARGET ommavstream)
