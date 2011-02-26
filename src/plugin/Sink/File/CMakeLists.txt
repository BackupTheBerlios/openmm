if(BUILD_PLUGINS)

add_library(ommplugin-videosink-ppm
SHARED
PpmVideoSink.cpp
)

target_link_libraries(ommplugin-videosink-ppm
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
)

set_target_properties(ommplugin-videosink-ppm
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

add_library(ommplugin-audiosink-pcm
SHARED
PcmAudioSink.cpp
)

target_link_libraries(ommplugin-audiosink-pcm
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
)

set_target_properties(ommplugin-audiosink-pcm
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-videosink-ppm ommplugin-audiosink-pcm
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)

endif(BUILD_PLUGINS)
