add_library(ommvideosink-ppm
SHARED
PpmVideoSink.cpp
)

target_link_libraries(ommvideosink-ppm
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
)

add_library(ommaudiosink-pcm
SHARED
PcmAudioSink.cpp
)

target_link_libraries(ommaudiosink-pcm
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
)

install(TARGETS ommvideosink-ppm ommaudiosink-pcm
LIBRARY DESTINATION lib/omm
)

